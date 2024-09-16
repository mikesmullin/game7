#include "String.h"

#include <string.h>

String8* str8_alloc(Arena_t* a, const char* str) {
  u64 len = strlen(str) + 1;
  void* p = Arena__Push(a, len);
  memcpy(p, str, len);
  String8* s = Arena__Push(a, sizeof(String8));
  s->size = len;
  s->str = p;
  return s;
}
