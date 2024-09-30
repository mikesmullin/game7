#include "Bitmap3D.h"

#include "../game/Logic.h"
#include "Color.h"
#include "Easing.h"
#include "Engine.h"
#include "GLMShim.h"
#include "Log.h"
#include "Math.h"

static f32 W, H;
static u32 BLACK = 0xff000000;
static u32 LIME = 0xff00ff00;
static u32 PINK = 0xffff00ff;

// Perspective projection and draw function with camera
void Bitmap3D__Set3DPixel(
    Bitmap_t* dst, f32 x, f32 y, f32 z, u32 color, mat4 model, mat4 view, mat4 projection) {
  // Create a vec4 for the point in model space (homogeneous coordinates)
  vec4 model_point = {x, y, z, 1.0f};

  // Transform the point by the model matrix (from model space to world space)
  vec4 world_point;
  glms_mat4_mulv(model, model_point, world_point);

  // Transform the point by the view (camera) matrix (from world space to camera space)
  vec4 camera_point;
  glms_mat4_mulv(view, world_point, camera_point);

  // Apply perspective projection (from camera space to clip space)
  vec4 clip_point;
  glms_mat4_mulv(projection, camera_point, clip_point);

  // Perform perspective division (convert homogeneous to normalized device coordinates)
  if (clip_point[3] != 0.0f) {
    clip_point[0] /= clip_point[3];
    clip_point[1] /= clip_point[3];
    clip_point[2] /= clip_point[3];
  }

  // Convert normalized device coordinates to screen space
  s32 screen_x = (s32)((clip_point[0] + 1.0f) * 0.5f * W);
  s32 screen_y = (s32)((1.0f - clip_point[1]) * 0.5f * H);

  // Draw the pixel in the RGBA buffer
  Bitmap__Set2DPixel(dst, screen_x, screen_y, color);
  // LOG_DEBUGF("screen_x %d screen_y %d", screen_x, screen_y);
  // Bitmap__Set2DPixel(dst, 11, 20, PINK);
}

void Bitmap3D__RenderHorizon(Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Bitmap_t* atlas = &state->local->atlas;
  Bitmap_t* screen = &state->local->screen;
  Player_t* player = (Player_t*)state->local->game->curPlyr;
  W = screen->w, H = screen->h;

  Bitmap__Fill(screen, 0, 0, W, H, BLACK);  // wipe

  f32 SPEED = 1.0f / 10;
  f32 s = Math__sin(state->currentTime / 10000);
  f32 c = Math__cos(state->currentTime / 10000);
  f32 xa = 0, ya = 0, za = 0;
  xa = Math__map(s, -1, 1, 0, 1);
  ya = Math__map(c, -1, 1, 0, 1);
  za = Math__map(s, -1, 1, 0, 1);
  Bitmap__DebugText2(
      state,
      4,
      6 * 27,
      LIME,
      0,
      "s %3.3f c %3.3f xa %3.3f ya %3.3f za %3.3f",
      s,
      c,
      xa,
      ya,
      za);

  // Model matrix: Identity at first
  mat4 model = {
      {1, 0, xa, 0},      //
      {0, 1, ya, 0},      //
      {0, 0, 1 + za, 0},  //
      {0, 0, 0, 1},       //
  };
  // View matrix (camera setup)
  mat4 view = {
      {1, 0, 0, 0},  //
      {0, 1, 0, 0},  //
      {0, 0, 1, 0},  //
      {0, 0, 0, 1},  //
  };
  // Projection matrix (Perspective projection)
  mat4 projection;
  float fovy = glms_rad(player->camera.fov);
  float aspect = W / H;
  float nearZ = player->camera.nearZ;
  float farZ = player->camera.farZ;
  glms_perspective(fovy, aspect, nearZ, farZ, projection);

  for (f32 z = -1.0f; z <= 1.0f; z += 0.1f) {
    for (f32 y = -1.0f; y <= 1.0f; y += 0.1f) {
      for (f32 x = -1.0f; x <= -0.91f; x += 0.1f) {
        u32 r = ((u32)((x + 1.0f) * 255.0f) / 2.0f);
        u32 g = ((u32)((y + 1.0f) * 255.0f) / 2.0f);
        u32 b = ((u32)((z + 1.0f) * 255.0f) / 2.0f);
        u32 color = (u32)0xff000000 | b << 16 | g << 8 | r;
        u32 xx = ((u32)(x * (W / 4.0f)) % 8) + 0;
        u32 yy = ((u32)(y * (H / 4.0f)) % 8) + 0;
        // color = Bitmap__Get2DPixel(atlas, xx, yy, PINK);
        Bitmap3D__Set3DPixel(screen, x, y, z, color, model, view, projection);
      }
    }
  }
}

void Bitmap3D__RenderWall(
    Engine__State_t* state, f64 x0, f64 y0, f64 x1, f64 y1, u32 tex, u32 color, f64 tx, f64 ty) {
}

void Bitmap3D__RenderSprite(Engine__State_t* state, f64 x, f64 y, f64 z, u32 tex, u32 color) {
}

void Bitmap3D__PostProcessing(Engine__State_t* state) {
}