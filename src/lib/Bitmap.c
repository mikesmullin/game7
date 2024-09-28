#include "Bitmap.h"

#include <stdarg.h>
#include <stdio.h>

#include "Arena.h"
#include "Log.h"

// TODO: rename to Alloc
Bitmap_t* Bitmap__Prealloc(Arena_t* a) {
  return Arena__Push(a, sizeof(Bitmap_t));
}

// TODO: merge into Init
void Bitmap__Alloc(Arena_t* a, Bitmap_t* bmp, u32 w, u32 h, u32 chan) {
  Bitmap__Init(bmp, w, h, chan);

  bmp->buf = Arena__Push(a, w * h * chan * sizeof(u32));
}

void Bitmap__Init(Bitmap_t* bmp, u32 w, u32 h, u32 chan) {
  bmp->w = w;
  bmp->h = h;
  bmp->chan = chan;
  bmp->len = w * h * chan;
}

// set 32-bit pixel color at given 2d coordinate
void Bitmap__Set2DPixel(Bitmap_t* bmp, u32 x, u32 y, u32 color) {
  // if (x >= 0 && x < bmp->w && y >= 0 && y < bmp->h) {
  if (!(x + y < 0 || x + y > bmp->len)) {
    ((u32*)bmp->buf)[x + y * bmp->w] = color;
  }
}

u32 Bitmap__Get2DPixel(Bitmap_t* bmp, u32 x, u32 y, u32 def) {
  // if (x >= 0 && x < bmp->w && y >= 0 && y < bmp->h) {
  if (!(x + y < 0 || x + y > bmp->len)) {
    return ((u32*)bmp->buf)[x + y * bmp->w];
  }
  return def;
}

/**
 * Get a pixel from an atlas of tiled artwork.
 *
 * @param bmp Source bitmap
 * @param x x coordinate within tile (overflow will wrap/repeat)
 * @param y y coordinate within tile (overflow will wrap/repeat)
 * @param ts tile size (assumed square dimensions)
 * @param tx tile index (x)
 * @param ty tile index (y)
 * @param def default/error color, if bitmap can't be read
 * @return u32 pixel color
 */
u32 Bitmap__Get2DTiledPixel(Bitmap_t* bmp, u32 x, u32 y, u32 ts, u32 tx, u32 ty, u32 def) {
  u32 xt = ((u32)x & (ts - 1)) + (ts * tx);
  u32 yt = (u32)y & (ts - 1) + (ts * ty);
  return Bitmap__Get2DPixel(bmp, xt, yt, def);
}

// a.k.a. Blit/Copy
void Bitmap__Draw(Bitmap_t* src, Bitmap_t* dst, u32 dX, u32 dY) {
  u32 xp, yp;
  for (u32 y = 0; y < src->h; y++) {
    yp = dY + y;
    if (yp < 0 || yp >= dst->h) {
      continue;
    }

    for (u32 x = 0; x < src->w; x++) {
      xp = dX + x;
      if (xp < 0 || xp >= dst->w) {
        continue;
      }

      u32 o = ((u32*)src->buf)[x + y * src->w];
      if (o > 0) {  // bitmask transparency
        ((u32*)dst->buf)[xp + yp * dst->w + 1] = o;
      }
    }
  }
}

void Bitmap__Set2(
    Bitmap_t* dst,
    Bitmap_t* src,
    u32 xOffs,
    u32 yOffs,
    u32 xo,
    u32 yo,
    u32 w,
    u32 h,
    u32 fg,
    u32 bg) {
  for (u32 y = 0; y < h; y++) {
    u32 yPix = y + yOffs;
    if (yPix < 0 || yPix >= dst->h) continue;

    for (u32 x = 0; x < w; x++) {
      u32 xPix = x + xOffs;
      if (xPix < 0 || xPix >= dst->w) continue;

      u32 srcp = ((u32*)src->buf)[(x + xo) + (y + yo) * src->w];
      if (srcp == 0xffffffff) {  // bitmask: white = transparency
        if (0x00 != bg >> 16) {
          ((u32*)dst->buf)[xPix + yPix * dst->w] = /*srcp * */ bg;
        }
      } else {
        ((u32*)dst->buf)[xPix + yPix * dst->w] = /*srcp * */ fg;
      }
    }
  }
}

void Bitmap__SetText(Bitmap_t* screen, Bitmap_t* glyphs, char* str, u32 x, u32 y, u32 fg, u32 bg) {
  for (u32 i = 0; 0 != str[i]; i++) {
    u32 ch = str[i];
    u32 xx = ch % 32;
    u32 yy = ch / 32;
    Bitmap__Set2(screen, glyphs, x + i * 4, y, xx * 4, yy * 6, 3, 6, fg, bg);
  }
}

void Bitmap__DebugText(
    Bitmap_t* dst, Bitmap_t* src, u32 x, u32 y, u32 fg, u32 bg, char* format, ...) {
  u32 len = 255;
  char buf[len];
  va_list args;
  va_start(args, format);
  vsnprintf(buf, len, format, args);
  va_end(args);

  Bitmap__SetText(dst, src, buf, x, y, fg, bg);
}