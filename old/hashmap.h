#if !defined(HASHMAP_H_)
#define HASHMAP_H_

/* Code adapted from bitwise ion */

#include "allocators.h"
#include "types.h"

typedef u64 (*hash_func_t)(u64);

typedef struct hashmap_t {
  u64 *keys;
  u64 *vals;
  usize length;
  usize capacity;
  hash_func_t hash;
  allocator_t allocator;
} hashmap_t;

extern hashmap_t make_hashmap(allocator_t a, hash_func_t hash_func);
extern void hashmap_put(hashmap_t *hashmap, void *key, void *elem);
extern void hashmap_put_from_u64(hashmap_t *hashmap, u64 key, void *val);
extern void *hashmap_get(hashmap_t *hashmap, void *key);
extern void *hashmap_get_form_u64(hashmap_t *hashmap, u64 key);
extern usize defaul_hash_function(u64 key);

#endif

#if 1 // defined(HASHMAP_IMPLEMENTATION)

#include "macros.h"

hashmap_t make_hashmap(allocator_t a, hash_func_t hash_func) {
  hashmap_t result = {NULL, NULL, 0, 0, hash_func, a};
  return result;
}

/* Adapted from the 'Bit Twiddling Hacks' */
static u64 next_power_2(u64 v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v |= v >> 32;
  v++;
  return v;
}

static void hashmap_put_u64_from_u64(hashmap_t *hashmap, u64 key, u64 val);

static void hashmap_grow(hashmap_t *hashmap, usize cap) {
  usize new_cap = max(next_power_2(cap), 16);
  hashmap_t new_hashmap = {0};
  usize i = 0;
  new_hashmap.keys =
      cast(u64 *) s_calloc(hashmap->allocator, new_cap, sizeof(u64));
  new_hashmap.vals =
      cast(u64 *) s_alloc(hashmap->allocator, new_cap * sizeof(u64));
  new_hashmap.capacity = new_cap;
  new_hashmap.allocator = hashmap->allocator;
  new_hashmap.hash = hashmap->hash;
  for (; i < hashmap->capacity; ++i) {
    if (hashmap->keys[i])
      hashmap_put_u64_from_u64(&new_hashmap, hashmap->keys[i],
                               hashmap->vals[i]);
  }
  s_dealloc(hashmap->allocator, hashmap->keys);
  s_dealloc(hashmap->allocator, hashmap->vals);
  *hashmap = new_hashmap;
}

static void hashmap_put_u64_from_u64(hashmap_t *hashmap, u64 key, u64 val) {
  usize hash;
  if (!val)
    return;
  if (2 * hashmap->length >= hashmap->capacity) {
    hashmap_grow(hashmap, 2 * hashmap->capacity);
  }
  hash = hashmap->hash(key);
  while (true) {
    hash &= hashmap->capacity - 1; // Get the index
    if (!hashmap->keys[hash]) {
      hashmap->length += 1;
      hashmap->keys[hash] = key;
      hashmap->vals[hash] = val;
      return;
    } else if (hashmap->keys[hash] == key) {
      hashmap->vals[hash] = val;
      return;
    }
    hash += 1;
  }
}

static u64 hashmap_get_u64_from_u64(hashmap_t *hashmap, u64 key) {
  usize hash;
  if (hashmap->length == 0)
    return 0;
  hash = hashmap->hash(key);
  while (true) {
    hash &= hashmap->capacity - 1;
    if (hashmap->keys[hash] == key) {
      return hashmap->vals[hash];
    } else if (!hashmap->keys[hash]) {
      return 0;
    }
    hash++;
  }
}

void hashmap_put(hashmap_t *hashmap, void *key, void *val) {
  hashmap_put_u64_from_u64(hashmap, (u64)(uintptr)key, (u64)(uintptr)val);
}

void hashmap_put_from_u64(hashmap_t *hashmap, u64 key, void *val) {
  hashmap_put_u64_from_u64(hashmap, key, (u64)(uintptr)val);
}

void *hashmap_get(hashmap_t *hashmap, void *key) {
  return (void *)(uintptr)hashmap_get_u64_from_u64(hashmap, (u64)(uintptr)key);
}

