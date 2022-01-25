#if !defined(HEADER_H_)
#define HEADER_H_

#if defined(__cplusplus)
extern "C" {
#endif

/* Includes */
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(OS_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#if !defined(__USE_MISC)
#define __USE_MISC 1
#endif
#include <sys/mman.h>
#endif

#if !defined(HEADER_DEF)
#define HEADER_DEF extern
#endif

/* CPU Macros */
#if defined(_WIN64) || defined(__x86_64__) || defined(_M_X64) ||               \
    defined(__64BIT__) || defined(__powerpc64__) || defined(__ppc64__)
#if !defined(ARCH_64BIT)
#define ARCH_64BIT 1
#endif
#else
#if !defined(ARCH_32BIT)
#define ARCH_32BIT 1
#endif
#endif

#if !defined(WORDSIZE)
#if defined(__x86_64__) && !defined(__ILP32__)
#define WORDSIZE 64
#else
#define WORDSIZE 32
#endif
#endif

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) ||                \
    defined(__x86_64__)
#if !defined(CPU_X86)
#define CPU_X86 1
#endif
#elif defined(_M_PPC) || defined(__powerpc__) || defined(__powerpc64__)
#if !defined(CPU_POWERPC)
#define CPU_POWERPC 1
#endif
#elif defined(__arm__)
#if !defined(CPU_ARM)
#define CPU_ARM 1
#endif
#elif defined(__MIPSEL__) || defined(__mips_isa_rev)
#if !defined(CPU_MIPS)
#define CPU_MIPS 1
#endif
#else
#error Unknown CPU
#endif

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) ||                \
    defined(__x86_64__) || defined(__arm__) || defined(__MIPSEL__) ||          \
    defined(__mips_isa_rev)
#if !defined(CACHE_LINE_SIZE)
#define CACHE_LINE_SIZE 64
#endif
#elif defined(_M_PPC) || defined(__powerpc__) || defined(__powerpc64__)
#if !defined(CACHE_LINE_SIZE)
#define CACHE_LINE_SIZE 128
#endif
#else
#error Unknown CPU
#endif

/* Types */
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

/* Print specifiers */
#if defined(COMPILER_MSVC) && _MSC_VER >= 1300
#define PRId8 "%d"
#define PRIi8 "%i"
#define PRIdLEAST8 "%d"
#define PRIiLEAST8 "%i"
#define PRIdFAST8 "%d"
#define PRIiFAST8 "%i"

#define PRId16 "%hd"
#define PRIi16 "%hi"
#define PRIdLEAST16 "%hd"
#define PRIiLEAST16 "%hi"
#if WORDSIZE == 64
#define PRIdFAST16 "%I64d"
#define PRIiFAST16 "%I64i"
#else
#define PRIdFAST16 "%I32d"
#define PRIiFAST16 "%I32i"
#endif

#define PRId32 "%I32d"
#define PRIi32 "%I32i"
#define PRIdLEAST32 "%I32d"
#define PRIiLEAST32 "%I32i"
#if WORDSIZE == 64
#define PRIdFAST32 "%I64d"
#define PRIiFAST32 "%I64i"
#else
#define PRIdFAST32 "%I32d"
#define PRIiFAST32 "%I32i"
#endif

#define PRId64 "%I64d"
#define PRIi64 "%I64i"
#define PRIdLEAST64 "%I64d"
#define PRIiLEAST64 "%I64i"
#define PRIdFAST64 "%I64d"
#define PRIiFAST64 "%I64i"

#define PRIdMAX "%I64d"
#define PRIiMAX "%I64i"

#define PRIdPTR "%p"
#define PRIiPTR "%p"

#else /* Here we probably have a C99 compliant compilers? */

#define PRId8 "%d"
#define PRIi8 "%i"
#define PRIdLEAST8 "%d"
#define PRIiLEAST8 "%i"
#define PRIdFAST8 "%d"
#define PRIiFAST8 "%i"

#define PRId16 "%hd"
#define PRIi16 "%hi"
#define PRIdLEAST16 "%hd"
#define PRIiLEAST16 "%hi"
#if WORDSIZE == 64
#define PRIdFAST16 "%ld"
#define PRIiFAST16 "%li"
#else
#define PRIdFAST16 "%hd"
#define PRIiFAST16 "%hi"
#endif

#define PRId32 "%d"
#define PRIi32 "%i"
#define PRIdLEAST32 "%d"
#define PRIiLEAST32 "%i"
#if WORDSIZE == 64
#define PRIdFAST32 "%ld"
#define PRIiFAST32 "%li"
#else
#define PRIdFAST32 "%d"
#define PRIiFAST32 "%i"
#endif

#if WORDSIZE == 64
#define PRId64 "%ld"
#define PRIi64 "%li"
#define PRIdLEAST64 "%ld"
#define PRIiLEAST64 "%li"
#define PRIdFAST64 "%ld"
#define PRIiFAST64 "%li"

#define PRIdMAX "%ld"
#define PRIiMAX "%li"
#else
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define PRId64 "%lld"
#define PRIi64 "%lli"
#define PRIdLEAST64 "%lld"
#define PRIiLEAST64 "%lli"
#define PRIdFAST64 "%lld"
#define PRIiFAST64 "%lli"

#define PRIdMAX "%lld"
#define PRIiMAX "%lli"
#else
#error Cannot define print specifiers for int64. Consider compiling with a C99 compliant compiler
#endif
#endif

#define PRIdPTR "%p"
#define PRIiPTR "%p"
#endif

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

typedef intptr ptrdiff;

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
#if (defined(_MSC_VER) && _MSC_VER < 1800) ||                                  \
    (!defined(_MSC_VER) && !defined(__STDC_VERSION__))
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
#define NULL ((void *)0)
#endif

/* OSes Macros */
#if defined(_WIN32) || defined(_WIN64)
#if !defined(OS_WINDOWS)
#define OS_WINDOWS 1
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#if !defined(OS_MACOS)
#define OS_MACOS 1
#endif
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#error MacOS header files are written in C99. Consider compiling with a C99 compliant compiler
#endif
#elif defined(__unix__)
#if !defined(OS_UNIX)
#define OS_UNIX 1
#endif
#if defined(__linux__)
#if !defined(OS_LINUX)
#define OS_LINUX 1
#endif
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#if !defined(OS_FREEBSD)
#define OS_FREEBSD 1
#endif
#elif defined(__OpenBSD__)
#if !defined(OS_OPENBSD)
#define OS_OPENBSD 1
#endif
#elif defined(__NetBSD__)
#if !defined(OS_NETBSD)
#define OS_NETBSD 1
#endif
#else
#error This Unix Operating System is not supported
#endif
#define _POSIX_C_SOURCE 200809L
typedef intptr intptr_t;
#include <unistd.h>
#if defined(_POSIX_VERSION) || defined(_POSIX2_VERSION)
#if !defined(OS_POSIX)
#define OS_POSIX 1
#endif
#endif
#else
#error This Operating System is not supported
#endif

