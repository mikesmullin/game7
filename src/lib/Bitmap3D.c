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

// Transforms 2D world coordinates into 3D camera space
void transformToCameraSpace(f64 x, f64 y, f64* result) {
  f64 xCamSpace = ((x - 0.5) - camX) * 2;
  f64 yCamSpace = ((y - 0.5) - camY) * 2;

  result[0] = xCamSpace * rCos - yCamSpace * rSin;
  result[2] = yCamSpace * rCos + xCamSpace * rSin;
}

// Projects a 3D point onto the 2D screen
void projectToScreen(f64 x, f64 z, f64* result) {
  result[0] = (W / 2.0f) - (x / z * fov);
}

// Draws the wall between two projected points
void drawWall(
    f64* screenStart,
    f64* screenEnd,
    f64 zStart,
    f64 zEnd,
    f64 texCoordStart,
    f64 texCoordEnd,
    u32 tex,
    u32 color,
    Bitmap_t* bmp,
    Bitmap_t* tbmp) {
  u32 xStart = (u32)ceil(screenStart[0]);
  u32 xEnd = (u32)ceil(screenEnd[0]);
  if (xStart < 0) xStart = 0;
  if (xEnd > W) xEnd = W;

  f64 texScaleStart = texCoordStart * 16;
  f64 texScaleEnd = texCoordEnd * 16;

  f64 zInvStart = 1 / zStart;
  f64 zInvEnd = 1 / zEnd;

  f64 texInvStart = texScaleStart * zInvStart;
  f64 texInvEnd = texScaleEnd * zInvEnd;

  f64 xRangeInverse = 1 / (screenEnd[0] - screenStart[0]);

  for (u32 x = xStart; x < xEnd; x++) {
    f64 t = (x - screenStart[0]) * xRangeInverse;
    f64 zInvCurrent = zInvStart + (zInvEnd - zInvStart) * t;

    // if (zBufferWall[x] > zInvCurrent) continue;  // Skip if pixel is farther away
    // zBufferWall[x] = zInvCurrent;

    u32 textureX = (u32)((texInvStart + (texInvEnd - texInvStart) * t) / zInvCurrent);

    // Calculate vertical bounds for the wall at this x-position
    f64 yStart = ((-0.5) / zStart * fov + (H / 2.0f)) - 0.5;
    f64 yEnd = ((+0.5) / zEnd * fov + (H / 2.0f));

    u32 yPixelStart = (u32)ceil(yStart);
    u32 yPixelEnd = (u32)ceil(yEnd);
    if (yPixelStart < 0) yPixelStart = 0;
    if (yPixelEnd > H) yPixelEnd = H;

    f64 yRangeInverse = 1 / (yEnd - yStart);

    for (u32 y = yPixelStart; y < yPixelEnd; y++) {
      f64 yRatio = (y - yStart) * yRangeInverse;
      u32 textureY = (u32)(16 * yRatio);
      u32 texel = Bitmap__Get2DPixel(tbmp, textureX, textureY, 0xffff00ff);
      Bitmap__Set2DPixel(bmp, x, y, texel * color);
      // zBuffer[x + y * W] = 1 / zInvCurrent * 4;
    }
  }
}

void Bitmap3D__RenderWall(
    Engine__State_t* game,
    f64 x0,
    f64 y0,
    f64 x1,
    f64 y1,
    u32 tex,
    u32 color,
    f64 texCoordStart,
    f64 texCoordEnd) {
  f64 start[3], end[3];
  transformToCameraSpace(x0, y0, start);
  transformToCameraSpace(x1, y1, end);

  f64 screenStart[2], screenEnd[2];
  projectToScreen(start[0], start[2], screenStart);
  projectToScreen(end[0], end[2], screenEnd);

  if (screenStart[0] >= screenEnd[0]) return;

  drawWall(
      screenStart,
      screenEnd,
      start[2],
      end[2],
      texCoordStart,
      texCoordEnd,
      tex,
      color,
      &game->local->screen,
      &game->local->atlas);
}

void Bitmap3D__RenderHorizon(Engine__State_t* game) {
  W = game->CANVAS_WIDTH;
  H = game->CANVAS_HEIGHT;
  s32 x, y;
  u32 color = 0;
  u32* buf = (u32*)game->local->screen.buf;
  u32 len = game->local->screen.len;

  f32 yd = 0, zd = 0, xd = 0;
  camX = game->local->player.transform.position[0];
  camY = game->local->player.transform.position[1];
  camZ = game->local->player.transform.position[2];
  // camZ = -0.2 + Math__sin(game->local->player->bobPhase * 0.4) * 0.01 * game->local->player->bob
  // - game->local->player->y;
  rot = game->local->player.transform.rotation[0];
  rCos = Math__cos(rot);
  rSin = Math__sin(rot);
  fov = H;

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
      color = ((u32*)game->local->atlas.buf)[(xx + yy * atlas_dim) % game->local->atlas.len];
      buf[(x + y * W) % len] = color;

      game->local->zbuf[(x + y * W) % len] = ydd;
      // uncomment to render zbuf
      // buf[(x + y * W) % len] = (f32)((zd + 1.0f) * 0.5f * 0xffffffff);
    }
  }

  Bitmap3D__RenderWall(game, 1, 0, 1, 1, 0, 0xffff00ff, 0, 0);
  Bitmap3D__RenderFloor(game);
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
    // f32 s = Math__map(Math__sin((game->local->currentTime / 1000)), -1, 1, 0, 1);
    // brightness *= easeInQuart(s);
    brightness = easeMike(brightness, 10 * easeInQuart(0.5f));
    buf[i] = ((u32)(buf[i] * brightness) & 0xff000000) | (buf[i] & 0x00ffffff);
  }
}