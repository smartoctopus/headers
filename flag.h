#if !defined(FLAG_H_)
#define FLAG_H_

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "defines.h"
#include "macros.h"
#include "memory.h"
#include "types.h"

#if !defined(FLAG_INT_TYPE)
#define FLAG_INT_TYPE i64
#endif

typedef struct SubMode {
  const char *name;
  const char *description;
  bool parsed;
} SubMode;

typedef enum FlagType { FLAG_BOOL, FLAG_INT, FLAG_STRING } FlagType;

typedef union FlagValue {
  bool bool_value;
  FLAG_INT_TYPE int_value;
  char *string_value;
} FlagValue;

typedef struct Flag {
  char *name;
  char *description;
  FlagType type;
  FlagValue value;
  FlagValue default_value;
} Flag;

typedef enum FlagErrorKind {
  FLAG_ERROR_NONE,
  FLAG_ERROR_NO_VALUE,
  FLAG_ERROR_INVALID_VALUE,
  FLAG_ERROR_COUNT,
  FLAG_ERROR_INVALID_FLAG,
  FLAG_ERROR_SUBMODE_COUNT,
  FLAG_ERROR_INVALID_SUBMODE
} FlagErrorKind;

typedef struct FlagError {
  FlagErrorKind kind;
  char *flag;
} FlagError;

#if !defined(SET_FLAG_PARSER_ERROR)
#define SET_FLAG_PARSER_ERROR(_parser, _kind, _flag)                           \
  do {                                                                         \
    _parser->error.kind = _kind;                                               \
    _parser->error.flag = _flag;                                               \
  } while (0)
#endif

#if !defined(flag_has_error)
#define flag_has_error(error) (error.kind != FLAG_ERROR_NONE)
#endif

#if !defined(SUBMODE_NUM)
#define SUBMODE_NUM 12
#endif

#if !defined(FLAG_NUM)
#define FLAG_NUM 128
#endif

typedef struct FlagParser {
  SubMode submodes[SUBMODE_NUM];
  Flag flags[FLAG_NUM];

  usize flags_count;
  usize submodes_count;

  bool has_skipped_program_name;

  i32 rest_argc;
  char **rest_argv;

  FlagError error;
} FlagParser;

bool *flag_submode(const char *name, const char *description);
FlagError _flag_parse_submode(int *argc, char ***argv);

#if !defined(flag_parse_submode)
#define flag_parse_submode(_argc, _argv) _flag_parse_submode(&_argc, &_argv)
#endif

bool *flag_bool(const char *name, const char *description, bool default_value);
FLAG_INT_TYPE *flag_int(const char *name, const char *description,
                        FLAG_INT_TYPE default_value);
char **flag_string(const char *name, const char *description,
                   const char *default_value);
FlagError flag_parse(int argc, char **argv);

const char *flag_error_to_string(FlagError error);
void flag_print_error(FlagError error);

bool flag_has_rest_args();
int flag_get_rest_argc();
char **flag_get_rest_argv();

#endif

#if defined(FLAG_IMPLEMENTATION)
#undef FLAG_IMPLEMENTATION

static FlagParser flag_parser;

bool *flag_submode(const char *name, const char *description) {
  FlagParser *parser = &flag_parser;

  if (parser->submodes_count < SUBMODE_NUM) {
    SubMode *submode = &parser->submodes[parser->submodes_count++];
    submode->name = name;
    submode->description = description;
    submode->parsed = false;
    return &submode->parsed;
  } else {
    SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_SUBMODE_COUNT, cast(char *) name);
    return NULL;
  }
}

FlagError _flag_parse_submode(int *argc, char ***argv) {
  FlagParser *parser = &flag_parser;
  usize iter = 0;
  char *flag;
  bool found;


  if (!parser->has_skipped_program_name) {
    parser->has_skipped_program_name = true;
    *argc -= 1;
    *argv += 1;
  }

  flag = *argv[0];

  SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_NONE, flag);

  for (iter = 0; iter < parser->submodes_count; ++iter) {
    if (strcmp(flag, parser->submodes[iter].name) == 0) {
      parser->submodes[iter].parsed = true;
      found = true;
    }
  }
  if (!found) {
    SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_INVALID_SUBMODE, flag);
    return parser->error;
  }
  *argc -= 1;
  *argv += 1;

  return parser->error;
}

static Flag *flag_new(FlagType type, const char *name,
                      const char *description) {
  FlagParser *parser = &flag_parser;

  if (parser->flags_count < FLAG_NUM) {
    Flag *flag = &parser->flags[parser->flags_count++];
    flag->type = type;
    flag->name = cast(char *) name;
    flag->description = cast(char *) description;
    return flag;
  } else {
    SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_COUNT, cast(char *) name);
    return NULL;
  }
}

bool *flag_bool(const char *name, const char *description, bool default_value) {
  Flag *flag = flag_new(FLAG_BOOL, name, description);
  flag->default_value.bool_value = default_value;
  flag->value.bool_value = default_value;
  return &flag->value.bool_value;
}

FLAG_INT_TYPE *flag_int(const char *name, const char *description,
                        FLAG_INT_TYPE default_value) {
  Flag *flag = flag_new(FLAG_INT, name, description);
  flag->default_value.int_value = default_value;
  flag->value.int_value = default_value;
  return &flag->value.int_value;
}

char **flag_string(const char *name, const char *description,
                   const char *default_value) {
  Flag *flag = flag_new(FLAG_STRING, name, description);
  flag->default_value.string_value = cast(char *) default_value;
  flag->value.string_value = cast(char *) default_value;
  return &flag->value.string_value;
}