/* Compiler Macros */
#if defined(_MSC_VER)
#if !defined(VISUAL_STUDIO_ENV)
#define VISUAL_STUDIO_ENV 1
#endif
#if !defined(__clang__)
#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 1
#endif
#endif
#elif defined(__GNUC__) && (!defined(__clang__) || !defined(__INTEL_COMPILER))
#if !defined(COMPILER_GCC)
#define COMPILER_GCC
#endif
#elif defined(__clang__)
#if !defined(COMPILER_CLANG)
#define COMPILER_CLANG 1
#endif
#elif defined(__INTEL_COMPILER)
#if !defined(COMPILER_INTEL)
#define COMPILER_INTEL 1
#endif
#else
#error Unknown Compiler
#endif

#if defined(__GNUC__)
#if !defined(GNUC_FEATURES)
#define GNUC_FEATURES 1
#endif
#endif

/* Inline */
#if !defined(INLINE)
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) ||              \
    (defined(__cplusplus) && __cplusplus >= 199711L)
#define INLINE inline
#elif defined(COMPILER_GCC)
#define INLINE __inline__
#elif defined(_MSC_VER)
#if _MSC_VER > 1800
#define INLINE inline
#elif _MSC_VER >= 1200
#define INLINE __inline
#endif
#else
#define INLINE
#endif
#endif

/* Always Inline */
#if !defined(alwaysinline)
#if defined(_MSC_VER) && _MSC_VER >= 1300
#define alwaysinline __forceinline
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) ||                      \
    defined(COMPILER_INTEL)
#define alwaysinline __attribute__((__always_inline__)) INLINE
#else
#define alwaysinline
#endif
#endif

/* No-inline */
#if !defined(noinline)
#if defined(COMPILER_MSVC)
#define noinline __declspec(noinline)
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) ||                      \
    defined(COMPILER_INTEL)
#define noinline __attribute__((noinline))
#else
#define noinline
#endif
#endif

/* Restrict */
#if !defined(RESTRICT)
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) &&              \
    !defined(__cplusplus)
#define RESTRICT restrict
#elif (defined(_MSC_VER) && _MSC_VER >= 1400) || defined(COMPILER_GCC) ||      \
    defined(COMPILER_CLANG)
#define RESTRICT __restrict
#else
#define RESTRICT
#endif
#endif

/* Register */
#if !defined(REGISTER)
#if !defined(__cplusplus) || (defined(__cplusplus) && __cplusplus <= 201402L)
#define REGISTER register
#else
#define REGISTER
#endif
#endif

/* No_Return */
#if !defined(NORETURN)
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define noreturn _Noreturn
#elif defined(GNUC_FEATURES)
#define noreturn __attribute__((__noreturn__))
#elif defined(COMPILER_MSVC)
#define noreturn __declspec(noreturn)
#else
#error Unknown Compiler. Could not define NORETURN macro
#endif
#endif

/* Thread Local */
#if !defined(THREAD_LOCAL) && !defined(NO_THREAD_LOCAL)
#if defined(_MSC_VER) && _MSC_VER >= 1300
#define THREAD_LOCAL __declspec(thread)
#elif defined(GNUC_FEATURES)
#define THREAD_LOCAL __thread
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define THREAD_LOCAL _Thread_local
#else
#error Could not define thread local macro. Consider compiling with a compiler which supports thread local feature or defining NO_THREAD_LOCAL macro
#endif
#endif

#if !defined(PRINTF_ARGS)
#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#define PRINTF_ARGS(FMT) __attribute__((format(printf, FMT, (FMT + 1))))
#else
#define PRINTF_ARGS(FMT)
#endif
#endif

#if !defined(internal)
#define internal static
#endif

#if !defined(persistant)
#define persistant static
#endif

#if !defined(global)
#define global static
#endif

#if !defined(RANDOM_NUMBER)
#if defined(COMPILER_GCC) || defined(COMPILER_CLANG) || defined(COMPILER_MSVC)
#define RANDOM_NUMBER __COUNTER__
#else
#define RANDOM_NUMBER __LINE__
#endif
#endif

#if !defined(STATIC_ASSERT)
#define STATIC_ASSERT3(cond, name)                                             \
  struct static_assert##RANDOM_NUMBER {                                        \
    char name[!!(cond)*2 - 1]                                                  \
  }
