#include "Bitmap3D.h"

#include "Base.h"
#include "Engine.h"
#include "Math.h"

static f32 camX, camY, camZ, rCos, rSin, rot, fov, xCenter, yCenter;
static u8 atlas_tile_size = 8 - 1;
static u8 atlas_dim = 64;
static s32 W;
static s32 H;

void Bitmap3D__RenderHorizon(Engine__State_t* game) {
  W = game->CANVAS_WIDTH;
  H = game->CANVAS_HEIGHT;
  s32 x, y;
  u32 color = 0;
  u32* buf = (u32*)game->local->screen.buf;
  u32 len = game->local->screen.len;

  f32 eye = Math__sin((game->local->currentTime / 20 / 100) / 500) * 2;
  f32 d = 1.0f;  // ceiling height
  f32 yd = 0, zd = 0, xd = 0;
  camX = game->local->player->x;
  camY = game->local->player->y;
  camZ = game->local->player->z;
  // camZ = -0.2 + Math__sin(game->local->player->bobPhase * 0.4) * 0.01 * game->local->player->bob
  // - game->local->player->y;
  rot = game->local->player->rot;
  rCos = Math__cos(rot);
  rSin = Math__sin(rot);
  fov = H;
  xCenter = W / 2.0f;
  yCenter = H / 2.0f;  // TODO: 3.0 ?

  for (y = 0; y < H; y++) {
    yd = ((y / (f32)H) * 2) - 1;  // -1 .. 1

    zd = (d + camZ) / yd;  // -1 .. 1
    if (yd < 0) {          // ensures ceiling is mirrored not inverted
      zd = (d - camZ) / -yd;
    }

    for (x = 0; x < W; x++) {
      xd = ((x / (f32)W) * 2) - 1;  // -1 .. 1
      xd *= zd;                     // -1 .. 1

      // yd *= 8;
      xd *= 8;

      u32 xx = (u32)(((xd * rSin) + (zd * rCos)) + camX) & atlas_tile_size;
      u32 yy = (u32)(((xd * rCos) - (zd * rSin)) + camY) & atlas_tile_size;
      color = ((u32*)game->local->atlas.buf)[(xx + yy * atlas_dim) % game->local->atlas.len];
      buf[(x + y * W) % len] = color;

      game->local->zbuf[(x + y * W) % len] = zd;
      // uncomment to render zbuf
      // ((u32*)game->local->screen.buf)[x + y * W] = (u32)0xffffffff * (u32)zd;
    }
  }

  Bitmap3D__RenderWall(game, 1, 1, 2, 2);
  Bitmap3D__RenderFloor(game);
}

void Bitmap3D__RenderWall(Engine__State_t* game, s32 x0, s32 y0, s32 x1, s32 y1) {
  s32 x, y;
  u32 color = 0xffff00ff;
  u32 ts = 8;    // tile size
  f32 d = 4.0f;  // tile size
  u32* buf = (u32*)game->local->screen.buf;
  u32 len = game->local->screen.len;
  f32 yd = 0, zd = 0, xd = 0;

  for (y = 0; y < H; y++) {
    yd = (y - yCenter) / H;  // -0.5 .. 0.5

    zd = (d + camZ) / yd;     // -8 .. 8
    if (yd < 0) {             // ensures ceiling is mirrored not inverted
      zd = (d - camZ) / -yd;  // 8 .. 0 .. 8
    }

    for (x = 0; x < W; x++) {
      xd = (x - xCenter) / H;  // -0.5 .. 0.5
      xd *= zd;                // 4 .. 0 .. 4

      u32 xx = (u32)(((xd * rSin) + (zd * rCos)) + camX) & atlas_tile_size;
      u32 yy = (u32)(((xd * rCos) - (zd * rSin)) + camY) & atlas_tile_size;

      if (xx >= x0 && xx <= x1 && yy >= y0 && yy <= y1) {
        // color = ((u32*)game->local->atlas.buf)[(xx + yy * atlas_dim) % game->local->atlas.len];
        // but i instead want to write to a projected coordinate
        color = 0xffff00ff;  // pink
        buf[(x + y * W) % len] = color;
      }
    }
  }
}

void Bitmap3D__RenderFloor(Engine__State_t* game) {
  s32 W = game->CANVAS_WIDTH;
  s32 H = game->CANVAS_HEIGHT;
  s32 x, y;
  u32 color = 0xffff00ff;
  u32 ts = 8;  // tile size
  u32* buf = (u32*)game->local->screen.buf;
  u32 len = game->local->screen.len;

  for (y = 0; y < H; y++) {
    for (x = 0; x < W; x++) {
      // buf[(x + y * W) % len] = color;
    }
  }
}

void Bitmap3D__PostProcessing(Engine__State_t* game) {
  s32 W = game->CANVAS_WIDTH;
  s32 H = game->CANVAS_HEIGHT;
  for (u32 i = 0; i < W * H; i++) {
    // u16 d = Math__map(Math__sin(game->local->currentTime / 10000), -1, 1, 800, 5000);
    u8 brightness = (u16)(5000 / (game->local->zbuf[i] * 4));

    u32 col = ((u32*)game->local->screen.buf)[i];
    u8 b = (col >> 16) & 0xff;
    u8 g = (col >> 8) & 0xff;
    u8 r = (col) & 0xff;

    r = r * brightness / 255;
    g = g * brightness / 255;
    b = b * brightness / 255;

    // ((u32*)game->local->screen.buf)[i] = 0xff000000 | b << 16 | g << 8 | r;
  }
}