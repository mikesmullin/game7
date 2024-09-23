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
void Bitmap__Set2DPixel(Bitmap_t* bmp, u32 x, u32 y, u32 color);
u32 Bitmap__Get2DPixel(Bitmap_t* bmp, u32 x, u32 y, u32 def);
u32 Bitmap__Get2DTiledPixel(Bitmap_t* bmp, u32 x, u32 y, u32 ts, u32 tx, u32 ty, u32 def);
void Bitmap__Draw(Bitmap_t* src, Bitmap_t* dst, u32 dstX, u32 dstY);
void Bitmap__SetText(Bitmap_t* screen, Bitmap_t* glyphs, char* str, u32 x, u32 y, u32 fg, u32 bg);

#endif  // BITMAP_H
