#include "Bitmap3D.h"

#include "../game/Logic.h"
#include "Breakpoint.h"
#include "Color.h"
#include "Easing.h"
#include "Engine.h"
#include "GLMShim.h"
#include "List.h"
#include "Log.h"
#include "Math.h"
#include "Wavefront.h"

static f32 W, H;
static u32 BLACK = 0xff000000;
static u32 LIME = 0xff00ff00;
static u32 PINK = 0xffff00ff;
static u32 WHITE = 0xffffffff;

static f32 safe_divide(f32 numerator, f32 denominator) {
  // Check if y is close to zero, then clamp the value of z
  if (fabs(denominator) < FLT_EPSILON) {
    return 0.0f;  // Clamp z when y is too small
  }
  return numerator / denominator;
}

static f32 safe_multiply(f32 a, f32 b) {
  f32 result = a * b;

  // Check for underflow (too small to represent)
  // if (fabs(result) < FLT_MIN) {
  //   return 0.0f;  // Clamp to zero for underflow
  // }

  // Check for overflow
  // if (fabs(result) > FLT_MAX) {
  //   return result > 0 ? FLT_MAX : -FLT_MAX;  // Clamp to max float
  // }

  return result;
}

static void mat4_mulv(mat4 mat, vec4 v, vec4 dest) {
  for (u8 i = 0; i < 4; i++) {
    dest[i] = safe_multiply(mat[i][0], v[0]) +  //
              safe_multiply(mat[i][1], v[1]) +  //
              safe_multiply(mat[i][2], v[2]) +  //
              safe_multiply(mat[i][3], v[3]);
  }
}

