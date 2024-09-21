#include "Bitmap3D.h"

#include "Base.h"
#include "Engine.h"
#include "Math.h"

static f32 camX, camY, camZ, rCos, rSin, rot, fov;
static u8 atlas_tile_size = 8 - 1;
static u8 atlas_dim = 64;
static u32 W;
static u32 H;
static u32 PS = 8;  // pixel super sample factor

void Bitmap3D__RenderHorizon(Engine__State_t* game) {
  W = game->CANVAS_WIDTH;
  H = game->CANVAS_HEIGHT;
  s32 x, y;
  u32 color = 0;
  u32* buf = (u32*)game->local->screen.buf;
  u32 len = game->local->screen.len;

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

  for (y = 0; y < H; y++) {
    yd = ((y / (f32)H) * 2) - 1;  // -1 .. 1
    f32 ydd = yd;
    if (ydd < 0) {  // ceiling is mirrored, not flipped
      ydd = -ydd;
    }
    yd *= PS;

    zd = (camZ * PS) / yd;
    if (yd < 0) {  // ceiling is mirrored, not flipped
      zd = -zd;
    }

    for (x = 0; x < W; x++) {
      xd = ((x / (f32)W) * 2) - 1;  // -1 .. 1
      xd *= zd;

      u32 xx = (u32)(((xd * rSin) + (zd * rCos)) + camX) & atlas_tile_size;
      u32 yy = (u32)(((xd * rCos) - (zd * rSin)) + camY) & atlas_tile_size;
      color = ((u32*)game->local->atlas.buf)[(xx + yy * atlas_dim) % game->local->atlas.len];
      buf[(x + y * W) % len] = color;

      game->local->zbuf[(x + y * W) % len] = ydd;  // 1 .. 0 .. 1
      // uncomment to render blank white canvas,
      // which will later only show zbuf on top of it
      // buf[(x + y * W) % len] = 0xffffffff;
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
    yd = ((y / (f32)H) * 2) - 1;
    f32 ydd = yd;
    if (ydd < 0) {  // ceiling is mirrored, not flipped
      ydd = -ydd;
    }
    yd *= PS;

    zd = (camZ * PS) / yd;
    if (yd < 0) {  // ceiling is mirrored, not flipped
      zd = -zd;
    }

    for (x = 0; x < W; x++) {
      xd = ((x / (f32)W) * 2) - 1;
      xd *= zd;

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

f64 easeInQuart(f64 t) {
  return t * t * t * t;
}

f64 easeOutQuart(f64 t) {
  return 1 - Math__pow(1 - t, 4);
}

f64 easeInOutQuart(f64 t) {
  return t < 0.5 ? 8 * t * t * t * t : 1 - Math__pow(-2 * t + 2, 4) / 2;
}

f64 easeMike(f32 t, f32 s) {
  return Math__pow(t, 1 + s);
}

void Bitmap3D__PostProcessing(Engine__State_t* game) {
  u32* buf = (u32*)game->local->screen.buf;
  f32* zbuf = game->local->zbuf;
  for (u32 i = 0; i < W * H; i++) {
    f32 brightness = zbuf[i];
    f32 s = Math__map(Math__sin((game->local->currentTime / 1000)), -1, 1, 0, 1);
    // brightness *= easeInQuart(s);
    brightness = easeMike(brightness, 10 * easeInQuart(s));
    buf[i] = ((u32)(buf[i] * brightness) & 0xff000000) | (buf[i] & 0x00ffffff);
  }
}