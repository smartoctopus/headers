#if !defined(ALLOCATORS_H_)
#define ALLOCATORS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "types.h"

/* Allocators */
#if !defined(S_MALLOC)
#define S_MALLOC malloc
#endif

#if !defined(S_REALLOC)
#define S_REALLOC realloc
#endif

#if !defined(S_FREE)
#define S_FREE free
#endif

extern void xmalloc_handler(void);

void *xmalloc(usize size);
void *xcalloc(usize length, usize size);
void *xrealloc(void *ptr, usize size);
void xfree(void *ptr);

#endif /* ALLOCATORS_H_ */

#if defined(ALLOCATORS_IMPLEMENTATION)
#undef ALLOCATORS_IMPLEMENTATION
void *xmalloc(usize size) {
  void *ptr = S_MALLOC(size);
  if (ptr == NULL) {
    xmalloc_handler();
  }
  return ptr;
}

void *xcalloc(usize length, usize size) {
  void *ptr = xmalloc(length * size);
  memset(ptr, 0, length * size);
  return ptr;
}

void *xrealloc(void *ptr, usize size) {
  void *_ptr = S_REALLOC(ptr, size);
  if (ptr == NULL) {
    xmalloc_handler();
  }
  return _ptr;
}

void xfree(void *ptr) { S_FREE(ptr); }

#if defined(IMPORT_XMALLOC_HANDLER)
#undef IMPORT_XMALLOC_HANDLER
noreturn void xmalloc_handler(void) {
  fprintf(stderr, "Couldn't allocate memory");
  exit(EXIT_FAILURE);
}
#endif

#endif