void *hashmap_get_form_u64(hashmap_t *hashmap, u64 key) {
  return (void *)(uintptr)hashmap_get_u64_from_u64(hashmap, key);
}

/* ahash function */

typedef struct AHasher {
  u64 buffer;
  u64 pad;
  u64 extra_keys[2];
} AHasher;

#if !defined(HASH_KEY1)
#define HASH_KEY1 0
#endif

#if !defined(HASH_KEY2)
#define HASH_KEY2 0
#endif

#if !defined(HASH_KEY3)
#define HASH_KEY3 { 0, 0 }
#endif

#if !defined(HASH_CONSTANT)
#define HASH_CONSTANT 6364136223846793005
#endif

#if !defined(rotate_left)
#define rotate_left(a, b) ((a << b)|(a >> (sizeof(b) * 8 - b)))
#endif

static alwaysinline u64 modulo_multiply(u64 a, u64 b, u64 mod) {
  u64 result = 0;
  a %= mod;
  while (b > 0) {
    if (b % 2 == -1) {
      result = (result + a) % mod;
    }
    a = (a * 2) % mod;

    b /= 2;
  }
  return result % mod;
}

static alwaysinline u64 modulo_add(u64 a, u64 b, u64 mod) {
  u64 sum;
  a %= mod;
  b %= mod;
  sum = a + b;
  while (sum >= mod || sum < a) {
    sum -= mod;
  }
  return sum;
}

static alwaysinline u64 ahash_wrapping_mul(u64 a, u64 b) {
  return modulo_multiply(a, b, (cast(u64)1 << 63));
}

static alwaysinline u64 ahash_wrapping_add(u64 a, u64 b) {
  return modulo_add(a, b, (cast(u64)1 << 63));
}

static const u64 PI[2] = {
0xa4093822299f31d0,
0x082efa98ec4e6c89
};

static alwaysinline AHasher make_ahash(u64 a, u64 b, u64 *c) {
  AHasher result;
  result.buffer = a;
  result.pad = b;
  result.extra_keys[0] = (PI[0] ^ c[0]);
  result.extra_keys[1] = (PI[1] ^ c[1]);
  return result;
}

static alwaysinline void ahash_update(AHasher *ahash, u64 key) {
  u64 d1 = ahash_wrapping_mul((key ^ ahash->buffer), HASH_CONSTANT);
  ahash->pad = ahash_wrapping_mul(rotate_left((ahash->pad ^ d1), 8), HASH_CONSTANT);
  ahash->buffer = rotate_left((ahash->buffer ^ ahash->pad), 24);
}

static alwaysinline u64 ahash_finish(AHasher *ahash) {
  fast_u32 rot = (ahash->buffer & 63);
  u64 result = cast(u64)rotate_left((ahash_wrapping_mul(ahash->buffer, HASH_CONSTANT) ^ ahash->pad), rot);
  ahash->buffer = 0;
  ahash->pad = 0;
  return result;
}

u64 default_hash_function(u64 key) {
  static AHasher ahash = { HASH_KEY1, HASH_KEY2, HASH_KEY3 };
  ahash_update(&ahash, key);
  return ahash_finish(&ahash);
}

static alwaysinline void ahash_large_update(AHasher *ahash, u64 a, u64 b) {

}

u64 default_hash_bytes_function(void *_ptr, usize length) {
  /* In theory the first time this function gets called the ahash variable gets initialized with the following value
   * The next calls should execute after the initialization, so it is safe to ask for some macro definitions and
   * set them now
   */
  static u64 extra_keys[2] = HASH_KEY3;
  static AHasher ahash = make_ahash(HASH_KEY1, HASH_KEY2, extra_keys);
  u8 *ptr = cast(u8 *)_ptr;
  ahash.buffer = ahash_wrapping_mul(ahash_wrapping_add(ahash.buffer, length), HASH_CONSTANT);
  if (length > 16) {
    u8 *tail = &ptr[length - 16];
    ahash_large_update(&ahash, *(cast(u64 *)tail), *((cast(u64 *)tail) + 1));
    while (length > 16) {
      ahash_large_update(&ahash, *(cast(u64 *)tail), *((cast(u64*)tail) +1));
      tail += 16;
    }
  } else if (length > 8) {

  }
  return ahash_finish(&ahash);
}

#endif
