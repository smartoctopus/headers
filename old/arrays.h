#if !defined(ARRAYS_H_)
#define ARRAYS_H_

#include "allocators.h"
#include "defines.h"
#include "macros.h"
#include "types.h"

/* Variable length arrays */
typedef struct ArrayHeader {
  usize length;
  usize capacity;
  allocator_t allocator;
} ArrayHeader;

void *__array_set_capacity(void *array, usize capacity, usize element_size);
void *__array_xmalloc_handler(void);

#if !defined(Array)
#define Array(T) T *
#endif

#if !defined(GROW_FORMULA)
#define GROW_FORMULA(x) (2 * (x) + 8)
#endif

#if !defined(ARRAY_HEADER)
#define ARRAY_HEADER(x) (cast(ArrayHeader *)(x) - 1)
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
    void **_array = cast(void **) & (x);                                       \
    allocator_t x_allocator =                                                  \
        make_xallocator(_allocator, __array_xmalloc_handler);                  \
    ArrayHeader *_header = cast(ArrayHeader *)                                 \
        s_alloc(x_allocator, sizeof(ArrayHeader) + sizeof(*(x)) * (_cap));     \
    _header->length = 0;                                                       \
    _header->capacity = _cap;                                                  \
    _header->allocator = x_allocator;                                          \
    *_array = cast(void *)(_header + 1);                                       \
  } while (0)
#endif

#if !defined(array_init)
#define array_init(_allocator, x) array_init_helper(_allocator, x, 8)
#endif

#if !defined(array_dealloc)
#define array_dealloc(x)                                                       \
  do {                                                                         \
    ArrayHeader *_header = ARRAY_HEADER(x);                                    \
    s_dealloc(_header->allocator, _header);                                    \
  } while (0)
#endif

#if !defined(array_set_capacity)
#define array_set_capacity(x, capacity)                                        \
  do {                                                                         \
    if (x) {                                                                   \
      void **_array = cast(void **) & (x);                                     \
      *_array = __array_set_capacity((x), (capacity), sizeof(*(x)));           \
    }                                                                          \
  } while (0)
#endif

#if !defined(array_grow)
#define array_grow(x, cap)                                                     \
  do {                                                                         \
    isize _cap = GROW_FORMULA(array_capacity(x));                              \
    if (_cap < cast(isize) cap)                                                \
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
        allocators_eq(ARRAY_HEADER(x)->allocator,                        \
                            ARRAY_HEADER(x)->allocator)) {                     \
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

#endif /* ARRAYS_H_ */

#if defined(ARRAYS_IMPLEMENTATION)
#undef ARRAYS_IMPLEMENTATION
#include "string.h"

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
  new_header = cast(ArrayHeader *) s_alloc(a, size);
  memmove(new_header, header,
          sizeof(ArrayHeader) + element_size * header->length);
  new_header->length = header->length;
  new_header->capacity = capacity;
  new_header->allocator = a;
  s_dealloc(a, header);
  return cast(void *)(new_header + 1);
}

void *__array_xmalloc_handler(void) {
  fprintf(stderr, "[Allocation Error] Couldn't allocate memory for arrays! Aborting...\n");
  abort();
  return NULL;
}

#endif