#define STATIC_ASSERT2(cond, __line__)                                         \
  STATIC_ASSERT3(cond, static_assertion_at_line##__line__)
#define STATIC_ASSERT1(cond, __line__) STATIC_ASSERT2((cond), __line__)
#define STATIC_ASSERT(cond) STATIC_ASSERT1(cond, __LINE__)
#endif

/* Useful macros */
#if !defined(offsetof)
#define offsetof(type, field) (&(((type *)0)->field))
#endif

#if !defined(alignof)
#define alignof(type)                                                          \
  offsetof(                                                                    \
      struct {                                                                 \
        char c;                                                                \
        type field;                                                            \
      },                                                                       \
      field)
#endif

#if !defined(unused)
#define unused(var) ((void)var)
#endif

#if !defined(swap)
#define swap(type, a, b)                                                       \
  do {                                                                         \
    type tmp = (a);                                                            \
    a = b;                                                                     \
    b = tmp                                                                    \
  } while (0)
#endif

#if !defined(bit_flag)
#define bit_flag(x) (1 << (x))
#define set_flag(flag, bit) ((flag) |= (bit))
#define toggle_flag(flag, bit) ((flag) ^= (bit))
#define unset_flag(flag, bit) ((flag) &= ~(bit))
#define has_flag(flag, bit) ((flag) & (bit))
#endif

#undef min
#undef max

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#if !defined(min3)
#define min3(a, b, c) (min(min((a), (b)), (c)))
#endif

#if !defined(max3)
#define max3(a, b, c) (max(max((a), (b)), (c)))
#endif

#if !defined(clamp)
#define clamp(x, lower_bound, higher_bound)                                    \
  min(max((x), (lower_bound)), (higher_bound))
#endif

#if !defined(likely) && !defined(unlikely)
#if defined(GNUC_FEATURES)
#define likely(expr) __builtin_expect(!!(expr), 1)
#define unlikely(expr) __builtin_expect(!!(expr), 0)
#else
#define likely(expr) (!!(expr))
#define unlikely(expr) (!!(expr))
#endif
#endif

/* Memory */
#if !defined(ispow2)
#define ispow2(x) ((x) <= 0 ? false : !(x & (x - 1)))
#endif

#if !defined(BYTES_MACROS)
#define BYTES_MACROS
#define s_kilobytes(x) ((x) * (1024))
#define s_megabytes(x) (s_kilobytes((x)) * (1024))
#define s_gigabytes(x) (s_megabytes((x)) * (1024))
#define s_terabytes(X) (s_gigabytes((x)) * (1024))
#endif

HEADER_DEF void memswap(void *ptr1, void *ptr2, usize size);

/* Debug */
#if !defined(debug_trap)
#define debug_trap() abort()
#endif

#if !defined(die)
#define die(msg)                                                               \
  do {                                                                         \
    fprintf(stderr, "%s:%d: Died: %s", __FILE__, __LINE__, msg);               \
    abort();                                                                   \
  } while (0)
#endif

/* Allocators */
typedef void *(*malloc_func_t)(void *, usize);
typedef void *(*realloc_func_t)(void *, void *, usize);
typedef void (*free_func_t)(void *, void *);

typedef struct allocator_t {
  malloc_func_t alloc;
  realloc_func_t realloc;
  free_func_t free;
  void *data;
} allocator_t;

#if !defined(allocators_eq)
#define allocators_eq(a, b)                                                    \
  (a.alloc == b.alloc && a.realloc == b.realloc && a.free == b.free &&         \
   a.data == b.data)
#endif

HEADER_DEF void *halloc(allocator_t a, usize size);
HEADER_DEF void *hcalloc(allocator_t a, usize num, usize size);
HEADER_DEF void *hrealloc(allocator_t a, void *ptr, usize size);
HEADER_DEF void hdealloc(allocator_t a, void *ptr);

HEADER_DEF void *system_alloc(usize size);
HEADER_DEF void system_dealloc(void *ptr, usize size);
HEADER_DEF void *system_realloc(void *ptr, usize prev_size, usize new_size);

HEADER_DEF allocator_t std_allocator;

typedef void *(*xmalloc_handler_t)(void);
HEADER_DEF allocator_t xallocator_make(allocator_t a,
                                       xmalloc_handler_t xmalloc_handler);
HEADER_DEF void *default_xmalloc_handler(void);

HEADER_DEF allocator_t arena_make(allocator_t a, usize arena_size);
HEADER_DEF void arena_dealloc(allocator_t *a);

/* Variable length arrays */
typedef struct ArrayHeader {
  usize length;
  usize capacity;
  allocator_t allocator;
} ArrayHeader;

HEADER_DEF void *__array_set_capacity(void *array, usize capacity,
                                      usize element_size);

#if !defined(Array)
#define Array(T) T *
#endif

#if !defined(GROW_FORMULA)
#define GROW_FORMULA(x) (2 * (x) + 8)
#endif

#if !defined(ARRAY_HEADER)
#define ARRAY_HEADER(x) ((ArrayHeader *)(x)-1)
#endif

#if !defined(array_length)
#define array_length(x) ARRAY_HEADER(x)->length
#endif

#if !defined(array_capacity)
#define array_capacity(x) ARRAY_HEADER(x)->capacity
#endif

#if !defined(array_init_helper)
#define array_init_helper(_allocator, x, _cap)                                 \
  do {                                                                         \
    void **_array = (void **)&(x);                                             \
    ArrayHeader *_header = (ArrayHeader *)halloc(                              \
        _allocator, sizeof(ArrayHeader) + sizeof(*(x)) * (_cap));              \
    _header->length = 0;                                                       \
    _header->capacity = _cap;                                                  \
    _header->allocator = _allocator;                                           \
    *_array = (void *)(_header + 1);                                           \
  } while (0)
#endif

#if !defined(array_init)
#define array_init(_allocator, x) array_init_helper(_allocator, x, 8)
#endif

#if !defined(array_dealloc)
#define array_dealloc(x)                                                       \
  do {                                                                         \
    ArrayHeader *_header = ARRAY_HEADER(x);                                    \
    hdealloc(_header->allocator, _header);                                     \
  } while (0)
#endif

#if !defined(array_set_capacity)
#define array_set_capacity(x, capacity)                                        \
  do {                                                                         \
    if (x) {                                                                   \
      void **_array = (void **)&(x);                                           \
      *_array = __array_set_capacity((x), (capacity), sizeof(*(x)));           \
    }                                                                          \
  } while (0)
#endif

#if !defined(array_grow)
#define array_grow(x, cap)                                                     \
  do {                                                                         \
    isize _cap = GROW_FORMULA(array_capacity(x));                              \
    if (_cap < (isize)cap)                                                     \
      _cap = cap;                                                              \
    array_set_capacity(x, _cap);                                               \
  } while (0)
#endif

#if !defined(array_resize)
#define array_resize(x, new_length)                                            \
  do {                                                                         \
    if (array_capacity(x) < (new_length))                                      \
      array_grow(x, (new_length));                                             \
    ARRAY_HEADER(x)->length = (new_length);                                    \
  } while (0)
#endif

#if !defined(array_reserve)
#define array_reserve(x, new_cap)                                              \
  do {                                                                         \
    if (array_capacity(x) < (new_cap))                                         \
      array_set_capacity(x, (new_cap));                                        \
  } while (0)
#endif

#if !defined(array_push)
#define array_push(x, item)                                                    \
  do {                                                                         \
    if (array_capacity(x) < array_length(x) + 1)                               \
      array_grow(x, 0);                                                        \
    x[array_length(x)++] = (item);                                             \
  } while (0)
#endif

#if !defined(array_concat)
#define array_concat(x, items)                                                 \
  do {                                                                         \
    if (sizeof(*(x)) == sizeof(*(items)) &&                                    \
        allocators_eq(ARRAY_HEADER(x)->allocator,                              \
                      ARRAY_HEADER(x)->allocator)) {                           \
      usize x_length = array_length(x);                                        \
      if (array_capacity(x) < (array_length(x) + array_length(items)))         \
        array_grow(x, (array_length(x) + array_length(items)));                \
      memcpy(x + x_length, (items), sizeof(*(items)) * array_length(items));   \
      array_length(x) += array_length(items);                                  \
    } else                                                                     \
      fprintf(stderr,                                                          \
              "[arrays error] Concatenating arrays of different types");       \
  } while (0)
#endif

#if !defined(array_pop)
#define array_pop(x)                                                           \
  do {                                                                         \
    if (array_length(x) > 0)                                                   \
      ARRAY_HEADER(x)->length -= 1;                                            \
    else                                                                       \
      fprintf(stderr, "[arrays error] Popping from a zero sized array");       \
  } while (0)
#endif

#if !defined(array_clear)
#define array_clear(x)                                                         \
  do {                                                                         \
    ARRAY_HEADER(x)->length = 0;                                               \
  } while (0)
#endif

#if !defined(array_end)
#define array_end(x) (x + array_length(x))
#endif

/* File */
typedef struct file_t {
  char *content;
  usize length;
  allocator_t allocator;
} file_t;

file_t file_read(allocator_t a, char *path);
void file_dealloc(file_t *file);

#if !defined(file_for)
#define file_for(_index, _value, _file, _body)                                 \
  do {                                                                         \
    if (_file.content != NULL && _file.length > 0) {                           \
      usize _index = 0;                                                        \
      char _value = _file.content[_index];                                     \
      for (; _index < _file.length; _value = _file.content[++_index]) {        \
        _body                                                                  \
      }                                                                        \
    }                                                                          \
  } while (0)
#endif

/* Flags */
#if !defined(FLAG_INT_TYPE)
#define FLAG_INT_TYPE i64
#endif

typedef int (*handler_t)(int, char **);

typedef struct SubMode {
  const char *name;
  const char *description;
  handler_t handler;
} SubMode;

typedef enum FlagType { FLAG_BOOL, FLAG_INT, FLAG_STRING } FlagType;

typedef union FlagValue {
  bool bool_value;
  FLAG_INT_TYPE int_value;
  char *string_value;
} FlagValue;

typedef struct Flag {
  char *name;
  char *description;
  FlagType type;
  FlagValue value;
  FlagValue default_value;
} Flag;

typedef enum FlagErrorKind {
  FLAG_ERROR_NONE,
  FLAG_ERROR_NO_VALUE,
  FLAG_ERROR_INVALID_VALUE,
  FLAG_ERROR_COUNT,
  FLAG_ERROR_INVALID_FLAG,
  FLAG_ERROR_SUBMODE_COUNT,
  FLAG_ERROR_INVALID_SUBMODE
} FlagErrorKind;

typedef struct FlagError {
  FlagErrorKind kind;
  char *flag;
} FlagError;

#if !defined(flag_has_error)
#define flag_has_error(error) (error.kind != FLAG_ERROR_NONE)
#endif

#if !defined(SUBMODE_NUM)
#define SUBMODE_NUM 12
#endif

#if !defined(FLAG_NUM)
#define FLAG_NUM 128
#endif

typedef struct FlagParser {
  SubMode submodes[SUBMODE_NUM];
  Flag flags[FLAG_NUM];

  usize flags_count;
  usize submodes_count;

  char *program_name;

  i32 rest_argc;
  char **rest_argv;

  FlagError error;
} FlagParser;

HEADER_DEF void flag_submode(const char *name, const char *description,
                             handler_t handler);
HEADER_DEF int _flag_parse_submode(int *argc, char ***argv);

#if !defined(flag_parse_submode)
#define flag_parse_submode(_argc, _argv) _flag_parse_submode(&_argc, &_argv)
#endif

HEADER_DEF bool *flag_bool(const char *name, const char *description,
                           bool default_value);
HEADER_DEF FLAG_INT_TYPE *flag_int(const char *name, const char *description,
                                   FLAG_INT_TYPE default_value);
HEADER_DEF char **flag_string(const char *name, const char *description,
                              const char *default_value);
HEADER_DEF FlagError flag_parse(int argc, char **argv);

HEADER_DEF const char *flag_error_to_string(FlagError error);
HEADER_DEF void flag_print_error(FlagError error);

HEADER_DEF const char *flag_get_program_name(void);
HEADER_DEF bool flag_has_rest_args(void);
HEADER_DEF int flag_get_rest_argc(void);
HEADER_DEF char **flag_get_rest_argv(void);

/* String */
#if !defined(CHAR_BIT)
#define CHAR_BIT (sizeof(char) * 8)
#endif

#if !defined(SSO_CAPACITY)
#define SSO_CAPACITY (sizeof(struct NonSSO_String) / sizeof(char) - 1)
#endif

typedef struct NonSSO_String {
  u8 *str;
  usize size;
  usize capacity;
} NonSSO_String;

typedef struct SSO_String {
  u8 str[sizeof(struct NonSSO_String) - 1];
  u8 size;
} SSO_String;

typedef union string_t {
  NonSSO_String non_sso;
  SSO_String sso;
} string_t;

HEADER_DEF string_t string_make(allocator_t a, char *string, ...);
HEADER_DEF void string_dealloc(allocator_t a, string_t *str);
HEADER_DEF u8 *string_to_cstr(string_t *str);
HEADER_DEF usize string_length(string_t *str);
HEADER_DEF usize string_capacity(string_t *str);

#if !defined(String_from_cstr)
#define String_from_cstr(_allocator, cstr)                                     \
  string_alloc(_allocator, (u8 *)(cstr))
#endif

/* Hashmaps */
typedef u64 (*hash_func_t)(u64);
typedef u64 (*hash_bytes_func_t)(void *, usize);

typedef struct hasher {
  hash_func_t hash;
  hash_bytes_func_t hash_bytes;
} hasher_t;

typedef struct hashmap_t {
  u64 *keys;
  u64 *vals;
  usize length;
  usize capacity;
  hasher_t hasher;
  allocator_t allocator;
} hashmap_t;

HEADER_DEF hashmap_t hashmap_make(allocator_t a, hasher_t hasher);
HEADER_DEF void hashmap_put(hashmap_t *hashmap, void *key, void *elem);
HEADER_DEF void hashmap_put_from_u64(hashmap_t *hashmap, u64 key, void *val);
HEADER_DEF void hashmap_put_from_str_length(hashmap_t *hashmap, char *key,
                                            usize length, void *val);
HEADER_DEF void hashmap_put_from_str(hashmap_t *hashmap, char *key, void *val);
HEADER_DEF void *hashmap_get(hashmap_t *hashmap, void *key);
HEADER_DEF void *hashmap_get_from_u64(hashmap_t *hashmap, u64 key);
HEADER_DEF void *hashmap_get_from_str_length(hashmap_t *hashmap, char *key,
                                             usize length);
HEADER_DEF void *hashmap_get_from_str(hashmap_t *hashmap, char *key);

HEADER_DEF hasher_t default_hasher;
HEADER_DEF hasher_t ahash_hasher;

#if defined(__cplusplus)
}
#endif

