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
static u32 WHITE = 0xffffffff;

void print_mat4(Engine__State_t* state, u32 row, mat4 m) {
  Bitmap__DebugText2(
      state,
      4,
      6 * (row + 0),
      WHITE,
      BLACK,
      "m %+5.2f %+5.2f %+5.2f %+5.2f",
      m[0][0],
      m[0][1],
      m[0][2],
      m[0][3]);
  Bitmap__DebugText2(
      state,
      4,
      6 * (row + 1),
      WHITE,
      BLACK,
      "  %+5.2f %+5.2f %+5.2f %+5.2f",
      m[1][0],
      m[1][1],
      m[1][2],
      m[1][3]);
  Bitmap__DebugText2(
      state,
      4,
      6 * (row + 2),
      WHITE,
      BLACK,
      "  %+5.2f %+5.2f %+5.2f %+5.2f",
      m[2][0],
      m[2][1],
      m[2][2],
      m[2][3]);
  Bitmap__DebugText2(
      state,
      4,
      6 * (row + 3),
      WHITE,
      BLACK,
      "  %+5.2f %+5.2f %+5.2f %+5.2f",
      m[3][0],
      m[3][1],
      m[3][2],
      m[3][3]);
}

void print_vec4(Engine__State_t* state, u32 row, vec4 v, u32 col) {
  Bitmap__DebugText2(
      state,
      4,
      6 * row,
      col,
      BLACK,
      "v %+5.2f %+5.2f %+5.2f %+5.2f",
      v[0],
      v[1],
      v[2],
      v[3]);
}

void mat4_rotx(Engine__State_t* state, vec4 v, f32 deg, vec4 dest) {
  f32 s = Math__sin(glms_rad(deg));
  f32 c = Math__cos(glms_rad(deg));
  vec4 vc = (vec4){v[0], v[1], v[2], v[3]};
  mat4 rot1;
  glm_rotate_x((mat4)GLM_MAT4_IDENTITY_INIT, glms_rad(deg), rot1);
  mat4 rot2 = {
      {1, 0, 0, 0},   //
      {0, c, s, 0},   //
      {0, -s, c, 0},  //
      {0, 0, 0, 1},   //
  };
  print_mat4(state, 16, rot1);
  print_mat4(state, 16 + 4, rot2);
  glms_mat4_mulv(rot2, vc, dest);
}

void mat4_roty(Engine__State_t* state, vec4 v, f32 deg, vec4 dest) {
  f32 s = Math__sin(glms_rad(deg));
  f32 c = Math__cos(glms_rad(deg));
  vec4 vc = (vec4){v[0], v[1], v[2], v[3]};
  mat4 rot1;
  glm_rotate_y((mat4)GLM_MAT4_IDENTITY_INIT, glms_rad(deg), rot1);
  mat4 rot2 = {
      {c, 0, -s, 0},  //
      {0, 1, 0, 0},   //
      {s, 0, c, 0},   //
      {0, 0, 0, 1},   //
  };
  // print_mat4(state, 16, rot1);
  // print_mat4(state, 16 + 4, rot2);
  glms_mat4_mulv(rot2, vc, dest);
}

void mat4_rotz(Engine__State_t* state, vec4 v, f32 deg, vec4 dest) {
  f32 s = Math__sin(glms_rad(deg));
  f32 c = Math__cos(glms_rad(deg));
  vec4 vc = (vec4){v[0], v[1], v[2], v[3]};
  mat4 rot1 = {
      {c, -s, 0, 0},  //
      {s, c, 0, 0},   //
      {0, 0, 1, 0},   //
      {0, 0, 0, 1},   //
  };
  glms_mat4_mulv(rot1, vc, dest);
}

