#include "Bitmap3D.h"

#include "Base.h"
#include "Engine.h"
#include "Math.h"

static f32 camX, camY, camZ, rCos, rSin, rot, fov;
static u8 floor_tile_idxX = 0;
static u8 atlas_dim = 64;
static u32 W;
static u32 H;
static u32 PS;
static f32 Wh;

// rotate plane along axis
void rot2d(f32 a, f32 b, f32 rSin, f32 rCos, f32* r) {
  // see: https://www.desmos.com/calculator/uvrekqtsis
  // a oscillates along the a-axis the circle intersecting a,y
  // a-axis range +/- a+b+e
  // where e = the extra required for the circle to intersect at a,b
  // b-axis range is exactly the same, but offset by 90 degree phase (obviously)
  r[0] = -(b * rSin) + (a * rCos);  // a-axis
  r[1] = (a * rSin) + (b * rCos);   // b-axis
}

// project Syn (screen) to Tz (texture) coord in perspective
f32 perspective(f32 Syn) {
  f32 Tz;
  camZ = MATH_CLAMP(0, camZ, Wh);
  f32 Vhn = ((camZ / Wh) * 2.0f) - 1.0f;  // -1 = floor, 0 = middle, 1 = ceiling
  f32 Vh = Vhn * Wh;
  f32 Ty = 1.0f;
  if (Syn > 0) {
    // floor
  }
  if (Syn < 0) {
    // ceiling
    Vh *= -1.0f;  // determines eye position
    Ty *= -1.0f;  // ensure ceiling is mirrored, not flipped
  }
  Ty *= Syn;
  Tz = (Wh + Vh);  // tile repeat count
  Tz /= Ty;        // perspective projection (depth)
                   //  near = few repeats, far = many repeats
  return Tz;
}

f32 deg2rad(f32 deg) {
  return (Math__PI / 180.0f) * deg;
}

