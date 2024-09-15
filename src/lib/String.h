#ifndef STRING_H
#define STRING_H

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

#endif  // STRING_H