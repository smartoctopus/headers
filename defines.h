#if !defined(DEFINES_H_)
#define DEFINES_H_

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

/* CPU Macros */
#if defined(_WIN64) || defined(__x86_64__) || defined(_M_X64) || defined(__64BIT__) || defined(__powerpc64__) || defined(__ppc64__)
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

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
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

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__MIPSEL__) || defined(__mips_isa_rev)
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

#if !defined(EXTERN)
  #if defined(__cplusplus)
    #define EXTERN extern "C"
  #else
    #define EXTERN extern
  #endif /* __cplusplus */
#endif /* EXTERN */

#if !defined(DLL_EXPORT) && !defined(DLL_IMPORT)
  #if defined(_WIN32)
    #define DLL_EXPORT EXTERN __declspec(dllexport)
    #define DLL_IMPORT EXTERN __declspec(dllimport)
  #else
    #define DLL_EXPORT EXTERN __attribute__((visibility("default")))
    #define DLL_IMPORT EXTERN
  #endif
#endif

#if !defined(STATIC)
  #define STATIC static
#endif

#if !defined(STATIC_ASSERT)
#if defined(__cplusplus)
#if __cplusplus >= 201103L
#define STATIC_ASSERT(cond, msg) static_assert(cond, #msg)
#endif
#elif defined(__STDC_VERSION__)
#if __STDC_VERSION__ >= 201112l
#define STATIC_ASSERT(cond, msg) _Static_assert(cond, #msg)
#endif
#endif
#endif

#if !defined(STATIC_ASSERT)
#define STATIC_ASSERT3(cond, msg) typedef char msg[(!!(cond))*2-1]
#define STATIC_ASSERT2(cond, msg, line) STATIC_ASSERT3(cond, static_assertion_at_line_##line##msg)
#define STATIC_ASSERT1(cond, msg, line) STATIC_ASSERT2(cond, msg, line)
#define STATIC_ASSERT(cond, msg) STATIC_ASSERT1(cond, msg, __LINE__)
#endif

/* Inline */
#if !defined(INLINE)
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (defined(__cplusplus) && __cplusplus >= 199711L)
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
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || defined(COMPILER_INTEL)
#define alwaysinline __attribute__((__always_inline__)) INLINE
#else
#define alwaysinline
#endif
#endif

/* No-inline */
#if !defined(noinline)
#if defined(COMPILER_MSVC)
#define noinline __declspec(noinline)
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG) || defined(COMPILER_INTEL)
#define noinline __attribute__((noinline))
#else
#define noinline
#endif
#endif

/* Restrict */
#if !defined(RESTRICT)
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) && !defined(__cplusplus)
#define RESTRICT restrict
#elif (defined(_MSC_VER) && _MSC_VER >= 1400) || defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#define RESTRICT __restrict
#else
#define RESTRICT
#endif
#endif

/* Register */
#if !defined(REGISTER)
#define REGISTER register
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
#define PRINTF_ARGS(FMT) __attribute__((format(printf, FMT, (FMT+1))))
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

#endif /* DEFINES_H_*/