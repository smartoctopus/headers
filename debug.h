#if !defined(DEBUG_H_)
#define DEBUG_H_

#include "defines.h"
#include "types.h"

/* Debugging */
/* I know there are builtin functions in various compilers but this is more
 * readable*/
#if !defined(debug_trap)
#define debug_trap() abort()
#endif

void assert_helper(const char *prefix, const char *condition, const char *file,
                   i32 line, const char *msg);

#if !defined(assert_help)
#define assert_help(cond, msg)                                                 \
  do {                                                                         \
    if (!(cond)) {                                                             \
      assert_helper("Assertion Failure", #cond, __FILE__, cast(i32) __LINE__,  \
                    msg);                                                      \
      debug_trap();                                                            \
    }                                                                          \
  } while (0)
#endif

#if !defined(Assert)
#define Assert(cond) assert_help(cond, NULL)
#endif

#if DEBUG
#if !defined(Debugassert)
#define Debugassert(cond) s_assert(cond)
#endif
#endif

#if !defined(Panic)
#define Panic(msg)                                                             \
  do {                                                                         \
    assert_helper("Panic", NULL, __FILE__, cast(i32) __LINE__, msg);           \
    debug_trap();                                                              \
  } while (0)
#endif

#if defined(IMPORT_ASSERT_HELPER)
#undef IMPORT_ASSERT_HELPER
noreturn void assert_helper(const char *prefix, const char *condition,
                            const char *file, i32 line, const char *msg) {
  if (condition == NULL) {
    fprintf(stderr, "%s: %d: %s: %s", file, line, prefix, msg);
  } else {
    fprintf(stderr, "%s: %d: %s: %s: %s", file, line, prefix, condition, msg);
  }
  exit(1);
}
#endif

#endif /* DEBUG_H_ */