#endif
#if defined(HEADER_IMPLEMENTATION)
#undef HEADER_IMPLEMENTATION
#if defined(__cplusplus)
extern "C" {
#endif
/* Memory */
alwaysinline void memswap(void *ptr1, void *ptr2, usize size) {
  REGISTER u8 *_ptr1 = (u8 *)ptr1;
  REGISTER u8 *_ptr2 = (u8 *)ptr2;
  REGISTER u8 value;
  if (_ptr1 != _ptr2) {
    while (size--) {
      value = *_ptr1;
      *_ptr1++ = *_ptr2;
      *_ptr2++ = value;
    }
  }
}
/* Allocators */
alwaysinline void *halloc(allocator_t a, usize size) {
  return a.alloc(a.data, size);
}

alwaysinline void *hcalloc(allocator_t a, usize num, usize size) {
  void *ptr = halloc(a, num * size);
  memset(ptr, 0, num * size);
  return ptr;
}

alwaysinline void *hrealloc(allocator_t a, void *ptr, usize size) {
  return a.realloc(a.data, ptr, size);
}

alwaysinline void hdealloc(allocator_t a, void *ptr) { a.free(a.data, ptr); }

#if defined(OS_WINDOWS)
alwaysinline void *system_alloc(usize size) {
  return VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

alwaysinline void system_dealloc(void *ptr, usize size) {
  unused(size);
  VirtualFree(ptr, 0, MEM_RELEASE);
}
#else
alwaysinline void *system_alloc(usize size) {
  return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
              -1, 0);
}

alwaysinline void system_dealloc(void *ptr, usize size) {
  if (munmap(ptr, size) < 0) {
    fprintf(stderr, "System allocator failed! Aborting...\n");
    abort();
  }
}
#endif
void *std_alloc(void *data, usize size) {
  unused(data);
  return malloc(size);
}

void *std_realloc(void *data, void *ptr, usize size) {
  unused(data);
  return realloc(ptr, size);
}

void std_dealloc(void *data, void *ptr) {
  unused(data);
  free(ptr);
}

allocator_t std_allocator = {std_alloc, std_realloc, std_dealloc, NULL};

typedef struct xmalloc_info_t {
  allocator_t allocator;
  xmalloc_handler_t xmalloc_handler;
} xmalloc_info_t;

void *xmalloc_alloc(void *_data, usize size) {
  xmalloc_info_t *data = (xmalloc_info_t *)_data;
  void *ptr = halloc(data->allocator, size);
  if (ptr == NULL) {
    return data->xmalloc_handler();
  }
  return ptr;
}

void *xmalloc_realloc(void *_data, void *_ptr, usize size) {
  xmalloc_info_t *data = (xmalloc_info_t *)_data;
  void *ptr = hrealloc(data->allocator, _ptr, size);
  if (ptr == NULL) {
    return data->xmalloc_handler();
  }
  return ptr;
}

void xmalloc_dealloc(void *_data, void *ptr) {
  xmalloc_info_t *data = (xmalloc_info_t *)_data;
  hdealloc(data->allocator, ptr);
}

allocator_t xallocator_make(allocator_t a, xmalloc_handler_t xmalloc_handler) {
  allocator_t result;
  xmalloc_info_t *data = (xmalloc_info_t *)halloc(a, sizeof(xmalloc_info_t));
  data->allocator = a;
  data->xmalloc_handler = xmalloc_handler;
  result.data = (void *)data;
  result.alloc = xmalloc_alloc;
  result.realloc = xmalloc_realloc;
  result.free = xmalloc_dealloc;
  return result;
}

void *default_xmalloc_handler(void) {
  fprintf(stderr, "[Allocation error] Couldn't allocate memory! Aborting...\n");
  abort();
  return NULL;
}

#if !defined(DEFAULT_ALIGNMENT)
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

/* Arena Allocators */
#if !defined(ARENA_SIZE)
#define ARENA_SIZE 1024 * 1024
#endif

#define ALIGN_DOWN(n, a) ((n) & ~((a)-1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n) + (a)-1, (a))
#define ALIGN_DOWN_PTR(p, a) ((void *)ALIGN_DOWN((uintptr)(p), (a)))
#define ALIGN_UP_PTR(p, a) ((void *)ALIGN_UP((uintptr)(p), (a)))

typedef struct arena_block_t {
  u8 *ptr;
  usize size;
} arena_block_t;

typedef struct arena_t {
  u8 *ptr;
  u8 *end;
  Array(arena_block_t) blocks;
} arena_t;

static void arena_grow(arena_t *arena, usize min_size) {
  usize size = ALIGN_UP(max(min_size, ARENA_SIZE), DEFAULT_ALIGNMENT);
  arena_block_t block;
  arena->ptr = (u8 *)system_alloc(size);
  arena->end = arena->ptr + size;
  block.ptr = arena->ptr;
  block.size = size;
  array_push(arena->blocks, block);
}

void *arena_alloc(void *data, usize size) {
  arena_t *arena = (arena_t *)data;
  void *ptr;
  if (size > (usize)(arena->end - arena->ptr)) {
    arena_grow(arena, size);
  }
  ptr = arena->ptr;
  arena->ptr = (u8 *)ALIGN_UP_PTR(arena->ptr + size, DEFAULT_ALIGNMENT);
  return ptr;
}

void *arena_realloc(void *data, void *ptr, usize size) {
  unused(data);
  unused(ptr);
  unused(size);
  fprintf(stderr, "Cannot call s_realloc on arena allocator! Aborting...\n");
  abort();
  return NULL;
}

void arena_dealloc(allocator_t *a) {
  arena_t *arena = (arena_t *)a->data;
  arena_block_t *it;
  for (it = arena->blocks; it != array_end(arena->blocks); ++it) {
    system_dealloc(it->ptr, it->size);
  }
  array_dealloc(arena->blocks);
}

void arena_error_dealloc(void *data, void *ptr) {
  unused(data);
  unused(ptr);
}

allocator_t arena_make(allocator_t a, usize size) {
  allocator_t result;
  arena_t *arena = (arena_t *)halloc(a, sizeof(arena_t));
  arena->end = NULL;
  arena->ptr = NULL;
  array_init(a, arena->blocks);
  arena_grow(arena, size);
  result.data = (void *)arena;
  result.alloc = arena_alloc;
  result.realloc = arena_realloc;
  result.free = arena_error_dealloc;
  return result;
}

/* Variable Length Arrays */
void *__array_set_capacity(void *array, usize capacity, usize element_size) {
  ArrayHeader *header = ARRAY_HEADER(array);
  allocator_t a = header->allocator;
  usize new_capacity = capacity;
  usize size;
  ArrayHeader *new_header;

  if (capacity == header->capacity)
    return array;

  if (capacity < header->length) {
    if (header->capacity < capacity) {
      new_capacity = GROW_FORMULA(header->capacity);
      if (new_capacity < capacity)
        new_capacity = capacity;
    }
    header->capacity = new_capacity;
  }
  size = sizeof(ArrayHeader) + element_size * capacity;
  new_header = (ArrayHeader *)halloc(a, size);
  memmove(new_header, header,
          sizeof(ArrayHeader) + element_size * header->length);
  new_header->length = header->length;
  new_header->capacity = capacity;
  new_header->allocator = a;
  hdealloc(a, header);
  return (void *)(new_header + 1);
}

/* File */
file_t file_read(allocator_t a, char *path) {
  file_t result;
  FILE *file;
  usize length;
  char *content;

  file = fopen(path, "rb");
  if (!file) {
    result.content = NULL;
    result.length = 0;
    return result;
  }
  fseek(file, 0, SEEK_END);
  length = ftell(file);
  fseek(file, 0, SEEK_SET);
  content = (char *)halloc(a, length + 1);
  if (length && fread(content, length, 1, file) != 1) {
    fclose(file);
    hdealloc(a, content);
    result.content = NULL;
    result.length = 0;
    return result;
  }
  fclose(file);
  content[length] = 0;
  result.content = content;
  result.length = length;
  result.allocator = a;
  return result;
}

void file_dealloc(file_t *file) {
  hdealloc(file->allocator, file->content);
  file->length = 0;
  file->content = NULL;
}

/* Flag Parsing */

#if !defined(SET_FLAG_PARSER_ERROR)
#define SET_FLAG_PARSER_ERROR(_parser, _kind, _flag)                           \
  do {                                                                         \
    _parser->error.kind = _kind;                                               \
    _parser->error.flag = _flag;                                               \
  } while (0)
#endif

static FlagParser flag_parser;

void flag_submode(const char *name, const char *description,
                  handler_t handler) {
  FlagParser *parser = &flag_parser;

  if (parser->submodes_count < SUBMODE_NUM) {
    SubMode *submode = &parser->submodes[parser->submodes_count++];
    submode->name = name;
    submode->description = description;
    submode->handler = handler;
  } else {
    SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_SUBMODE_COUNT, (char *)name);
  }
}

