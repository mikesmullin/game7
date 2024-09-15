#ifndef ARENA_H
#define ARENA_H

#include "Base.h"

// Arena/Linear/Bump allocator
typedef struct {
  void* buf;
  void* pos;
  void* end;
} Arena_t;

Arena_t Arena__Alloc(u64 sz);
void* Arena__Push(Arena_t* a, u64 sz);
void Arena__Free(Arena_t* a);

#endif  // ARENA_H