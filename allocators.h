#if !defined(ALLOCATORS_H_)
#define ALLOCATORS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "types.h"

/* Allocators */
typedef void *(*malloc_func_t)(usize);
typedef void *(*realloc_func_t)(void *, usize);
typedef void (*free_func_t)(void *);

typedef struct allocator_t {
  malloc_func_t _alloc;
  realloc_func_t _realloc;
  free_func_t _dealloc;
} allocator_t;

#undef alloc
#undef realloc
#undef dealloc
#undef calloc

#define alloc(_a, _size) _a._alloc(_size)

#define calloc(_a, _num, _size) _a._alloc((_num) * (_size))

#define realloc(_a, _ptr, _size) _a._realloc(_ptr, _size)

#define dealloc(_a, _ptr) _a._dealloc(_ptr)

#if !defined(are_same_allocators)
#define are_same_allocators(x, y)                                              \
  (x._alloc == y._alloc && x._realloc == y._realloc && x._dealloc == y._dealloc)
#endif

/* xmalloc */
extern void xmalloc_handler(void);

void *xmalloc(allocator_t a, usize size);
void *xcalloc(allocator_t a, usize num, usize size);
void *xrealloc(allocator_t a, void *ptr, usize size);
void xfree(allocator_t a, void *ptr);

#endif /* ALLOCATORS_H_ */

#if defined(ALLOCATORS_IMPLEMENTATION)
#undef ALLOCATORS_IMPLEMENTATION
/* Std-allocator */
allocator_t std_allocator = {malloc, realloc, free};

void *xmalloc(allocator_t a, usize size) {
  void *ptr = alloc(a, size);
  if (ptr == NULL) {
    xmalloc_handler();
  }
  return ptr;
}

void *xcalloc(allocator_t a, usize num, usize size) {
  void *ptr = calloc(a, num, size);
  if (ptr == NULL) {
    xmalloc_handler();
  }
  memset(ptr, 0, num * size);
  return ptr;
}

void *xrealloc(allocator_t a, void *ptr, usize size) {
  void *_ptr = realloc(a, ptr, size);
  if (ptr == NULL) {
    xmalloc_handler();
  }
  return _ptr;
}

void xfree(allocator_t a, void *ptr) { dealloc(a, ptr); }

#if defined(IMPORT_XMALLOC_HANDLER)
#undef IMPORT_XMALLOC_HANDLER
noreturn void xmalloc_handler(void) {
  fprintf(stderr, "[Allocator Error] Couldn't allocate memory!\n");
  exit(EXIT_FAILURE);
}
#endif

#endif