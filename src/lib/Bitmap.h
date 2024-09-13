#ifndef BITMAP_H
#define BITMAP_H

#include "Base.h"

typedef struct {
  u32 w;
  u32 h;
  u64 len;
  u8 chan;
  u8* buf;
} Bitmap_t;

void Bitmap__Construct(Bitmap_t* bmp, u32 w, u32 h, u32 chan, u8* buf);
void Bitmap__Draw(Bitmap_t* src, Bitmap_t* dst, u32 dstX, u32 dstY);

#endif  // BITMAP_H
