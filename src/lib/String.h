#ifndef STRING_H
#define STRING_H

#include <stdint.h>
typedef uint32_t u32;

typedef struct Arena_t Arena_t;

// "Fat Pointer"
typedef struct String_t {
  u32 size;
  char* str;
} String_t;

String_t* String__alloc(Arena_t* arena, const char* str);

#endif  // STRING_H