int _flag_parse_submode(int *argc, char ***argv) {
  FlagParser *parser = &flag_parser;
  usize iter = 0;
  char *submode;

  if (parser->program_name == NULL) {
    parser->program_name = *argv[0];
    *argc -= 1;
    *argv += 1;
  }

  submode = *argv[0];

  SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_NONE, submode);

  for (iter = 0; iter < parser->submodes_count; ++iter) {
    if (strcmp(submode, parser->submodes[iter].name) == 0) {
      *argc -= 1;
      *argv += 1;
      return parser->submodes[iter].handler(*argc, *argv);
    }
  }

  fprintf(stderr, "Unknown submode %s\n", submode);
  fprintf(stderr, "Available submodes are:\n");

  for (iter = 0; iter < parser->submodes_count; ++iter) {
    fprintf(stderr, "\t- %s\n", parser->submodes[iter].name);
  }

  return 1;
}

static Flag *flag_new(FlagType type, const char *name,
                      const char *description) {
  FlagParser *parser = &flag_parser;

  if (parser->flags_count < FLAG_NUM) {
    Flag *flag = &parser->flags[parser->flags_count++];
    flag->type = type;
    flag->name = (char *)name;
    flag->description = (char *)description;
    return flag;
  } else {
    SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_COUNT, (char *)name);
    return NULL;
  }
}

