#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct Arena_t Arena_t;
typedef struct Engine__State_t Engine__State_t;

typedef struct Bitmap_t {
  u32 w;
  u32 h;
  u64 len;
  u8 chan;
  u8* buf;
} Bitmap_t;

Bitmap_t* Bitmap__Prealloc(Arena_t* a);
void Bitmap__Alloc(Arena_t* a, Bitmap_t* bmp, u32 w, u32 h, u32 chan);
void Bitmap__Init(Bitmap_t* bmp, u32 w, u32 h, u32 chan);
void Bitmap__Fill(Bitmap_t* bmp, u32 x0, u32 y0, u32 x1, u32 y1, u32 color);
void Bitmap__Set2DPixel(Bitmap_t* bmp, u32 x, u32 y, u32 color);
u32 Bitmap__Get2DPixel(Bitmap_t* bmp, u32 x, u32 y, u32 def);
u32 Bitmap__Get2DTiledPixel(Bitmap_t* bmp, u32 x, u32 y, u32 ts, u32 tx, u32 ty, u32 def);
void Bitmap__Draw(Bitmap_t* src, Bitmap_t* dst, u32 dstX, u32 dstY);
void Bitmap__SetText(Bitmap_t* screen, Bitmap_t* glyphs, char* str, u32 x, u32 y, u32 fg, u32 bg);
void Bitmap__DebugText(
    Bitmap_t* dst, Bitmap_t* src, u32 x, u32 y, u32 fg, u32 bg, char* format, ...);
void Bitmap__DebugText2(Engine__State_t* game, u32 x, u32 y, u32 fg, u32 bg, char* format, ...);

#endif  // BITMAP_H
