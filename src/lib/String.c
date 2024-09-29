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