bool *flag_bool(const char *name, const char *description, bool default_value) {
  Flag *flag = flag_new(FLAG_BOOL, name, description);
  flag->default_value.bool_value = default_value;
  flag->value.bool_value = default_value;
  return &flag->value.bool_value;
}

FLAG_INT_TYPE *flag_int(const char *name, const char *description,
                        FLAG_INT_TYPE default_value) {
  Flag *flag = flag_new(FLAG_INT, name, description);
  flag->default_value.int_value = default_value;
  flag->value.int_value = default_value;
  return &flag->value.int_value;
}

char **flag_string(const char *name, const char *description,
                   const char *default_value) {
  Flag *flag = flag_new(FLAG_STRING, name, description);
  flag->default_value.string_value = (char *)default_value;
  flag->value.string_value = (char *)default_value;
  return &flag->value.string_value;
}

static bool flag_str_to_int(char *str, FLAG_INT_TYPE *result) {
  FlagParser *parser = &flag_parser;
  char *start = str;
  bool is_negative = false;
  FLAG_INT_TYPE sign = 1;
  persistant const FLAG_INT_TYPE char_to_digit[256] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  if (str != NULL) {
    is_negative = *str == '-';
    sign = is_negative * -1;
    str += is_negative;
    if (isdigit(*str)) {
      while (isdigit(*str)) {
        *result *= 10;
        *result += char_to_digit[(u8)*str];
        str++;
      }
      if (strcmp(str, "G") == 0) {
        *result = s_gigabytes(*result);
      } else if (strcmp(str, "M") == 0) {
        *result = s_megabytes(*result);
      } else if (strcmp(str, "K") == 0) {
        *result = s_kilobytes(*result);
      } else if (strcmp(str, "") != 0) {
        SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_INVALID_VALUE, start);
        return false;
      }
      *result *= sign;
      return true;
    } else {
      SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_INVALID_VALUE, (char *)start);
    }
  } else {
    SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_NO_VALUE, start);
  }
  return false;
}

#if !defined(next_flag)
#define next_flag(_argc, _argv, _iter) _next_flag(_argc, _argv, &_iter)
#endif

static char *_next_flag(int argc, char **argv, usize *iter) {
  *iter += 1;
  if (*iter < (usize)argc)
    return argv[*iter];

  return NULL;
}

noinline FlagError flag_parse(int argc, char **argv) {
  FlagParser *parser = &flag_parser;
  usize iter = 0;

  if (parser->program_name == NULL) {
    parser->program_name = argv[0];
    iter = 1;
  }

  for (; iter < (usize)argc; ++iter) {
    char *flag = argv[iter];
    if (*flag == '-') {
      usize i = 0;
      bool found = false;
      flag++;
      if (*flag == '-')
        flag++;
      for (i = 0; i < parser->flags_count; ++i) {
        if (strcmp(flag, parser->flags[i].name) == 0 && !found) {
          switch (parser->flags[i].type) {
          case FLAG_BOOL: {
            parser->flags[i].value.bool_value = true;
            found = true;
          } break;
          case FLAG_INT: {
            FLAG_INT_TYPE int_value = 0;
            flag = next_flag(argc, argv, iter);
            if (flag_str_to_int(flag, &int_value)) {
              parser->flags[i].value.int_value = int_value;
            } else {
              SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_INVALID_VALUE, flag);
              parser->rest_argc = argc - iter;
              parser->rest_argv = argv + iter;
              return parser->error;
            }
            found = true;
          } break;
          case FLAG_STRING: {
            char *string_value = next_flag(argc, argv, iter);
            if (string_value != NULL) {
              parser->flags[i].value.string_value = string_value;
            } else {
              parser->rest_argc = argc - iter;
              parser->rest_argv = argv + iter;

              SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_INVALID_VALUE, flag);
              return parser->error;
            }
            found = true;
          } break;
          default: {
            fprintf(stderr, "Unreachable");
          } break;
          }
        }
      }
      if (!found) {
        SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_INVALID_FLAG, flag);
        parser->rest_argc = argc - iter;
        parser->rest_argv = argv + iter;
        return parser->error;
      }
    } else {
      SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_INVALID_FLAG, flag);
      parser->rest_argc = argc - iter;
      parser->rest_argv = argv + iter;
      return parser->error;
    }
  }

  parser->rest_argc = argc - iter;
  parser->rest_argv = argv + iter;
  return parser->error;
}

static Flag *get_flag_by_name(const char *name) {
  FlagParser *parser = &flag_parser;
  usize iter = 0;

  for (; iter < parser->flags_count; ++iter) {
    if (strcmp(name, parser->flags[iter].name) == 0) {
      return &parser->flags[iter];
    }
  }

  return NULL;
}

const char *flag_error_to_string(FlagError error) {
  static const char *error_string[] = {
      "No Error",     "No value passed",     "Invalid value",  "Count error",
      "Invalid flag", "Submode count error", "Invalid submode"};

  if (error.kind >= FLAG_ERROR_NONE && error.kind <= FLAG_ERROR_INVALID_FLAG) {
    return error_string[error.kind];
  }

  return "Unreachable";
}