void Bitmap3D__RenderHorizon(Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Bitmap_t* atlas = &state->local->atlas;
  Bitmap_t* screen = &state->local->screen;
  Player_t* player = (Player_t*)state->local->game->curPlyr;
  W = screen->w, H = screen->h;

  Bitmap__Fill(screen, 0, 0, W, H, BLACK);  // wipe

  // Model matrix: Identity at first
  f32 s0 = Math__triangleWave(state->currentTime, 1000 * 10);
  f32 sz0 = 1.0;
  // sz0 = Math__map(s0, -1, 1, 0, 2);
  mat4 model = {
      {sz0, 0, 0, 0},  //
      {0, sz0, 0, 0},  //
      {0, 0, sz0, 0},  //
      {0, 0, 0, 1},    //
  };
  // print_mat4(state, 16, model);

  // View matrix (camera setup)
  // f32 s1 = Math__triangleWave(state->currentTime, 1000 * 10);
  f32 s1 = Math__sin(state->currentTime / (1000 * 7));
  f32 c1 = Math__cos(state->currentTime / (1000 * 3));
  f32 c2 = Math__cos(state->currentTime / (1000 * 1));
  f32 xa = 0, ya = 0, za = 5;
  // xa = -1, ya = -1, za = -1;
  xa = Math__map(s1, -1, 1, -1, 1);
  ya = Math__map(c1, -1, 1, -1, 1);
  za = Math__map(c2, -1, 1, -10, 0);
  f32 s2 = Math__triangleWave(state->currentTime, 1000 * 10);
  f32 sz1 = 1.0;
  // sz1 = Math__map(s2, -1, 1, 0, 1);
  mat4 view = {
      {sz1, 0, 0, xa},  //
      {0, sz1, 0, ya},  //
      {0, 0, sz1, za},  //
      {0, 0, 0, 1},     //
  };
  // print_mat4(state, 16 + 4, view);

  // Projection matrix (Perspective projection)
  mat4 projection;
  float fovy = glms_rad(90.0f);
  float aspect = W / H;
  float nearZ = 1.0f;
  float farZ = 10.0f;
  glm_perspective(fovy, aspect, nearZ, farZ, projection);
  // print_mat4(state, 16 + 8, projection);

  // +x/right +y/up -z/fwd
  f32 step = 2.0f;
  step = 0.5f;
  for (f32 z = -1.0f; z <= 1.0f; z += step) {
    for (f32 y = -1.0f; y <= 1.0f; y += step) {
      for (f32 x = -1.0f; x <= 1.0f; x += step) {
        u32 r = Math__map(x, -1, 1, 128, 255);
        u32 g = Math__map(y, -1, 1, 128, 255);
        u32 b = Math__map(z, -1, 1, 128, 255);
        u32 color = (u32)0xff000000 | b << 16 | g << 8 | r;

        // Create a vec4 for the point in model space (homogeneous coordinates)
        vec4 model_point = {x, y, z, 1.0f};

        f32 deg = Math__map(Math__sin(state->currentTime / (1000 * 1)), -1, 1, 0, 180);
        mat4_rotx(state, model_point, deg, model_point);
        mat4_roty(state, model_point, deg, model_point);

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
        vec4 ndc;
        if (clip_point[3] != 0.0f) {
          ndc[0] = clip_point[0] / clip_point[3];
          ndc[1] = clip_point[1] /= clip_point[3];
          ndc[2] = clip_point[2] /= clip_point[3];
        }
        if (ndc[0] < -1.0f || ndc[0] > 1.0f) continue;
        if (ndc[1] < -1.0f || ndc[1] > 1.0f) continue;
        if (ndc[2] < -1.0f || ndc[2] > 1.0f) continue;
        if (z == -1.0f && x == 1.0f && y == -1.0f) {
          u32 row = Math__map(r, 128, 255, 0, 19);
          print_vec4(state, row, ndc, color);
        }

        // Convert normalized device coordinates to screen space
        s32 sx = (s32)((ndc[0] + 1.0f) * 0.5f * W);
        s32 sy = (s32)((ndc[1] + 1.0f) * 0.5f * H);

        // Draw the pixel in the RGBA buffer
        Bitmap__Set2DPixel(screen, sx, sy, color);
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