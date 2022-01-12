#if !defined(ALLOCATORS2_H_)
#define ALLOCATORS2_H_

#include "defines.h"
#include "types.h"

/* Allocators */
typedef enum AllocationType {
  ALLOC_MALLOC,
  ALLOC_REALLOC,
  ALLOC_FREE
} AllocationType;

#if !defined(ALLOCATOR_FUNC)
#define ALLOCATOR_FUNC(name)                                                   \
  void *name(AllocationType action, void *data, void *ptr, usize size)
#endif

typedef void *(*allocator_func_t)(AllocationType, void *, void *, usize);

typedef struct allocator_t {
  allocator_func_t func;
  void *data;
} allocator_t;

extern void *s_calloc(allocator_t a, usize num, usize size);

#if !defined(s_alloc)
#define s_alloc(_a, _size) (_a.func(ALLOC_MALLOC, _a.data, NULL, _size))
#endif

#if !defined(s_realloc)
#define s_realloc(_a, _ptr, _size)                                             \
  (_a.func(ALLOC_REALLOC, _a.data, _ptr, _size))
#endif

#if !defined(s_dealloc)
#define s_dealloc(_a, _ptr) (_a.func(ALLOC_FREE, _a.data, _ptr, 0))
#endif

#if !defined(allocators_eq)
#define allocators_eq(_a, _b) (_a.func == _b.func && _a.data == _b.data)
#endif

/* System Allocators */
alwaysinline void *system_alloc(usize size);
alwaysinline void system_dealloc(void *ptr, usize size);
/* Implemented simply using a couple of _system_alloc and system_dealloc +
 * memcpy() */
alwaysinline void *system_realloc(void *ptr, usize prev_size, usize new_size);

/* Std allocator */
extern allocator_t std_allocator;

/* xmalloc */
typedef void *(*xmalloc_handler_t)(void);
allocator_t make_xallocator(allocator_t a, xmalloc_handler_t xmalloc_handler);
extern void *default_xmalloc_handler(void);

/* Arena allocators */
allocator_t make_arena_allocator(allocator_t a, usize arena_size);

#endif

#if defined(ALLOCATORS_IMPLEMENTATION)
#undef ALLOCATORS_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arrays.h"
#include "macros.h"

/* s_calloc */
void *s_calloc(allocator_t a, usize num, usize size) {
  usize new_size = num * size;
  void *ptr = s_alloc(a, new_size);
  memset(ptr, 0, new_size);
  return ptr;
}