void flag_print_error(FlagError error) {
  if (error.kind != FLAG_ERROR_NONE) {
    Flag *flag = get_flag_by_name(error.flag);
    fprintf(stderr, "[Error] %s on flag '%s': ", flag_error_to_string(error),
            flag->name);
    switch (flag->type) {
    case FLAG_BOOL: {
      fprintf(stderr, "%s\n",
              flag->default_value.bool_value ? "true" : "false");
    } break;
    case FLAG_INT: {
      fprintf(stderr, PRId64 "\n", flag->value.int_value);
    } break;
    case FLAG_STRING: {
      fprintf(stderr, "%s\n", flag->value.string_value);
    } break;
    default: {
      fprintf(stderr, "Unreahcable!");
    } break;
    }
  }
}

void print_usage(FILE *stream) {
  FlagParser *parser = &flag_parser;
  usize iter = 0;
  for (iter = 0; iter < parser->flags_count; ++iter) {
    Flag flag = parser->flags[iter];

    fprintf(stream, "    -%s\n", flag.name);
    fprintf(stream, "        %s\n", flag.description);
    switch (flag.type) {
    case FLAG_BOOL: {
      fprintf(stream, "        Default: %s\n",
              flag.default_value.bool_value ? "true" : "false");
    } break;
    case FLAG_STRING: {
      fprintf(stream, "        Default: %s\n", flag.default_value.string_value);
    } break;
    case FLAG_INT: {
      fprintf(stream, "        Default: " PRId64 "\n",
              flag.default_value.int_value);
    } break;
    default: {
      fprintf(stderr, "Unreachable");
    } break;
    }
  }
}

const char *flag_get_program_name(void) {
  FlagParser *parser = &flag_parser;

  return (const char *)parser->program_name;
}

bool flag_has_rest_args(void) {
  FlagParser *parser = &flag_parser;
  return parser->rest_argc > 0 && parser->error.kind == FLAG_ERROR_INVALID_FLAG;
}

int flag_get_rest_argc(void) {
  FlagParser *parser = &flag_parser;
  return parser->rest_argc;
}

char **flag_get_rest_argv(void) {
  FlagParser *parser = &flag_parser;
  return parser->rest_argv;
}

/* Private API */
typedef struct NonSSO_Data {
  usize capacity;
  usize size;
} NonSSO_Data;

static u8 *most_sig_byte(void *obj, usize size) {
  u8 *ptr = (u8 *)obj;
  return ptr + size - 1;
}

static bool lsb(u8 byte, usize n) { return byte & (1u << n); }

static bool msb(u8 byte, usize n) { return byte & (1u << (CHAR_BIT - n - 1)); }

static void set_msb(u8 *byte, bool bit, usize n) {
  if (bit) {
    (*byte) |= 1u << (CHAR_BIT - n - 1);
  } else {
    (*byte) &= ~(1u << (CHAR_BIT - n - 1));
  }
}

static void set_lsb(u8 *byte, bool bit, usize n) {
  if (bit) {
    (*byte) |= 1u << n;
  } else {
    (*byte) &= ~(1u << n);
  }
}

static void String_set_sso_size(string_t *str, usize size) {
  str->sso.size = (u8)(SSO_CAPACITY - size) << 2;
}

static void String_set_non_sso_data(string_t *str, usize size, usize capacity) {
  u8 *size_hsb = most_sig_byte(&size, sizeof(size));
  const bool size_high_bit = msb(*size_hsb, 0);

  u8 *cap_hsb = most_sig_byte(&capacity, sizeof(capacity));
  const bool cap_high_bit = msb(*size_hsb, 0);
  const bool cap_sec_high_bit = msb(*size_hsb, 1);

  set_msb(size_hsb, cap_sec_high_bit, 0);

  (*cap_hsb) <<= 2;
  set_lsb(cap_hsb, cap_high_bit, 0);
  set_lsb(cap_hsb, !size_high_bit, 1);

  str->non_sso.size = size;
  str->non_sso.capacity = capacity;
}

static bool String_is_sso(string_t *str) {
  return !lsb(str->sso.size, 0) && !lsb(str->sso.size, 1);
}

static usize String_get_sso_size(string_t *str) {
  return SSO_CAPACITY - ((str->sso.size >> 2) & 63u);
}