static void print_mat4(Engine__State_t* state, u32 row, mat4 m) {
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

static void print_vec4(Engine__State_t* state, u32 row, vec4 v, u32 col) {
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

static void mat4_rotx(Engine__State_t* state, vec4 v, f32 deg, vec4 dest) {
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
  // print_mat4(state, 16, rot1);
  // print_mat4(state, 16 + 4, rot2);
  mat4_mulv(rot2, vc, dest);
}

static void mat4_roty(Engine__State_t* state, vec4 v, f32 deg, vec4 dest) {
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
  mat4_mulv(rot2, vc, dest);
}

static void mat4_rotz(Engine__State_t* state, vec4 v, f32 deg, vec4 dest) {
  f32 s = Math__sin(glms_rad(deg));
  f32 c = Math__cos(glms_rad(deg));
  vec4 vc = (vec4){v[0], v[1], v[2], v[3]};
  mat4 rot1;
  glm_rotate_z((mat4)GLM_MAT4_IDENTITY_INIT, glms_rad(deg), rot1);
  mat4 rot2 = {
      {c, -s, 0, 0},  //
      {s, c, 0, 0},   //
      {0, 0, 1, 0},   //
      {0, 0, 0, 1},   //
  };
  mat4_mulv(rot2, vc, dest);
}

static void mat4_proj(
    Engine__State_t* state, f32 aspect, f32 fovy, f32 nearZ, f32 farZ, mat4 dest) {
  dest[0][1] = 0.0f;
  dest[0][2] = 0.0f;
  dest[0][3] = 0.0f;
  dest[1][0] = 0.0f;
  dest[1][2] = 0.0f;
  dest[1][3] = 0.0f;
  dest[2][0] = 0.0f;
  dest[2][1] = 0.0f;
  dest[3][0] = 0.0f;
  dest[3][1] = 0.0f;
  dest[3][3] = 0.0f;

  // fovy should be given in radians!
  f32 t = tanf(fovy / 2.0f);
  f32 top = nearZ * t;
  f32 right = top * aspect;

  dest[0][0] = nearZ / right;
  dest[1][1] = nearZ / top;

  // produces a right-handed coordinate system (OpenGL-compatible)
  // at 0deg, -Z_FWD, +Y_UP, +X_RIGHT
  dest[2][2] = -(farZ + nearZ) / (farZ - nearZ);
  dest[2][3] = -(2 * farZ * nearZ) / (farZ - nearZ);
  dest[3][2] = -1.0f;
}

// Linear interpolation
f32 lerp(f32 a, f32 b, f32 t) {
  return a + t * (b - a);
}

static bool project(
    Engine__State_t* state, mat4 model, mat4 view, mat4 projection, vec3 v0, vec4 dest) {
  Logic__State_t* logic = state->local;
  Bitmap_t* screen = &state->local->screen;
  Player_t* player = (Player_t*)state->local->game->curPlyr;

  f32 cX = 0, cY = 0, cZ = 0, cRX, cRY;
  cX = player->base.transform.position.x;
  cY = player->base.transform.position.y;
  cZ = player->base.transform.position.z;
  cRX = player->base.transform.rotation.y;
  cRY = player->base.transform.rotation.x;

  // Create a vec4 for the point in model space (homogeneous coordinates)
  vec4 model_point = {v0[0], v0[1], v0[2], 1.0f};
  // print_vec4(state, 1, model_point, WHITE);

  // Transform the point by the model matrix (from model space to world space)
  vec4 world_point;
  mat4_mulv(model, model_point, world_point);
  // print_vec4(state, 2, world_point, WHITE);

  // Transform the point by the view (camera) matrix (from world space to camera space)
  vec4 camera_point;
  mat4_mulv(view, world_point, camera_point);
  // print_vec4(state, 3, camera_point, WHITE);
  mat4_roty(state, camera_point, cRX, camera_point);
  // print_vec4(state, 4, camera_point, WHITE);
  mat4_rotx(state, camera_point, cRY, camera_point);
  // print_vec4(state, 5, camera_point, WHITE);

  // Apply perspective projection (from camera space to clip space)
  vec4 clip_point;
  mat4_mulv(projection, camera_point, clip_point);
  // print_vec4(state, 6, clip_point, WHITE);

  // Perform perspective division (convert homogeneous to normalized device coordinates)
  vec4 ndc;
  if (clip_point[3] != 0.0f) {
    ndc[0] = safe_divide(clip_point[0], clip_point[3]);
    ndc[1] = safe_divide(clip_point[1], clip_point[3]);
    ndc[2] = safe_divide(clip_point[2], clip_point[3]);
    ndc[3] = safe_divide(clip_point[3], clip_point[3]);
  }
  if (ndc[0] < -1.0f || ndc[0] > 1.0f) return false;
  if (ndc[1] < -1.0f || ndc[1] > 1.0f) return false;
  if (ndc[2] < -1.0f || ndc[2] > 1.0f) return false;
  // print_vec4(state, 7, ndc, LIME);

  // Convert normalized device coordinates to screen space
  s32 sx = ((ndc[0] + 1.0f) * 0.5f * W);  // 0..W
  s32 sy = ((1.0f - ndc[1]) * 0.5f * H);  // 0..H
  f32 sz = (ndc[2] + 1.0f) * 0.5f;        // 0..1

  dest[0] = sx;
  dest[1] = sy;
  // dest[2] = MATH_CLAMP(0, sz, 1);
  dest[2] = sz;
  dest[3] = 0;
  // if (sz < FLT_EPSILON) sz = 0;  // clip rounding error

  return true;
}

static void draw_triangle(
    Engine__State_t* state,
    Bitmap_t* screen,
    f32* zbuffer,
    vec3 a,
    vec3 b,
    vec3 c,
    bool upper,
    Bitmap_t* texture,
    u32 tex,
    vec2 uv0,
    vec2 uv1,
    vec2 uv2,
    u32 color) {
  for (f32 y = a[1]; y <= b[1]; y++) {                                         // -n .. +n
    f32 t0 = upper ? (y - a[1]) / (c[1] - a[1]) : (y - c[1]) / (b[1] - c[1]);  // 0 .. 1, diagonal
    f32 t1 = (y - a[1]) / (b[1] - a[1]);                                       // 0 .. 1, vertical
    f32 x0 = upper ? lerp(a[0], c[0], t0) : lerp(c[0], b[0], t0);              // -n .. +n left edge
    f32 x1 = lerp(a[0], b[0], t1);                                           // -n .. +n right edge
    f32 z0 = upper ? lerp(a[2], c[2], t0) : lerp(c[2], b[2], t0);            // -n .. +n left edge
    f32 z1 = lerp(a[2], b[2], t1);                                           // -n .. +n right edge
    f32 uvx0 = upper ? lerp(uv0[0], uv2[0], t0) : lerp(uv2[0], uv1[0], t0);  // -n .. +n left edge
    f32 uvx1 = lerp(uv0[0], uv1[0], t1);                                     // -n .. +n right edge
    f32 uvy0 = upper ? lerp(uv0[1], uv2[1], t0) : lerp(uv2[1], uv1[1], t0);  // -n .. +n left edge
    f32 uvy1 = lerp(uv0[1], uv1[1], t1);                                     // -n .. +n right edge

    if (x0 > x1) {  // -n .. n
      f32 tmpX, tmpZ, tmpUVX, tmpUVY;
      tmpX = x0, x0 = x1, x1 = tmpX;
      tmpZ = z0, z0 = z1, z1 = tmpZ;
      tmpUVX = uvx0, uvx0 = uvx1, uvx1 = tmpUVX;
      // tmpUVY = uvy0, uvy0 = uvy1, uvy1 = tmpUVY;
    }
    for (f32 x = x0; x <= x1; x++) {
      // Horizontal interpolation factor
      f32 t = (x - x0) / (x1 - x0);
      // Interpolated Z value for this pixel
      // TODO: fix the rounding error and range here
      f32 z = 1000.0f * lerp(z0, z1, t);

      // Check if the pixel's depth is closer than what's stored in the Z-buffer
      u32 i = y * W + x;
      if (z > zbuffer[i]) {
        // Update Z-buffer with the new depth value
        zbuffer[i] = z;
        // Draw the Z-buffer (for debugging)
        u32 cmp = Math__map(z, 0, 1, 128, 255);
        // Bitmap__Set2DPixel(screen, x, y, 0xff000000 | cmp << 16 | cmp << 8 | cmp);

        // Get the texel color
        f32 tx = lerp(uvx0, uvx1, t) * 8 * 2;
        f32 ty = lerp(uvy0, uvy1, t1) * 8 * 2;
        color = Bitmap__Get2DTiledPixel(texture, tx, ty, 8, tex, 0, PINK);

        // Draw the pixel in the RGBA buffer
        Bitmap__Set2DPixel(screen, x, y, color);

        // print_vec4(state, 23, (vec4){x0, x1, a[1], b[1]}, WHITE);
        // print_vec4(state, 23, (vec4){a[2], b[2], c[2], 0}, WHITE);
        // print_vec4(state, 24, (vec4){z0, z1, 0, 0}, WHITE);
        // print_vec4(state, 25, (vec4){x, y, z, 0}, WHITE);
      }
    }
  }
}

void Bitmap3D__RenderHorizon(Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Bitmap__Fill(&logic->screen, 0, 0, W, H, BLACK);  // wipe

  for (u32 i = 0; i < W * H; i++) {
    logic->zbuf[i] = FLT_MIN;
  }
}

void Bitmap3D__RenderWall(Engine__State_t* state, f32 x0, f32 y0, f32 z0, u32 tex, u32 color) {
  Logic__State_t* logic = state->local;
  Bitmap_t* atlas = &state->local->atlas;
  Bitmap_t* screen = &state->local->screen;
  Player_t* player = (Player_t*)state->local->game->curPlyr;
  W = screen->w, H = screen->h;
  f32 cX = 0, cY = 0, cZ = 0, cRX = 0, cRY = 0;
  cX = player->base.transform.position.x;
  cY = player->base.transform.position.y;
  cZ = player->base.transform.position.z;
  cRX = player->base.transform.rotation.y;
  cRY = player->base.transform.rotation.x;

  // Model matrix
  mat4 model = {
      {1.0f, 0, 0, x0},  //
      {0, 1.0f, 0, y0},  //
      {0, 0, 1.0f, z0},  //
      {0, 0, 0, 1.0f},   //
  };

  // View matrix (camera)
  mat4 view = {
      {1, 0, 0, -cX},  //
      {0, 1, 0, -cY},  //
      {0, 0, 1, -cZ},  //
      {0, 0, 0, 1},    //
  };

  // Projection matrix (Perspective projection)
  mat4 projection;
  float fovy = glms_rad(90.0f);
  float aspect = W / H;
  float nearZ = 1000.0f;
  float farZ = 0.01f;
  mat4_proj(state, aspect, fovy, nearZ, farZ, projection);

  // --- POINT INTERPOLATION ----
  Wavefront_t* obj = List__get(logic->game->meshes, MODEL_BOX);

  // DEBUG: wipe screen so only last block is visible
  // Bitmap__Fill(&logic->screen, 0, 0, W, H, BLACK);  // wipe

  // render_mesh()
  // Project and draw all triangles that form the faces
  List__Node_t* c = obj->faces->head;
  for (s32 i = 0; i < obj->faces->len; i++) {
    Wavefront__Face_t* f = c->data;
    vec3* v00 = List__get(obj->vertices, f->vertex_idx[0] - 1);
    vec3* v01 = List__get(obj->vertices, f->vertex_idx[1] - 1);
    vec3* v02 = List__get(obj->vertices, f->vertex_idx[2] - 1);
    vec2* uv0 = List__get(obj->texcoords, f->vertex_idx[0] - 1);
    vec2* uv1 = List__get(obj->texcoords, f->vertex_idx[1] - 1);
    vec2* uv2 = List__get(obj->texcoords, f->vertex_idx[2] - 1);
    c = c->next;

    // Project the 3D vertices to 2D screen space
    vec4 v0, v1, v2;
    if (!project(state, model, view, projection, *v00, v0)) continue;
    if (!project(state, model, view, projection, *v01, v1)) continue;
    if (!project(state, model, view, projection, *v02, v2)) continue;

    // Perform basic clipping on each vertex
    if (v0[0] < 0) v0[0] = 0;
    if (v0[0] >= W) v0[0] = W - 1;
    if (v0[1] < 0) v0[1] = 0;
    if (v0[1] >= H) v0[1] = H - 1;
    // TODO: clip on Z 0..1?

    if (v1[0] < 0) v1[0] = 0;
    if (v1[0] >= W) v1[0] = W - 1;
    if (v1[1] < 0) v1[1] = 0;
    if (v1[1] >= H) v1[1] = H - 1;

    if (v2[0] < 0) v2[0] = 0;
    if (v2[0] >= W) v2[0] = W - 1;
    if (v2[1] < 0) v2[1] = 0;
    if (v2[1] >= H) v2[1] = H - 1;

    // Color for this face
    // u8 r = Math__map(Math__triangleWave(v0[1], 10), -1, 1, 128, 255);
    // u8 g = Math__map(Math__triangleWave(v1[1], 10), -1, 1, 128, 255);
    // u8 b = Math__map(Math__triangleWave(v2[1], 10), -1, 1, 128, 255);
    u8 r = Math__map(Math__triangleWave(i, obj->faces->len), -1, 1, 128, 255);
    u8 g = Math__map(Math__triangleWave(i + 1, obj->faces->len), -1, 1, 128, 255);
    u8 b = Math__map(Math__triangleWave(i + 2, obj->faces->len), -1, 1, 128, 255);
    u32 color = 0xff000000 | b << 16 | g << 8 | r;

    // Draw the 2 triangles for this square face

    // Sort vertices by y-coordinate (v0[1] <= v1[1] <= v2[1])
    // such that v0 becomes a reference to the point furthest into the negative Y plane
    if (v0[1] > v1[1]) {
      vec3 temp = (vec3){v0[0], v0[1], v0[2]};
      v0[0] = v1[0], v0[1] = v1[1], v0[2] = v1[2];
      v1[0] = temp[0], v1[1] = temp[1], v1[2] = temp[2];
    }
    if (v1[1] > v2[1]) {
      vec3 temp = (vec3){v1[0], v1[1], v1[2]};
      v1[0] = v2[0], v1[1] = v2[1], v1[2] = v2[2];
      v2[0] = temp[0], v2[1] = temp[1], v2[2] = temp[2];
    }
    if (v0[1] > v1[1]) {
      vec3 temp = (vec3){v0[0], v0[1], v0[2]};
      v0[0] = v1[0], v0[1] = v1[1], v0[2] = v1[2];
      v1[0] = temp[0], v1[1] = temp[1], v1[2] = temp[2];
    }

    // Bitmap__DebugText2(state, 4, 6 * 3, WHITE, BLACK, "face %u", i);

    // the concept of upper and lower here is nuanced
    // 0 .. 1 .. 2 are divided along y-axis into min ... mid ... max
    // this allows the scanline to loop over y,x, in two rectangular passes
    // it means we can draw any triangle (not only right triangles, or 90deg aligned rotations)
    draw_triangle(
        state,
        screen,
        logic->zbuf,
        v0,
        v1,
        v2,
        true,
        &logic->atlas,
        tex,
        *uv0,
        *uv1,
        *uv2,
        color);
    draw_triangle(
        state,
        screen,
        logic->zbuf,
        v1,
        v2,
        v0,
        false,
        &logic->atlas,
        tex,

        *uv0,
        *uv1,
        *uv2,
        color);
  }

  return;
}

void Bitmap3D__RenderSprite(Engine__State_t* state, f64 x, f64 y, f64 z, u32 tex, u32 color) {
}

void Bitmap3D__PostProcessing(Engine__State_t* state) {
}