void Bitmap3D__RenderWall2(
    Engine__State_t* game, f64 x0, f64 y0, f64 x1, f64 y1, u32 tex, u32 color, f64 tx, f64 ty) {
  f64 br = 0.5f;  // block radius
  f64 s = Math__map(Math__triangleWave(game->local->currentTime, 1000), -1, 1, 0.0f, 1.0f);
  f64 um1 = 2.0f;  // unknown multiplier (affects opacity between 0-0.01)

  f32 cX = -camX;
  f32 cY = camY;
  f32 cZ = -camZ + 0.5f;
  f32 rS = rSin;
  f32 rC = rCos;

  // transformToCameraSpace()
  // Translates and scales the world coordinates (x0, y0) relative to
  // the camera's position (xCam, yCam).
  f64 xc0 = ((x0 + 0.25) - cX) * um1;  // right hinged door
  f64 yc0 = ((y0 - 1.25) - cY) * um1;  // sliding close from right
  // Rotates the point (xc0, yc0) using the precomputed sine and cosine values (rSin, rCos)
  // and calculates the transformed coordinates xx0 and zz0. u0 and l0 are the upper and lower
  // boundaries of the wall in 3D space (z-axis).
  f32 r[2];
  rot2d(xc0, yc0, rSin, rCos, r);
  f64 xx0 = r[0];
  f64 zz0 = r[1];
  f64 u0 = (-1 - cZ) * um1;  // t |\.
  f64 l0 = (+1 - cZ) * um1;  // b |/

  // Similarly, translates and scales the second endpoint (x1, y1) of the wall
  // relative to the camera.
  f64 xc1 = ((x1 + 0.25) - cX) * um1;  // left hinged door
  f64 yc1 = ((y1 + 0) - cY) * um1;     // sliding close from left
  // Rotates the second point (xc1, yc1) and computes its transformed coordinates (xx1, zz1).
  rot2d(xc1, yc1, rSin, rCos, r);
  f64 xx1 = r[0];
  f64 zz1 = r[1];
  f64 u1 = (-1 - cZ) * um1;  // t /|
  f64 l1 = (+1 - cZ) * um1;  // b \|

  // projectToScreen()
  // Projects the x-coordinates of the wall's two endpoints from 3D space
  // to 2D screen space using perspective projection (fov).
  f64 xCenter = W / 2.0f;
  f64 yCenter = H / 2.0f;

  f64 xPixel0 = xCenter - (xx0 / zz0 * fov);
  f64 xPixel1 = xCenter - (xx1 / zz1 * fov);

  if (xPixel0 >= xPixel1) return;  // don't render wall behind player

  // Determines the pixel boundaries (xp0, xp1) on the screen
  // where the wall will be rendered. These are clamped to the screen's width.
  u32 xp0 = (u32)(xPixel0);
  u32 xp1 = (u32)(xPixel1);
  if (xp0 < 0) xp0 = 0;
  if (xp1 > W) xp1 = W;

  // Projects the y-coordinates of the wall's top and bottom edges
  // onto the screen using perspective projection.
  f64 yPixel00 = (u0 / zz0 * fov + yCenter);
  f64 yPixel01 = (l0 / zz0 * fov + yCenter);
  f64 yPixel10 = (u1 / zz1 * fov + yCenter);
  f64 yPixel11 = (l1 / zz1 * fov + yCenter);

  // texture mapping
  // Precomputes inverse depth values (iz0, iz1) for depth interpolation across the wall.
  f64 iz0 = 1 / zz0;
  f64 iz1 = 1 / zz1;
  f64 iza = iz1 - iz0;
  // Prepares for interpolating texture coordinates and depth across the x-axis of the wall segment.
  f64 ixt0 = tx * iz0;
  f64 ixta = ty * iz1 - ixt0;
  f64 iw = 1 / (xPixel1 - xPixel0);

  // Iterates over each x-coordinate between xp0 and xp1.
  // Interpolates the inverse depth (iz) and
  // checks if the current pixel is closer than the existing value in zBufferWall.
  // If so, calculates the corresponding x texture coordinate.
  for (u32 x = xp0; x < xp1; x++) {
    f64 pr = (x - xPixel0) * iw;
    f64 iz = iz0 + iza * pr;

    u32 xTex = (u32)((ixt0 + ixta * pr) / iz);

    // Interpolates the projected y-coordinates of the wall's
    // top and bottom edges for the current x-position.
    f64 yPixel0 = yPixel00 + (yPixel10 - yPixel00) * pr - 0.5;
    f64 yPixel1 = yPixel01 + (yPixel11 - yPixel01) * pr;

    // Clamps the y-coordinates to the screen height.
    u32 yp0 = (u32)(yPixel0);
    u32 yp1 = (u32)(yPixel1);
    if (yp0 < 0) yp0 = 0;
    if (yp1 > H) yp1 = H;

    // Iterates over each y-coordinate between yp0 and yp1,
    // calculates the corresponding y texture coordinate,
    // and sets the pixel color
    f64 ih = 1 / (yPixel1 - yPixel0);
    for (u32 y = yp0; y < yp1; y++) {
      game->local->zbuf[x + y * W] = 1.0f;
      color = 0xff000000 | ((u8)y) << 8 | (u8)x;
      Bitmap__Set2DPixel(&game->local->screen, x, y, color);
    }
  }
}

/**
 * This method renders a wall segment in 3D space. The parameters are:
 * @param x0, y0: Start coordinates of the wall segment in the world.
 * @param x1, y1: End coordinates of the wall segment.
 * @param tex: Texture ID to be used for the wall.
 * @param color: Color multiplier for the texture.
 * @param xt0, xt1: Texture coordinates on the x-axis (left and right).
 */