static NonSSO_Data String_get_non_sso_data(string_t *str) {
  NonSSO_Data result = {0, 0};
  usize size = str->non_sso.size;
  usize capacity = str->non_sso.capacity;

  u8 *size_hsb = most_sig_byte(str, sizeof(*str));
  u8 *cap_hsb = most_sig_byte(str, sizeof(*str));

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

/* Public API */
string_t string_alloc_internal(allocator_t a, char *string, usize size,
                               va_list args) {
  string_t result;
  if (size <= SSO_CAPACITY) {
    vsnprintf((char *)result.sso.str, size, string, args);
    result.sso.str[size] = '\0';
    String_set_sso_size(&result, size);
  } else {
    result.non_sso.str = (u8 *)halloc(a, size);
    vsnprintf((char *)result.non_sso.str, size, string, args);
    result.non_sso.str[size] = '\0';
    String_set_non_sso_data(&result, size, size);
  }
  return result;
}

string_t string_make(allocator_t a, char *string, ...) {
  va_list args;
  string_t result;
  usize size;
  va_start(args, string);
  size = 1 + vsnprintf(NULL, 0, string, args);
  va_end(args);
  va_start(args, string);
  result = string_alloc_internal(a, string, size, args);
  va_end(args);
  return result;
}

void string_dealloc(allocator_t a, string_t *str) {
  if (!String_is_sso(str)) {
    hdealloc(a, str->non_sso.str);
  }
}

u8 *string_to_cstr(string_t *str) {
  if (String_is_sso(str)) {
    return str->sso.str;
  } else {
    return str->non_sso.str;
  }
}

usize string_length(string_t *str) {
  if (String_is_sso(str)) {
    return String_get_sso_size(str);
  } else {
    return String_get_non_sso_data(str).size;
  }
}

usize string_capacity(string_t *str) {
  if (String_is_sso(str)) {
    return sizeof(*str) - 1;
  } else {
    return String_get_non_sso_data(str).capacity;
  }
}

/* Hashmap */

hashmap_t hashmap_make(allocator_t a, hasher_t hasher) {
  hashmap_t result = {NULL, NULL, 0, 0, hasher, a};
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
  new_hashmap.keys = (u64 *)hcalloc(hashmap->allocator, new_cap, sizeof(u64));
  new_hashmap.vals = (u64 *)halloc(hashmap->allocator, new_cap * sizeof(u64));
  new_hashmap.capacity = new_cap;
  new_hashmap.allocator = hashmap->allocator;
  new_hashmap.hasher = hashmap->hasher;
  for (; i < hashmap->capacity; ++i) {
    if (hashmap->keys[i])
      hashmap_put_u64_from_u64(&new_hashmap, hashmap->keys[i],
                               hashmap->vals[i]);
  }
  hdealloc(hashmap->allocator, hashmap->keys);
  hdealloc(hashmap->allocator, hashmap->vals);
  *hashmap = new_hashmap;
}

static void hashmap_put_u64_from_u64(hashmap_t *hashmap, u64 key, u64 val) {
  usize hash;
  if (!val)
    return;
  if (2 * hashmap->length >= hashmap->capacity) {
    hashmap_grow(hashmap, 2 * hashmap->capacity);
  }
  hash = hashmap->hasher.hash(key);
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

static void hashmap_put_u64_from_str(hashmap_t *hashmap, char *key, usize size,
                                     u64 val) {
  usize hash;
  if (!val)
    return;
  if (2 * hashmap->length >= hashmap->capacity) {
    hashmap_grow(hashmap, 2 * hashmap->capacity);
  }
  hash = hashmap->hasher.hash_bytes(key, size);
  while (true) {
    hash &= hashmap->capacity - 1; // Get the index
    if (!hashmap->keys[hash]) {
      hashmap->length += 1;
      hashmap->keys[hash] = (u64)(uintptr)key;
      hashmap->vals[hash] = val;
      return;
    } else if (strncmp((const char *)(uintptr)hashmap->keys[hash], key, size) ==
               0) {
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
  hash = hashmap->hasher.hash(key);
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

static u64 hashmap_get_u64_from_str(hashmap_t *hashmap, char *key, usize size) {
  usize hash;
  if (hashmap->length == 0)
    return 0;
  hash = hashmap->hasher.hash_bytes(key, size);
  while (true) {
    hash &= hashmap->capacity - 1;
    if (strncmp((const char *)(uintptr)hashmap->keys[hash], key, size) == 0) {
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

void hashmap_put_from_str_length(hashmap_t *hashmap, char *key, usize length,
                                 void *val) {
  hashmap_put_u64_from_str(hashmap, key, length, (u64)(uintptr)val);
}

void hashmap_put_from_str(hashmap_t *hashmap, char *key, void *val) {
  hashmap_put_u64_from_str(hashmap, key, strlen(key), (u64)(uintptr)val);
}

void *hashmap_get(hashmap_t *hashmap, void *key) {
  return (void *)(uintptr)hashmap_get_u64_from_u64(hashmap, (u64)(uintptr)key);
}

void *hashmap_get_from_u64(hashmap_t *hashmap, u64 key) {
  return (void *)(uintptr)hashmap_get_u64_from_u64(hashmap, key);
}

void *hashmap_get_from_str_length(hashmap_t *hashmap, char *key, usize length) {
  return (void *)(uintptr)hashmap_get_u64_from_str(hashmap, key, length);
}

void *hashmap_get_from_str(hashmap_t *hashmap, char *key) {
  return (void *)(uintptr)hashmap_get_u64_from_str(hashmap, key, strlen(key));
}

u64 default_hash_function(u64 key) {
  key *= 0xff51afd7ed558ccd;
  key ^= key >> 32;
  return key;
}

u64 default_hash_bytes_function(void *_ptr, usize length) {
  u64 x = 0xcbf29ce484222325;
  const char *buf = (const char *)_ptr;
  for (size_t i = 0; i < length; i++) {
    x ^= buf[i];
    x *= 0x100000001b3;
    x ^= x >> 32;
  }
  return x;
}

hasher_t default_hasher = {default_hash_function, default_hash_bytes_function};

typedef struct AHasher {
  u64 buffer;
  u64 pad;
  u64 extra_keys[2];
} AHasher;

#if !defined(AHASH_KEY1)
#define AHASH_KEY1 0
#endif

#if !defined(AHASH_KEY2)
#define AHASH_KEY2 0
#endif

#if !defined(AHASH_KEY3)
#define AHASH_KEY3                                                             \
  { 0, 0 }
#endif

#if !defined(AHASH_CONSTANT)
#define AHASH_CONSTANT 6364136223846793005
#endif

#if !defined(rotate_left)
#define rotate_left(a, b) ((a << b) | (a >> (sizeof(b) * 8 - b)))
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
  return modulo_multiply(a, b, ((u64)1 << 63));
}

static alwaysinline u64 ahash_wrapping_add(u64 a, u64 b) {
  return modulo_add(a, b, ((u64)1 << 63));
}

static const u64 PI[2] = {0xa4093822299f31d0, 0x082efa98ec4e6c89};

static alwaysinline void ahash_update(AHasher *ahash, u64 key) {
  u64 d1 = ahash_wrapping_mul((key ^ ahash->buffer), AHASH_CONSTANT);
  ahash->pad = ahash_wrapping_mul(rotate_left((ahash->pad ^ d1), (u64)8),
                                  AHASH_CONSTANT);
  ahash->buffer = rotate_left((ahash->buffer ^ ahash->pad), (u64)24);
}

static alwaysinline u64 ahash_finish(AHasher *ahash) {
  fast_u32 rot = (ahash->buffer & 63);
  u64 result = (u64)rotate_left(
      (ahash_wrapping_mul(ahash->buffer, AHASH_CONSTANT) ^ ahash->pad), rot);
  ahash->buffer = 0;
  ahash->pad = 0;
  return result;
}

AHasher ahash = {AHASH_KEY1, AHASH_KEY2, AHASH_KEY3};

u64 ahash_hash_function(u64 key) {
  ahash_update(&ahash, key);
  return ahash_finish(&ahash);
}

static alwaysinline void ahash_large_update(AHasher *ahash, u64 a, u64 b) {
  ahash_update(ahash, a ^ ahash->extra_keys[0]);
  ahash_update(ahash, b ^ ahash->extra_keys[1]);
}

u64 ahash_hash_bytes_function(void *_ptr, usize length) {
  u8 *ptr = (u8 *)_ptr;
  ahash.buffer = ahash_wrapping_mul(ahash_wrapping_add(ahash.buffer, length),
                                    AHASH_CONSTANT);
  if (length > 16) {
    u8 *tail = &ptr[length - 16];
    ahash_large_update(&ahash, *((u64 *)tail), *(((u64 *)tail) + 1));
    while (length > 16) {
      ahash_large_update(&ahash, *((u64 *)tail), *(((u64 *)tail) + 1));
      tail += 16;
      length -= 16;
    }
  } else if (length > 8) {
    ahash_large_update(&ahash, *((u64 *)ptr), *(((u64 *)ptr) + 1));
  } else {
    u64 value[2];
    /* readsmall() */
    if (length >= 2) {
      if (length >= 4) {
        value[0] = (u64)(*((u32 *)ptr));
        value[1] = (u64)(*(((u32 *)ptr) + 1));
      } else {
        value[0] = (u64)(*((u16 *)ptr));
        value[1] = (u64)(*(((u16 *)ptr) + 1));
      }
    } else {
      if (length > 0) {
        value[0] = (u64)ptr[0];
        value[0] = (u64)ptr[0];
      } else {
        value[0] = 0;
        value[0] = 0;
      }
    }
    ahash_large_update(&ahash, value[0], value[1]);
  }
  return ahash_finish(&ahash);
}

hasher_t ahash_hasher = {ahash_hash_function, ahash_hash_bytes_function};

#if defined(__cplusplus)
}
#endif

#endif
