#include "String.h"

#include <stdio.h>
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

void str8__fputs(String8Node* s, FILE* stream) {
  String8Node* c = s;
  while (c) {
    fputs(c->string->str, stream);
    c = c->next;
  }
  fputs("\n", stream);
}

String8Node* str8n__allocf(Arena_t* a, String8Node* first, const char* format, u32 len, ...) {
  void* p = Arena__Push(a, len);  // c_str

  va_list args;
  va_start(args, len);
  vsnprintf(p, len, format, args);
  va_end(args);

  String8* s = str8_alloc(a, p);
  s->size = len;
  s->str = p;

  String8Node* sn = Arena__Push(a, sizeof(String8Node));
  sn->string = s;
  sn->next = NULL;

  if (!first) {
    first = sn;
  } else {
    String8Node* c = first;
    while (c->next) {
      c = c->next;
    }
    c->next = sn;
  }
  return first;
}