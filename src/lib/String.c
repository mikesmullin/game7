#include "String.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "Arena.h"

String_t* String__alloc(Arena_t* arena, const char* str) {
  u64 len = strlen(str) + 1;
  void* p = Arena__Push(arena, len);
  memcpy(p, str, len);
  String_t* s = Arena__Push(arena, sizeof(String_t));
  s->size = len;
  s->str = p;
  return s;
}

String_t* String__allocf(Arena_t* arena, const char* format, u32 maxLen, ...) {
  char p[maxLen];
  va_list args;
  va_start(args, maxLen);
  vsnprintf(p, maxLen, format, args);
  va_end(args);
  return String__alloc(arena, p);
}

char* String__concat(Arena_t* arena, const char* a, const char* b) {
  return String__allocf(arena, "%s%s", 255, a, b)->str;
}

char* Path__join(Arena_t* arena, ...) {
  char s[255] = "";
  va_list args;
  va_start(args, arena);
  u32 i = 0, ii = 0;
  const char* arg = NULL;
  while ((arg = va_arg(args, const char*)) != NULL) {
    if (i != 0) s[ii++] = '/';
    i = 0;
    while (0 != arg[i]) {
      s[ii] = arg[i];
      i++;
      ii++;
    }
  }
  va_end(args);
  s[ii] = 0;

  return String__alloc(arena, s)->str;
}
