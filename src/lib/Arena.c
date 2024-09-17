#include "Arena.h"

Arena_t Arena__Alloc(u64 sz) {
  // LOG_DEBUGF("arena malloc %llu", sz);
  void* p = malloc(sz);
  // LOG_DEBUGF("arena p %p", p);
  ASSERT_CONTEXT(NULL != p, "Arena malloc request rejected by OS.");
  Arena_t a = {.buf = p, .pos = p, .end = p + sz};
  return a;
}

Arena_t* Arena__SubAlloc(Arena_t* a, u64 sz) {
  Arena_t* sa = Arena__Push(a, sizeof(Arena_t));
  sa->buf = Arena__Push(a, sz);
  sa->pos = sa->buf;
  sa->end = sa->buf + sz;
  return sa;
}

void* Arena__Push(Arena_t* a, u64 sz) {
  ASSERT_CONTEXT(
      a->pos + sz < a->end,
      "Arena exhausted. pos: %p, sz: %d, end: %p, cap: %d, ask: %d, over: %d",
      a->pos,
      sz,
      a->end,
      a->end - a->buf,
      sz,
      a->buf - a->pos - sz);
  // memset(a->pos, 0, sz);  // zero-fill
  a->pos += sz;
  return a->pos - sz;
}

void Arena__Free(Arena_t* a) {
  free(a->buf);
}

void Arena__Reset(Arena_t* a) {
  a->pos = a->buf;
}