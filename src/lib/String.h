#ifndef STRING_H
#define STRING_H

#include <stdint.h>

typedef struct _iobuf FILE;

typedef uint32_t u32;
typedef uint64_t u64;

typedef struct Arena_t Arena_t;

// "Fat Pointer"
typedef struct String8 {
  u32 size;
  char* str;
} String8;

// immutable strings
// copy-on-write
typedef struct String8Node {
  struct String8Node* next;
  String8* string;
} String8Node;

typedef struct String8List {
  String8Node* first;
  String8Node* last;
  u64 node_count;
  u64 total_size;
} String8List;

String8* str8_alloc(Arena_t* a, const char* str);
void str8__fputs(String8Node* s, FILE* stream);
String8Node* str8n__allocf(Arena_t* a, String8Node* first, const char* format, u32 len, ...);

#endif  // STRING_H