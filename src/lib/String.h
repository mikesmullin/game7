#ifndef STRING_H
#define STRING_H

#include <stdio.h>

#include "Arena.h"
#include "Base.h"

// "Fat Pointer"
typedef struct {
  u32 size;
  char* str;
} String8;

// immutable strings
// copy-on-write
typedef struct String8Node {
  struct String8Node* next;
  String8* string;
} String8Node;

typedef struct {
  String8Node* first;
  String8Node* last;
  u64 node_count;
  u64 total_size;
} String8List;

String8* str8_alloc(Arena_t* a, const char* str);
void str8__fputs(String8Node* s, FILE* stream);
String8Node* str8n__allocf(Arena_t* a, String8Node* first, const char* format, u32 len, ...);

#endif  // STRING_H