/* System allocators */
#if defined(OS_WINDOWS)
alwaysinline void *system_alloc(usize size) {
  return VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

alwaysinline void system_dealloc(void *ptr, usize size) {
  unused(size);
  VirtualFree(ptr, 0, MEM_RELEASE);
}
#else
#if !defined(__USE_MISC)
#define __USE_MISC 1
#endif
#include <sys/mman.h>
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

alwaysinline void *system_realloc(void *ptr, usize prev_size, usize new_size) {
  void *new_ptr = system_alloc(new_size);
  memcpy(new_ptr, ptr, prev_size);
  system_dealloc(ptr, prev_size);
  return new_ptr;
}

/* Std allocators */
ALLOCATOR_FUNC(std_alloc_func) {
  switch (action) {
  case ALLOC_MALLOC: {
    return malloc(size);
  } break;
  case ALLOC_REALLOC: {
    return realloc(ptr, size);
  } break;
  case ALLOC_FREE: {
    free(ptr);
  } break;
  }
  return NULL;
}

allocator_t std_allocator = {std_alloc_func, NULL};

typedef struct xmalloc_info_t {
  allocator_t allocator;
  xmalloc_handler_t xmalloc_handler;
} xmalloc_info_t;

static void *xmalloc(allocator_t a, usize size,
                     xmalloc_handler_t xmalloc_handler) {
  void *ptr = s_alloc(a, size);
  if (ptr == NULL) {
    return xmalloc_handler();
  }
  return ptr;
}

static void *xrealloc(allocator_t a, void *ptr, usize size,
                      xmalloc_handler_t xmalloc_handler) {
  void *new_ptr = s_realloc(a, ptr, size);
  if (new_ptr == NULL) {
    return xmalloc_handler();
  }
  return new_ptr;
}

static void xfree(allocator_t a, void *ptr) { s_dealloc(a, ptr); }

ALLOCATOR_FUNC(xallocator_func) {
  xmalloc_info_t *xdata = cast(xmalloc_info_t *) data;
  switch (action) {
  case ALLOC_MALLOC: {
    return xmalloc(xdata->allocator, size, xdata->xmalloc_handler);
  } break;
  case ALLOC_REALLOC: {
    return xrealloc(xdata->allocator, ptr, size, xdata->xmalloc_handler);
  } break;
  case ALLOC_FREE: {
    xfree(xdata->allocator, ptr);
  } break;
  }
  return NULL;
}

allocator_t make_xallocator(allocator_t a, xmalloc_handler_t xmalloc_handler) {
  allocator_t result;
  xmalloc_info_t *data =
      cast(xmalloc_info_t *) s_alloc(a, sizeof(xmalloc_info_t));
  data->allocator = a;
  data->xmalloc_handler = xmalloc_handler;
  result.data = cast(void *) data;
  result.func = xallocator_func;
  return result;
}

#if defined(IMPORT_XMALLOC_HANDLER)
void *default_xmalloc_handler(void) {
  fprintf(stderr, "[Allocation Error] Couldn't allocate memory! Aborting...\n");
  abort();
  return NULL;
}
#endif

/* Arena Allocators */
#if !defined(ARENA_SIZE)
#define ARENA_SIZE 1024 * 1024
#endif

#if !defined(ARENA_ALIGNMENT)
#define ARENA_ALIGNMENT (sizeof(void *))
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
  usize size = ALIGN_UP(max(min_size, ARENA_SIZE), ARENA_ALIGNMENT);
  arena_block_t block;
  arena->ptr = cast(u8 *) system_alloc(size);
  arena->end = arena->ptr + size;
  block.ptr = arena->ptr;
  block.size = size;
  array_push(arena->blocks, block);
}

void *arena_alloc(void *data, usize size) {
  arena_t *arena = cast(arena_t *) data;
  void *ptr;
  if (size > (usize)(arena->end - arena->ptr)) {
    arena_grow(arena, size);
  }
  ptr = arena->ptr;
  arena->ptr = cast(u8 *) ALIGN_UP_PTR(arena->ptr + size, ARENA_ALIGNMENT);
  return ptr;
}

void *arena_realloc(void *ptr, usize size) {
  fprintf(stderr, "Cannot call s_realloc on arena allocator! Aborting...");
  abort();
  return NULL;
}

void arena_dealloc(allocator_t *a) {
  arena_t *arena = cast(arena_t *) a->data;
  arena_block_t *it;
  for (it = arena->blocks; it != array_end(arena->blocks); ++it) {
    system_dealloc(it->ptr, it->size);
  }
  array_dealloc(arena->blocks);
}

ALLOCATOR_FUNC(arena_alloc_func) {
  switch (action) {
  case ALLOC_MALLOC: {
    return arena_alloc(data, size);
  } break;
  case ALLOC_REALLOC: {
    return arena_realloc(ptr, size);
  } break;
  case ALLOC_FREE: {
  } break;
  }
  return NULL;
}

allocator_t make_arena_allocator(allocator_t a, usize size) {
  allocator_t result;
  arena_t *arena = cast(arena_t *) s_alloc(a, sizeof(arena_t));
  arena->end = NULL;
  arena->ptr = NULL;
  array_init(a, arena->blocks);
  result.func = arena_alloc_func;
  result.data = cast(void *) arena;
  return result;
}

#endif