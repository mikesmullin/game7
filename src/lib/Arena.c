#include "Arena.h"

Arena_t Arena__Alloc(u64 sz) {
  void* p = malloc(sz);
  Arena_t a = {.buf = p, .pos = p, .end = p + sz};
  return a;
}

void* Arena__Push(Arena_t* a, u64 sz) {
  ASSERT_CONTEXT(a->pos + sz < a->end, "Arena exhausted.");
  // memset(a->pos, 0, sz);  // zero-fill
  a->pos += sz;
  return a->pos - sz;
}

void Arena__Free(Arena_t* a) {
  free(a->buf);
}
