#if !defined(MEMORY_H_)
#define MEMORY_H_

#include "defines.h"
#include "macros.h"
#include "types.h"

/* Memory */
#if !defined(ispow2)
#define ispow2(x) ((x) <= 0 ? false : !(x & (x - 1)))
#endif

void *align_forward(void *ptr, usize alignment);
void *ptr_add(void *ptr, usize bytes);
void *ptr_sub(void *ptr, usize bytes);
usize ptr_diff(void *ptr1, void *ptr2);

void memswap(void *ptr1, void *ptr2, usize size);

#if !defined(array_memcpy)
#define array_memcopy(dest, src, count)                                        \
  memset((dest), (src), (sizeof(*(dest)) * (count)))
#endif

#if !defined(array_memmove)
#define array_memmove(dest, src, count)                                        \
  memmove((dest), (src), (sizeof(*(dest)) * (count)))
#endif

#if !defined(BYTES_MACROS)
#define BYTES_MACROS
#define s_kilobytes(x) ((x) * (1024))
#define s_megabytes(x) (s_kilobytes((x)) * (1024))
#define s_gigabytes(x) (s_megabytes((x)) * (1024))
#define s_terabytes(X) (s_gigabytes((x)) * (1024))
#endif

#endif /* MEMORY_H_ */

#if defined(MEMORY_IMPLEMENTATION)
#undef MEMORY_IMPLEMENTATION
alwaysinline void *align_forward(void *ptr, usize alignment) {
  uintptr _ptr = cast(uintptr) ptr;
  return ispow2(alignment)
             ? (cast(void *)((_ptr + (alignment - 1)) & ~(alignment - 1)))
             : NULL;
}

alwaysinline void *ptr_add(void *ptr, usize bytes) {
  return cast(void *)(cast(u8 *) ptr + bytes);
}

alwaysinline void *ptr_sub(void *ptr, usize bytes) {
  return cast(void *)(cast(u8 *) ptr - bytes);
}

alwaysinline usize ptr_diff(void *ptr1, void *ptr2) {
  return cast(usize)(cast(u8 *) ptr2 - cast(u8 *) ptr1);
}

alwaysinline void memswap(void *ptr1, void *ptr2, usize size) {
  REGISTER u8 *_ptr1 = cast(u8 *) ptr1;
  REGISTER u8 *_ptr2 = cast(u8 *) ptr2;
  REGISTER u8 value;
  if (_ptr1 != _ptr2) {
    while (size--) {
      value = *_ptr1;
      *_ptr1++ = *_ptr2;
      *_ptr2++ = value;
    }
  }
}
#endif