#ifndef BITMAP_H
#define BITMAP_H

#include "Arena.h"
#include "Base.h"

typedef struct {
  u32 w;
  u32 h;
  u64 len;
  u8 chan;
  u8* buf;
} Bitmap_t;

void Bitmap__Alloc(Arena_t* a, Bitmap_t* bmp, u32 w, u32 h, u32 chan);
void Bitmap__Init(Bitmap_t* bmp, u32 w, u32 h, u32 chan);
void Bitmap__Draw(Bitmap_t* src, Bitmap_t* dst, u32 dstX, u32 dstY);

#endif  // BITMAP_H