void Bitmap3D__RenderWall(
    Engine__State_t* game, f64 x0, f64 y0, f64 x1, f64 y1, u32 tex, u32 color, f64 tx, f64 ty) {
  f64 br = 0.5f;   // block radius
  f64 um1 = 2.0f;  // unknown multiplier (affects opacity between 0-0.01)

  f32 cX = -camX;
  f32 cY = camY;
  f32 cZ = -camZ + 0.5f;
  f32 rS = rSin;
  f32 rC = rCos;

  // transformToCameraSpace
  // Translates and scales the world coordinates (x0, y0) relative to
  // the camera's position (xCam, yCam).
  f64 xc0 = ((x0 - br) - cX) * um1;
  f64 yc0 = ((y0 - br) - cY) * um1;
  // Rotates the point (xc0, yc0) using the precomputed sine and cosine values (rSin, rCos)
  // and calculates the transformed coordinates xx0 and zz0. u0 and l0 are the upper and lower
  // boundaries of the wall in 3D space (z-axis).
  f64 xx0 = xc0 * rC - yc0 * rS;
  f64 zz0 = yc0 * rC + xc0 * rS;
  // f32 r[2];
  // rot2d(xc0, yc0, rS, rC, r);
  // f64 xx0 = r[0];
  // f64 zz0 = r[1];
  f64 u0 = ((-br) - cZ) * um1;
  f64 l0 = ((+br) - cZ) * um1;

  // Similarly, translates and scales the second endpoint (x1, y1) of the wall
  // relative to the cera.
  f64 xc1 = ((x1 - br) - cX) * um1;
  f64 yc1 = ((y1 - br) - cY) * um1;
  // Rotates the second point (xc1, yc1) and computes its transformed coordinates (xx1, zz1).
  f64 xx1 = xc1 * rC - yc1 * rS;
  f64 zz1 = yc1 * rC + xc1 * rS;
  // rot2d(xc1, yc1, rS, rC, r);
  // f64 xx1 = r[0];
  // f64 zz1 = r[1];
  f64 u1 = ((-br) - cZ) * um1;
  f64 l1 = ((+br) - cZ) * um1;

  // projectToScreen
  // Projects the x-coordinates of the wall's two endpoints from 3D space
  // to 2D screen space using perspective projection (fov).
  f64 xCenter = W / 2.0f;
  f64 yCenter = H / 2.0f;

  f64 xPixel0 = xCenter - (xx0 / zz0 * fov);
  f64 xPixel1 = xCenter - (xx1 / zz1 * fov);

  if (xPixel0 >= xPixel1) return;  // don't render wall behind player

  // Determines the pixel boundaries (xp0, xp1) on the screen
  // where the wall will be rendered. These are clamped to the screen's width.
  u32 xp0 = (u32)(xPixel0);
  u32 xp1 = (u32)(xPixel1);
  if (xp0 < 0) xp0 = 0;
  if (xp1 > W) xp1 = W;

  // Projects the y-coordinates of the wall's top and bottom edges
  // onto the screen using perspective projection.
  f64 yPixel00 = (u0 / zz0 * fov + yCenter);
  f64 yPixel01 = (l0 / zz0 * fov + yCenter);
  f64 yPixel10 = (u1 / zz1 * fov + yCenter);
  f64 yPixel11 = (l1 / zz1 * fov + yCenter);

  // texture mapping
  // Precomputes inverse depth values (iz0, iz1) for depth interpolation across the wall.
  f64 iz0 = 1 / zz0;
  f64 iz1 = 1 / zz1;
  f64 iza = iz1 - iz0;
  // Prepares for interpolating texture coordinates and depth across the x-axis of the wall segment.
  f64 ixt0 = tx * iz0;
  f64 ixta = ty * iz1 - ixt0;
  f64 iw = 1 / (xPixel1 - xPixel0);

  // LOG_DEBUGF(
  //     "x0 %3.0f xc0 %3.0f xx0 %3.0f u0 %3.0f l0 %3.0f xPixel0 %3.0f xp0 %3.0f yPixel00 %3.0f iz0
  //     %3.0f"
  //     "%3.0f ixt0 %3.0f",
  //     x0,
  //     xc0,
  //     xx0,
  //     u0,
  //     l0,
  //     xPixel0,
  //     xp0,
  //     yPixel00,
  //     iz0,
  //     ixt0);

  // LOG_DEBUGF(
  //     "y0 %3.0f yc0 %3.0f zz0 %3.0f u0 %3.0f l0 %3.0f xPixel1 %3.0f xp1 %3.0f yPixel01 %3.0f iz0
  //     "
  //     "%3.0f"
  //     "%3.0f iyt0 %3.0f",
  //     y0,
  //     yc0,
  //     zz0,
  //     u0,
  //     l0,
  //     xPixel1,
  //     xp1,
  //     yPixel01,
  //     iz0,
  //     ixta);

  // Iterates over each x-coordinate between xp0 and xp1.
  // Interpolates the inverse depth (iz) and
  // checks if the current pixel is closer than the existing value in zBufferWall.
  // If so, calculates the corresponding x texture coordinate.
  for (u32 x = xp0; x < xp1; x++) {
    f64 pr = (x - xPixel0) * iw;
    f64 iz = iz0 + iza * pr;

    u32 xTex = (u32)((ixt0 + ixta * pr) / iz);

    // Interpolates the projected y-coordinates of the wall's
    // top and bottom edges for the current x-position.
    f64 yPixel0 = yPixel00 + (yPixel10 - yPixel00) * pr - 0.5;
    f64 yPixel1 = yPixel01 + (yPixel11 - yPixel01) * pr;

    // Clamps the y-coordinates to the screen height.
    u32 yp0 = (u32)(yPixel0);
    u32 yp1 = (u32)(yPixel1);
    if (yp0 < 0) yp0 = 0;
    if (yp1 > H) yp1 = H;

    // Iterates over each y-coordinate between yp0 and yp1,
    // calculates the corresponding y texture coordinate,
    // and sets the pixel color
    f64 ih = 1 / (yPixel1 - yPixel0);
    for (u32 y = yp0; y < yp1; y++) {
      f64 pry = (y - yPixel0) * ih;
      u32 yTex = (u32)(16 * pry);

      if (x >= 0 && x < W && y >= 0 && y < H) {
        game->local->zbuf[x + y * W] = 1 / iz * 4;
        color = Bitmap__Get2DTiledPixel(
            &game->local->atlas,
            xTex,
            yTex,
            game->local->ATLAS_TILE_SIZE,
            tx,
            ty,
            0xffffffff);
        Bitmap__Set2DPixel(&game->local->screen, x, y, color);
      }
    }
  }
}

