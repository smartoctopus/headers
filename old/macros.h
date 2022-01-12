#if !defined(MACROS_H_)
#define MACROS_H_

/* Utility macros */

#if !defined(cast)
#define cast(type) (type)
#endif

#if !defined(swap)
#define swap(type, a, b)                                                       \
  do {                                                                         \
    type tmp = (a);                                                            \
    a = b;                                                                     \
    b = tmp;                                                                   \
  } while (0)
#endif

#if !defined(reinterpret_cast)
#define reinterpret_cast(type, var) *cast(type *)(&var)
#endif

#if !defined(countof)
#define countof(x) ((sizeof(x)/sizeof(x[0]))
#endif

#if !defined(offsetof)
#define offsetof(Type, field) (&(((Type *)0)->field))
#endif

#if !defined(alignof)
#define alignof(Type)                                                          \
  offsetof(                                                                    \
      struct {                                                                 \
        char c;                                                                \
        Type field;                                                            \
      },                                                                       \
      field)
#endif

#if !defined(unused)
#if defined(COMPILER_MSVC)
#define unused(x) (__pragma(warning(suppress : 4100))(x))
#elif defined(GNUC_FEATURES)
#define unused(x) __attribute__((__unused__)) (x)
#else
#define unused(x) (void)(sizeof(x))
#endif
#endif

#if !defined(JOIN_MACROS)
#define JOIN_MACROS
#define _INTERNAL_JOIN(a, b) a##b
#define join2(a, b) _INTERNAL_JOIN(a, b)
#define join3(a, b, c) join2(join2(a, b), c)
#define join4(a, b, c) join2(join2(join2(a, b), c), d)
#endif

#undef min
#undef max
#undef abs

#if !defined(bit_flag)
#define bit_flag(x) (1 << (x))
#endif

#if !defined(min)
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#if !defined(max)
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#if !defined(min3)
#define min3(a, b, c) min(min(a, b), c)
#endif

#if !defined(max3)
#define max3(a, b, c) max(max(a, b), c)
#endif

#if !defined(clamp)
#define clamp(x, lower_bound, higher_bound)                                    \
  min(max((x), (lower_bound)), (higher_bound))
#endif

#if !defined(is_between)
#define is_between(x, lower_bound, higher_bound)                               \
  (((lower_bound) <= (x)) && ((x)) <= (upper_bound))
#endif

#if !defined(_abs)
#define _abs(x) ((x) < 0 ? -(x) : (x))
#endif

#if !defined(set_flag)
#define set_flag(flag, bit) ((flag) |= (bit))
#define toggle_flag(flag, bit) ((flag) ^= (bit))
#define unset_flag(flag, bit) ((flag) &= ~(bit))
#define has_flag(flag, bit) ((flag) & (bit))
#endif

/* Likely/Unlikely */
#if !defined(likely) && !defined(unlikely)
#if defined(GNUC_FEATURES)
#define likely(expr) __builtin_expect(!!(expr), 1)
#define unlikely(expr) __builtin_expect(!!(expr), 0)
#else
#define likely(expr) (!!(expr))
#define unlikely(expr) (!!(expr))
#endif
#endif

#endif /* MACROS_H_ */