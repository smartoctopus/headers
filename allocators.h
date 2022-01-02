#if !defined(ALLOCATORS_H_)
#define ALLOCATORS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "macros.h"
#include "types.h"

/* Allocators */
struct allocator_t;
typedef void *(*malloc_func_t)(struct allocator_t *, usize);
typedef void *(*realloc_func_t)(struct allocator_t *, void *, usize);
typedef void (*free_func_t)(struct allocator_t *, void *);

typedef struct allocator_t {
  malloc_func_t _alloc;
  realloc_func_t _realloc;
  free_func_t _dealloc;
  void *data;
} allocator_t;

#undef alloc
#undef realloc
#undef dealloc
#undef calloc

#define s_alloc(_a, _size) (_a._alloc(&_a, _size))

#define s_calloc(_a, _num, _size) _a._alloc(&_a, (_num) * (_size))

#define s_realloc(_a, _ptr, _size) _a._realloc(&_a, _ptr, _size)

#define s_dealloc(_a, _ptr) _a._dealloc(&_a, _ptr)

#if !defined(are_same_allocators)
#define are_same_allocators(x, y)                                              \
  (x._alloc == y._alloc && x._realloc == y._realloc && x._dealloc == y._dealloc)
#endif

extern allocator_t std_allocator;

/* xmalloc */
extern void xmalloc_handler(void);

void *xmalloc(allocator_t a, usize size);
void *xcalloc(allocator_t a, usize num, usize size);
void *xrealloc(allocator_t a, void *ptr, usize size);
void xfree(allocator_t a, void *ptr);

/* System Allocators */
void *system_alloc(usize size);
void system_dealloc(void *ptr, usize size);

/* Arena allocator */
#if !defined(ARENA_SIZE)
#define ARENA_SIZE 1024 * 1024
#endif

#if !defined(ARENA_ALIGNMENT)
#define ARENA_ALIGNMENT (sizeof(void *))
#endif

allocator_t make_arena_allocator(allocator_t a, usize size);
void *arena_alloc(allocator_t *a, usize size);
void *arena_realloc(allocator_t *a, void *ptr, usize size);
void arena_dealloc(allocator_t *a);

#endif /* ALLOCATORS_H_ */

#if defined(ALLOCATORS_IMPLEMENTATION)
#undef ALLOCATORS_IMPLEMENTATION
#include "memory.h"
#include "debug.h"
#include "arrays.h"

/* Std-allocator */
void *std_malloc(allocator_t *a, usize size) {
  return malloc(size);
}

void *std_realloc(allocator_t *a, void *ptr, usize size) {
  return realloc(ptr, size);
}

void std_free(allocator_t *a, void *ptr) {
  free(ptr);
}

allocator_t std_allocator = {std_malloc, std_realloc, std_free, NULL};

void *xmalloc(allocator_t a, usize size) {
  void *ptr = s_alloc(a, size);
  if (ptr == NULL) {
    xmalloc_handler();
  }
  return ptr;
}

void *xcalloc(allocator_t a, usize num, usize size) {
  void *ptr = s_calloc(a, num, size);
  if (ptr == NULL) {
    xmalloc_handler();
  }
  memset(ptr, 0, num * size);
  return ptr;
}

void *xrealloc(allocator_t a, void *ptr, usize size) {
  void *_ptr = s_realloc(a, ptr, size);
  if (ptr == NULL) {
    xmalloc_handler();
  }
  return _ptr;
}

void xfree(allocator_t a, void *ptr) { s_dealloc(a, ptr); }

#if defined(IMPORT_XMALLOC_HANDLER)
#undef IMPORT_XMALLOC_HANDLER
noreturn void xmalloc_handler(void) {
  fprintf(stderr, "[Allocator Error] Couldn't allocate memory!\n");
  exit(EXIT_FAILURE);
}
#endif

/* Arena allocator */
#define ALIGN_DOWN(n, a) ((n) & ~((a) - 1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n) + (a) - 1, (a))
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

#if defined(OS_WINDOWS)
void *system_alloc(usize size) {
  return VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

void system_dealloc(void *ptr, usize size) {
  unused(size);
  VirtualFree(ptr, 0, MEM_RELEASE);  
}
#else
#ifndef __USE_MISC
#define __USE_MISC 1
#endif
#include <sys/mman.h>
void *system_alloc(usize size) {
  return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void system_dealloc(void *ptr, usize size) {
  if (munmap(ptr, size) < 0) {
    fprintf(stderr, "System allocator failed");
    abort();
  }
}
#endif

void arena_error_dealloc(allocator_t *a, void *ptr) { return; }

static void arena_grow(arena_t *arena, usize min_size) {
  usize size = ALIGN_UP(max(min_size, ARENA_SIZE), ARENA_ALIGNMENT);
  arena_block_t block;
  arena->ptr = cast(u8 *)system_alloc(size); 
  arena->end = arena->ptr + size;
  block.ptr = arena->ptr;
  block.size = size;
  array_push(arena->blocks, block);
}

allocator_t make_arena_allocator(allocator_t a, usize size) {
  allocator_t result;
  arena_t *arena = cast(arena_t *)s_alloc(a, sizeof(arena_t));
  arena->end = NULL;
  arena->ptr = NULL;
  array_init(a, arena->blocks);
  result._alloc = arena_alloc;
  result._realloc = arena_realloc;
  result._dealloc = arena_error_dealloc;
  result.data = cast(void *)arena;
  return result;
}

void *arena_alloc(allocator_t *a, usize size) {
  arena_t *arena = cast(arena_t *)a->data;
  void *ptr;
  if (size > (usize)(arena->end - arena->ptr)) {
    arena_grow(arena, size);
  }
  ptr = arena->ptr;
  arena->ptr = cast(u8 *)ALIGN_UP_PTR(arena->ptr + size, ARENA_ALIGNMENT);
  return ptr;
}

void *arena_realloc(allocator_t *a, void *ptr, usize size) {
  fprintf(stderr, "Cannot call s_realloc on arena allocator! Aborting...");
  abort();
  return NULL;
}

void arena_dealloc(allocator_t *a) {
  arena_t *arena = cast(arena_t *)a->data;
  arena_block_t *it;
  for (it = arena->blocks; it != array_end(arena->blocks); ++it) {
    system_dealloc(it->ptr, it->size);
  }
  array_dealloc(arena->blocks);
}

#endif