void Bitmap3D__RenderHorizon(Engine__State_t* game) {
  W = game->CANVAS_WIDTH;
  H = game->CANVAS_HEIGHT;
  PS = game->local->WORLD_TILE_SCALE;
  Wh = game->local->WORLD_HEIGHT * game->local->WORLD_TILE_SCALE;
  u32 color = 0;
  u32* buf = (u32*)game->local->screen.buf;
  u32 len = game->local->screen.len;
  f32 txs = 4.0f;  // texture scale (affects ceiling and floors; trying to match walls)

  camX = game->local->player.transform.position[2];
  camY = game->local->player.transform.position[0];
  camZ = game->local->player.transform.position[1];
  // camZ = -0.2 + Math__sin(game->local->player->bobPhase * 0.4) * 0.01 *
  // game->local->player->bob
  // - game->local->player->y;
  rot = -game->local->player.transform.rotation[0];
  f32 rad = deg2rad(rot);
  rCos = Math__cos(rad);
  rSin = Math__sin(rad);
  fov = H / 2.0f;

  // S = screen
  // W = world
  // V = view
  // M = model
  // T = texture (tile/block units, more accurately)

  // y/x = cartesian
  // n   = normalized
  // h   = homogeneous

  // r   = rotated (camera rotation)
  // o   = translated (camera position)
  // g   = gridded / tiled / repeat-xy

  // TODO: are we Y-UP or Z-UP rn? based on the draw-screen/read-texture loop
  //   screens and textures are 2d so x,y only
  //   the perception of depth becomes z by convenience
  //   but it should be thought of differently
  //   differentiate world v. screen/texture axes

  // x-axis = (-)left/right
  // y-axis = forward/backward(-)
  // z-axis = up/down

  if (true) {
    for (s32 Sy = 0; Sy < H; Sy++) {
      f32 Syn = ((Sy / (f32)H) * 2) - 1;  // -1 .. 1
      f32 Syh = Syn;
      if (Syh < 0) {  // ceiling is mirrored, not flipped
        Syh = -Syh;   // 1 .. 0 .. 1
      }

      f32 Tz = perspective(Syn);

      for (s32 Sx = 0; Sx < W; Sx++) {
        f32 Sxn = ((Sx / (f32)W) * 2) - 1;

        f32 Tx = Sxn;
        // repeat-x count, which is actually:
        //   by passing a decimal,
        //   we cause texture pixels to be skipped or repeated
        //   when rendered on screen
        //   so smaller numbers = larger textures
        //   larger numbers = smaller textures
        // pinching at 0,0 origin
        //   inherited from `/= Ty` earlier
        Tx *= Tz;

        // rotate on XZ plane along Z axis (Z-UP)
        f32 Wr[2] = {Tx, Tz};
        rot2d(Tx, Tz, rSin, rCos, Wr);

        // translation must happen after rotation
        // or the rotation origin seems detached from the player
        f32 Wxo = Wr[0] + camX;
        f32 Wyo = Wr[1] + camY;

        color = Bitmap__Get2DTiledPixel(
            &game->local->atlas,
            Wxo * txs,
            Wyo * txs,
            game->local->ATLAS_TILE_SIZE,
            floor_tile_idxX,  // = Math__map(Math__sin(game->local->currentTime / 1000), -1, 1, 0,
                              // 3),
            0,
            0xffff00ff);
        Bitmap__Set2DPixel(&game->local->screen, Sx, Sy, color);

        game->local->zbuf[(Sx + Sy * W) % len] = Syh;
        // uncomment to render all white, revealing zbuf
        // Bitmap__Set2DPixel(&game->local->screen, Sx, Sy, 0xffffffff);
      }
    }
  } else {
    memset(game->local->screen.buf, 0, game->local->screen.len);
  }

  Bitmap3D__RenderWall2(game, 2, 1, 2, 2, 0, 0xffff00ff, 0, 0);
  // Bitmap3D__RenderWall(game, 1, 0, 1, 1, 0, 0xffff00ff, 2, 0);
  // Bitmap3D__RenderWall(game, 0, 0, 1, 1, 0, 0xffff00ff, 1, 0);
  // Bitmap3D__RenderWall(game, 0, 0, -1, 1, 0, 0xffff00ff, 1, 0);
  // Bitmap3D__RenderWall(game, 0, 0, -1, -1, 0, 0xffff00ff, 1, 0);
  // Bitmap3D__RenderWall(game, -1, 0, 0, 0, 0, 0xffff00ff, 1, 0);
  // Bitmap3D__RenderWall(game, -1, -1, 0, 0, 0, 0xffff00ff, 2, 0);

  // Bitmap3D__RenderFloor(game);
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

f64 easeInOutQuart(f64 t) {  // in/out 0 .. 1
  return t < 0.5 ? 8 * t * t * t * t : 1 - Math__pow(-2 * t + 2, 4) / 2;
}

u32 alpha_blend(u32 bottom, u32 top) {
  // Extract RGBA components
  u32 src_a = (top >> 24) & 0xff;
  u32 src_r = (top >> 16) & 0xff;
  u32 src_g = (top >> 8) & 0xff;
  u32 src_b = top & 0xff;

  u32 dst_a = (bottom >> 24) & 0xff;
  u32 dst_r = (bottom >> 16) & 0xff;
  u32 dst_g = (bottom >> 8) & 0xff;
  u32 dst_b = bottom & 0xff;

  // Normalize alpha values to [0, 1]
  f32 alpha_src = src_a / 255.0f;
  f32 alpha_dst = dst_a / 255.0f;

  // Apply alpha blending formula
  u32 out_r = (u32)((alpha_src * src_r) + ((1 - alpha_src) * dst_r));
  u32 out_g = (u32)((alpha_src * src_g) + ((1 - alpha_src) * dst_g));
  u32 out_b = (u32)((alpha_src * src_b) + ((1 - alpha_src) * dst_b));
  u32 out_a = (u32)((alpha_src * 255.0f) + ((1 - alpha_src) * alpha_dst * 255.0f));

  // Combine components back into a single u32
  u32 result = (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
  return result;
}

void Bitmap3D__PostProcessing(Engine__State_t* game) {
  u32* buf = (u32*)game->local->screen.buf;
  f32* zbuf = game->local->zbuf;
  for (u32 i = 0; i < W * H; i++) {
    f32 b1 = zbuf[i];  // +1 .. 0 .. +1
    b1 = 1.0f - b1;    // invert so that the horizon has most alpha
    // f32 cutoff = Math__map(Math__sin(game->local->currentTime / 5000), -1, 1, 0.5, 1);  // 0 ..
    // 1
    f32 cutoff = 0.95f;  // higher number = further visibility distance
    b1 = Math__map(MATH_CLAMP(0, b1, cutoff), 0, cutoff, 0, 1);
    b1 = 255.0f * b1;                      // 0 .. 255
    u32 fog = (u32)b1 << 24 & 0xff000000;  // 0x00 .. 0xff alpha 000000 black
    u32 color = alpha_blend(buf[i], fog);  // blackness of varying alpha overlaid on existing color

    buf[i] = color;
  }
}