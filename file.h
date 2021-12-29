#if !defined(FILE_H_)
#define FILE_H_

#include <stdio.h>

#include "types.h"
#include "allocators.h"

typedef struct file_t {
  char *content;
  usize length;
  allocator_t allocator;
} file_t;

file_t read_file(allocator_t a, char *path);
void free_file(file_t *file);

#if !defined(file_for)
#define file_for(_index, _value, _file, _body)                                 \
  do {                                                                         \
    usize _index = 0;                                                          \
    char _value = _file.content[_index];                                       \
    for (; _index < _file.length; _value = _file.content[++_index])            \
      _body                                                                    \
  } while (0)
#endif

#endif

#if defined(FILE_IMPLEMENTATION)
#include "macros.h"

file_t read_file(allocator_t a, char *path) {
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
  content = cast(char *) xmalloc(a, length + 1);
  if (length && fread(content, length, 1, file) != 1) {
    fclose(file);
    xfree(a, content);
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

void free_file(file_t *file) {
  xfree(file->allocator, file->content);
  file->length = 0;
  file->content = NULL;
}
#endif