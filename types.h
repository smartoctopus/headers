#if !defined(TYPES_H_)
#define TYPES_H_

#include "defines.h"

#if defined(COMPILER_MSVC) && _MSC_VER >= 1300
typedef unsigned __int8 u8;
typedef signed __int8 i8;
typedef unsigned __int16 u16;
typedef signed __int16 i16;
typedef unsigned __int32 u32;
typedef signed __int32 i32;
typedef unsigned __int64 u64;
typedef signed __int64 i64;
#else
typedef unsigned char u8;
typedef signed char i8;
typedef unsigned short int u16;
typedef signed short int i16;
typedef unsigned int u32;
typedef signed int i32;
#if WORDSIZE == 64
typedef unsigned long int u64;
typedef signed long int i64;
#else
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
typedef unsigned long long int u64;
typedef signed long long int i64;
#else
#error Could not define int64 types. Consider compiling with a C99 compliant compiler
#endif
#endif
#endif

typedef i8 least_i8;
typedef i16 least_i16;
typedef i32 least_i32;
typedef i64 least_i64;
typedef u8 least_u8;
typedef u16 least_u16;
typedef u32 least_u32;
typedef u64 least_u64;

typedef i8 fast_i8;
#if WORDSIZE == 64
typedef i64 fast_i16;
typedef i64 fast_i32;
typedef i64 fast_i64;
#else
typedef i32 fast_i16;
typedef i32 fast_i32;
typedef i64 fast_i64;
#endif
typedef u8 fast_u8;
#if WORDSIZE == 64
typedef u64 fast_u16;
typedef u64 fast_u32;
typedef u64 fast_u64;
#else
typedef u32 fast_u16;
typedef u32 fast_u32;
typedef u64 fast_u64;
#endif

typedef i64 imax;
typedef u64 umax;

#if defined(_WIN64) || (WORDSIZE == 64)
typedef i64 intptr;
typedef u64 uintptr;
#elif defined(_WIN32)
  #if !defined(_W64)
    #if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
      #define _W64 __w64
    #else
      #define _W64
    #endif
  #endif
typedef _W64 signed int intptr;
typedef _W64 unsigned int uintptr;
#else
typedef i32 intptr;
typedef u32 uintptr;
#endif

typedef float f32;
typedef double f64;

typedef u64 usize;
typedef i64 isize;

/* Unicode */
typedef i32 Rune;
#if !defined(UNICODE_VALUES)
#define UNICODE_VALUES
#define RUNE_INVALID cast(Rune)(0xfffd)
#define RUNE_MAX cast(Rune)(0x0010ffff)
#define RUNE_MIN cast(Rune)(0xfeff)
#define RUNE_EOF cast(Rune)(-1)
#endif

#if !defined(MIN_MAX)
#define MIN_MAX 1
#define U8_MIN 0u
#define U8_MAX 0xffu
#define I8_MIN (-0x7f - 1)
#define I8_MAX 0x7f
#define U16_MIN 0u
#define U16_MAX 0xffffu
#define I16_MIN (-0x7fff - 1)
#define I16_MAX 0x7fff
#define U32_MIN 0u
#define U32_MAX 0xffffffffu
#define I32_MIN (-0x7fffffff - 1)
#define I32_MAX 0x7fffffff
#if WORDSIZE == 64
  #define U64_MIN 0ul
  #define U64_MAX 0xfffffffffffffffful
  #define I64_MIN (-0x7fffffffffffffffl - 1)
  #define I64_MAX 0x7fffffffffffffffl
#else
  #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #define U64_MIN 0ull
    #define U64_MAX 0xffffffffffffffffull
    #define I64_MIN (-0x7fffffffffffffffll - 1)
    #define I64_MAX 0x7fffffffffffffffll
  #else
    #error Could not define 64-bit min and max macros. Consider compiling with a C99 compliant compiler
  #endif
#endif
#define F32_MIN 1.17549435e-38f
#define F32_MAX 3.40282347e+38f
#define F64_MIN 2.2250738585072014e-308
#define F64_MAX 1.7976931348623157e+308
#endif

/* Bool */
#if !defined(__cplusplus)
  #if (defined(_MSC_VER) && _MSC_VER < 1800) || (!defined(_MSC_VER) && !defined(__STDC_VERSION__))
    #if !defined(true)
      #define true (0 == 0)
    #endif
    #if !defined(false)
      #define false (0 != 0)
    #endif
    typedef u8 bool;
  #else
    #include <stdbool.h>
  #endif
#endif

/* Null */
#undef NULL
#if defined(__cplusplus)
  #if __cplusplus >= 201103L
    #define NULL nullptr
  #else
    #define NULL 0
  #endif
#else
  #define NULL ((void*)0)
#endif

#endif /* TYPES_H_ */