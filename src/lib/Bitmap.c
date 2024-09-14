#include "Bitmap.h"

#include "Base.h"

void Bitmap__Construct(Bitmap_t* bmp, u32 w, u32 h, u32 chan, u8* buf) {
  bmp->w = w;
  bmp->h = h;
  bmp->chan = chan;
  bmp->len = w * h * chan;
  bmp->buf = buf;
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