static bool flag_str_to_int(char *str, FLAG_INT_TYPE *result) {
  FlagParser *parser = &flag_parser;
  char *start = str;
  bool is_negative = false;
  FLAG_INT_TYPE sign = 1;
  persistant const FLAG_INT_TYPE char_to_digit[256] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  if (str != NULL) {
    is_negative = *str == '-';
    sign = is_negative * -1;
    str += is_negative;
    if (isdigit(*str)) {
      while (isdigit(*str)) {
        *result *= 10;
        *result += char_to_digit[cast(u8) * str];
        str++;
      }
      if (strcmp(str, "G") == 0) {
        *result = s_gigabytes(*result);
      } else if (strcmp(str, "M") == 0) {
        *result = s_megabytes(*result);
      } else if (strcmp(str, "K") == 0) {
        *result = s_kilobytes(*result);
      } else if (strcmp(str, "") != 0) {
        SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_INVALID_VALUE, start);
        return false;
      }
      *result *= is_negative;
      return true;
    } else {
      SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_INVALID_VALUE,
                            cast(char *) start);
    }
  } else {
    SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_NO_VALUE, start);
  }
  return false;
}

#if !defined(next_flag)
#define next_flag(_argc, _argv, _iter) _next_flag(_argc, _argv, &_iter)
#endif

static char *_next_flag(int argc, char **argv, usize *iter) {
  *iter += 1;
  if (*iter <= cast(usize) argc)
    return argv[*iter];
  return NULL;
}

noinline FlagError flag_parse(int argc, char **argv) {
  FlagParser *parser = &flag_parser;
  usize iter = cast(usize)(!parser->has_skipped_program_name);

  for (; iter < cast(usize) argc; ++iter) {
    char *flag = argv[iter];
    if (*flag == '-') {
      usize i = 0;
      bool found = false;
      flag++;
      if (*flag == '-')
        flag++;
      for (i = 0; i < parser->flags_count; ++i) {
        if (strcmp(flag, parser->flags[i].name) == 0 && !found) {
          switch (parser->flags[i].type) {
          case FLAG_BOOL: {
            parser->flags[i].value.bool_value = true;
            found = true;
          } break;
          case FLAG_INT: {
            FLAG_INT_TYPE int_value = 0;
            flag = next_flag(argc, argv, iter);
            if (flag_str_to_int(flag, &int_value)) {
              parser->flags[i].value.int_value = int_value;
            } else {
              SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_INVALID_VALUE, flag);
              parser->rest_argc = argc - iter;
              parser->rest_argv = argv + iter;
              return parser->error;
            }
            found = true;
          } break;
          case FLAG_STRING: {
            char *string_value = next_flag(argc, argv, iter);
            if (string_value != NULL) {
              parser->flags[i].value.string_value = string_value;
            } else {
              parser->rest_argc = argc - iter;
              parser->rest_argv = argv + iter;
              /* The error was set by the next_flag function */
              return parser->error;
            }
            found = true;
          } break;
          default: {
            fprintf(stderr, "Unreachable");
          } break;
          }
        }
      }
      if (!found) {
        SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_INVALID_FLAG, flag);
        parser->rest_argc = argc - iter;
        parser->rest_argv = argv + iter;
        return parser->error;
      }
    } else {
      SET_FLAG_PARSER_ERROR(parser, FLAG_ERROR_INVALID_FLAG, flag);
      parser->rest_argc = argc - iter;
      parser->rest_argv = argv + iter;
      return parser->error;
    }
  }

  parser->rest_argc = argc - iter;
  parser->rest_argv = argv + iter;
  return parser->error;
}

const char *flag_error_to_string(FlagError error) {
  static const char *error_string[] = {
      "No Error",     "No value passed",     "Invalid value",  "Count error",
      "Invalid flag", "Submode count error", "Invalid submode"};

  if (error.kind >= FLAG_ERROR_NONE && error.kind <= FLAG_ERROR_INVALID_FLAG) {
    return error_string[error.kind];
  }

  return "Unreachable";
}

void flag_print_error(FlagError error) {
  if (error.kind != FLAG_ERROR_NONE) {
    fprintf(stderr, "Error %s on flag %s\n", flag_error_to_string(error),
            error.flag);
  }
}

void print_usage(FILE *stream) {
  FlagParser *parser = &flag_parser;
  usize iter = 0;
  for (iter = 0; iter < parser->flags_count; ++iter) {
    Flag flag = parser->flags[iter];

    fprintf(stream, "    -%s\n", flag.name);
    fprintf(stream, "        %s\n", flag.description);
    switch (flag.type) {
    case FLAG_BOOL: {
      fprintf(stream, "        Default: %s\n",
              flag.default_value.bool_value ? "true" : "false");
    } break;
    case FLAG_STRING: {
      fprintf(stream, "        Default: %s\n", flag.default_value.string_value);
    } break;
    case FLAG_INT: {
      fprintf(stream, "        Default: " PRINT_I64 "\n",
              flag.default_value.int_value);
    } break;
    default: {
      fprintf(stderr, "Unreachable");
    } break;
    }
  }
}

bool flag_has_rest_args() {
  FlagParser *parser = &flag_parser;
  return parser->rest_argc > 0 && parser->error.kind == FLAG_ERROR_INVALID_FLAG;
}

int flag_get_rest_argc() {
  FlagParser *parser = &flag_parser;
  return parser->rest_argc;
}

char **flag_get_rest_argv() {
  FlagParser *parser = &flag_parser;
  return parser->rest_argv;
}

#endif