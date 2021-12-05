#if !defined(STRING_H_)
#define STRING_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "macros.h"
#include "types.h"
#include "allocators.h"

#if !defined(CHAR_BIT)
#define CHAR_BIT (sizeof(char) * 8)
#endif

#if !defined(SSO_CAPACITY)
#define SSO_CAPACITY (sizeof(struct NonSSO_String) / sizeof(char) - 1)
#endif

typedef struct NonSSO_String {
  u8* str;
  usize size;
  usize capacity;
} NonSSO_String;

typedef struct SSO_String {
  u8 str[sizeof(struct NonSSO_String) - 1];
  u8 size;
} SSO_String;

typedef union String {
  NonSSO_String non_sso;
  SSO_String sso;
} String;

typedef struct NonSSO_Data {
  usize capacity;
  usize size;
} NonSSO_Data;

/* Private API */
static u8* most_sig_byte(void* obj, usize size);
static bool lsb(u8 byte, usize n);
static bool msb(u8 byte, usize n);
static void set_lsb(u8* byte, bool bit, usize n);
static void set_msb(u8* byte, bool bit, usize n);
static void String_set_sso_size(String* str, usize size);
static void String_set_non_sso_data(String* str, usize size, usize capacity);
static bool String_is_sso(String* str);
static usize String_get_sso_size(String* str);
static NonSSO_Data String_get_non_sso_data(String* str);
static usize u8_strlen(u8* str);

/* Public API */
String String_alloc_length(u8* string, usize size);
String String_alloc(u8* string);
void String_dealloc(String* str);
u8* String_to_cstr(String* str);
usize String_length(String* str);
usize String_capacity(String* str);

#endif /* STRING_H_ */

#if defined(STRING_IMPLEMENTATION)
#undef STRING_IMPLEMENTATION
/* Private API */
static u8* most_sig_byte(void* obj, usize size) {
  u8* ptr = cast(u8*)obj;
  return ptr + size - 1;
}

static bool lsb(u8 byte, usize n) {
  return byte & (1u << n);
}

static bool msb(u8 byte, usize n) {
  return byte & (1u << (CHAR_BIT - n - 1));
}

static void set_msb(u8* byte, bool bit, usize n) {
  if(bit) {
    (*byte) |= 1u << (CHAR_BIT - n - 1);
  } else {
    (*byte) &= ~(1u << (CHAR_BIT - n - 1));
  }
}

static void set_lsb(u8* byte, bool bit, usize n) {
  if(bit) {
    (*byte) |= 1u << n;
  } else {
    (*byte) &= ~(1u << n);
  }
}

static void String_set_sso_size(String* str, usize size) {
  str->sso.size = cast(u8)(SSO_CAPACITY - size) << 2;
}

static void String_set_non_sso_data(String* str, usize size, usize capacity) {
  u8* size_hsb = most_sig_byte(&size, sizeof(size));
  const bool size_high_bit = msb(*size_hsb, 0);

  u8* cap_hsb = most_sig_byte(&capacity, sizeof(capacity));
  const bool cap_high_bit = msb(*size_hsb, 0);
  const bool cap_sec_high_bit = msb(*size_hsb, 1);

  set_msb(size_hsb, cap_sec_high_bit, 0);

  (*cap_hsb) <<= 2;
  set_lsb(cap_hsb, cap_high_bit, 0);
  set_lsb(cap_hsb, !size_high_bit, 1);

  str->non_sso.size = size;
  str->non_sso.capacity = capacity;
}

static bool String_is_sso(String* str) {
  return !lsb(str->sso.size, 0) && !lsb(str->sso.size, 1);
}


static usize String_get_sso_size(String* str) {
  return SSO_CAPACITY - ((str->sso.size >> 2) & 63u);
}

static NonSSO_Data String_get_non_sso_data(String* str) {
  NonSSO_Data result = {0, 0};
  usize size = str->non_sso.size;
  usize capacity = str->non_sso.capacity;

  u8* size_hsb = most_sig_byte(str, sizeof(*str));
  u8* cap_hsb = most_sig_byte(str, sizeof(*str));

  const bool cap_high_bit = lsb(*cap_hsb, 0);
  const bool size_high_bit = !lsb(*cap_hsb, 1);
  const bool cap_sec_high_bit = msb(*size_hsb, 0);

  set_msb(size_hsb, size_high_bit, 0);

  (*cap_hsb) >>= 2;
  set_msb(cap_hsb, cap_high_bit, 0);
  set_msb(cap_hsb, cap_sec_high_bit, 1);

  result.size = size;
  result.capacity = capacity;
  return result;
}

static usize u8_strlen(u8* str) {
  int i = 0;
  while(str[i] != '\0') i += 1;
  return i;
}

/* Public API */
String String_alloc_length(u8* string, usize size) {
  String result;
  if(size <= SSO_CAPACITY) {
    memcpy(result.sso.str, string, size);
    result.sso.str[size] = '\0';
    String_set_sso_size(&result, size);
  } else {
    result.non_sso.str = xmalloc(size + 1);
    memcpy(result.non_sso.str, string, size);
    result.non_sso.str[size + 1] = '\0';
    String_set_non_sso_data(&result, size, size);
  }
  return result;
}

alwaysinline String String_alloc(u8* string) {
  return String_alloc_length(string, u8_strlen(string));
}

alwaysinline void String_dealloc(String* str) {
  if(!String_is_sso(str)) {
    xfree(str->non_sso.str);
  }
}

alwaysinline u8* String_to_cstr(String* str) {
  if(String_is_sso(str)) {
    return str->sso.str;
  } else {
    return str->non_sso.str;
  }
}

alwaysinline usize String_length(String* str) {
  if(String_is_sso(str)) {
    return String_get_sso_size(str);
  } else {
    return String_get_non_sso_data(str).size; 
  }
}

alwaysinline usize String_capacity(String* str) {
  if(String_is_sso(str)) {
    return sizeof(*str) - 1;
  } else {
    return String_get_non_sso_data(str).capacity;
  }
}
#endif /* STRING_IMPLEMENTATION */