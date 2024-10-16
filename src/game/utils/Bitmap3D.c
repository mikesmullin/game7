#include "Bitmap3D.h"

#include <float.h>
#include <math.h>

#include "../../lib/Engine.h"
#include "../../lib/List.h"
#include "../../lib/Log.h"
#include "../../lib/Math.h"
#include "../Logic.h"
#include "../entities/Player.h"
#include "Color.h"
#include "Easing.h"
#include "GLMShim.h"
#include "Wavefront.h"

static f32 W, H;
static mat4 model = {
    {1, 0, 0, 0},  //
    {0, 1, 0, 0},  //
    {0, 0, 1, 0},  //
    {0, 0, 0, 1},  //
};
static mat4 view = {
    {1, 0, 0, 0},  //
    {0, 1, 0, 0},  //
    {0, 0, 1, 0},  //
    {0, 0, 0, 1},  //
};
static mat4 projection;

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

static void vec3_clamp(vec3 a, f32 w, f32 h) {
  // a[0] = MATH_CLAMP(0, a[0], w - 1);
  // a[1] = MATH_CLAMP(0, a[1], h - 1);
  f32 hw = w / 2.0f;
  f32 hh = h / 2.0f;
  a[0] = MATH_CLAMP(-hw, a[0], w + hw);
  a[1] = MATH_CLAMP(-hh, a[1], h + hh);
}

static void vec3_cp(vec3 a, vec3 dst) {
  dst[0] = a[0];
  dst[1] = a[1];
  dst[2] = a[2];
}

static void vec2_cp(vec2 a, vec2 dst) {
  dst[0] = a[0];
  dst[1] = a[1];
}

static void vec3_swap(vec3 a, vec3 b) {
  vec3 t;
  t[0] = a[0], t[1] = a[1], t[2] = a[2];
  a[0] = b[0], a[1] = b[1], a[2] = b[2];
  b[0] = t[0], b[1] = t[1], b[2] = t[2];
}

static void vec2_swap(vec2 a, vec2 b) {
  vec2 t;
  t[0] = a[0], t[1] = a[1];
  a[0] = b[0], a[1] = b[1];
  b[0] = t[0], b[1] = t[1];
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

static void print_vec4(Engine__State_t* state, u32 row, char* lbl, vec4 v, u32 col) {
  Bitmap__DebugText2(
      state,
      4,
      6 * row,
      col,
      BLACK,
      "%s %+7.2f %+7.2f %+7.2f %+7.2f",
      lbl,
      v[0],
      v[1],
      v[2],
      v[3]);
}

static void mat4_rotx(Engine__State_t* state, vec4 v, f32 deg, vec4 dest) {
  f32 s = Math__sin(glms_rad(deg));
  f32 c = Math__cos(glms_rad(deg));
  vec4 vc = (vec4){v[0], v[1], v[2], v[3]};
  // mat4 rot1;
  // glm_rotate_x((mat4)GLM_MAT4_IDENTITY_INIT, glms_rad(deg), rot1);
  mat4 rot2 = {
      {1, 0, 0, 0},  //
      {0, c, s, 0},  //
      {0, -s, c, 0},  //
      {0, 0, 0, 1},  //
  };
  // print_mat4(state, 16, rot1);
  // print_mat4(state, 16 + 4, rot2);
  mat4_mulv(rot2, vc, dest);
}

static void mat4_roty(Engine__State_t* state, vec4 v, f32 deg, vec4 dest) {
  f32 s = Math__sin(glms_rad(deg));
  f32 c = Math__cos(glms_rad(deg));
  vec4 vc = (vec4){v[0], v[1], v[2], v[3]};
  // mat4 rot1;
  // glm_rotate_y((mat4)GLM_MAT4_IDENTITY_INIT, glms_rad(deg), rot1);
  mat4 rot2 = {
      {c, 0, -s, 0},  //
      {0, 1, 0, 0},  //
      {s, 0, c, 0},  //
      {0, 0, 0, 1},  //
  };
  // print_mat4(state, 16, rot1);
  // print_mat4(state, 16 + 4, rot2);
  mat4_mulv(rot2, vc, dest);
}

static void mat4_rotz(Engine__State_t* state, vec4 v, f32 deg, vec4 dest) {
  f32 s = Math__sin(glms_rad(deg));
  f32 c = Math__cos(glms_rad(deg));
  vec4 vc = (vec4){v[0], v[1], v[2], v[3]};
  // mat4 rot1;
  // glm_rotate_z((mat4)GLM_MAT4_IDENTITY_INIT, glms_rad(deg), rot1);
  mat4 rot2 = {
      {c, -s, 0, 0},  //
      {s, c, 0, 0},  //
      {0, 0, 1, 0},  //
      {0, 0, 0, 1},  //
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

f32 f32_zbuf_u8(f32 z) {
  // convert z from 0.0001..0 to 0..255
  // with a kind of ramp/cutoff close to znear
  return Math__map(MATH_CLAMP(0, z * 100, 1), 0, 1, 0, 255);
}

static void project(
    Engine__State_t* state,
    bool debug,
    mat4 model,
    mat4 view,
    mat4 projection,
    vec3 v0,
    vec4 ndc,
    vec4 dest) {
  Logic__State_t* logic = state->local;
  Bitmap_t* screen = &state->local->screen;
  Player_t* player = (Player_t*)state->local->game->curPlyr;

  f32 cX = 0, cY = 0, cZ = 0, cRX, cRY;
  cX = player->base.tform->pos.x;
  cY = player->base.tform->pos.y;
  cZ = player->base.tform->pos.z;
  cRX = player->base.tform->rot.y;
  cRY = player->base.tform->rot.x;

  // Create a vec4 for the point in model space (homogeneous coordinates)
  vec4 model_point = {v0[0], v0[1], v0[2], 1.0f};
  if (debug) print_vec4(state, 1, "m", model_point, WHITE);

  // Transform the point by the model matrix (from model space to world space)
  vec4 world_point;
  mat4_mulv(model, model_point, world_point);
  if (debug) print_vec4(state, 2, "w", world_point, WHITE);

  // Transform the point by the view (camera) matrix (from world space to camera space)
  vec4 camera_point;
  mat4_mulv(view, world_point, camera_point);
  if (debug) print_vec4(state, 3, "c", camera_point, WHITE);
  mat4_roty(state, camera_point, cRX, camera_point);
  if (debug) print_vec4(state, 4, "cRY", camera_point, WHITE);
  mat4_rotx(state, camera_point, cRY, camera_point);
  if (debug) print_vec4(state, 5, "cRX", camera_point, WHITE);

  // Apply perspective projection (from camera space to clip space)
  vec4 clip_point;
  mat4_mulv(projection, camera_point, clip_point);
  if (debug) print_vec4(state, 6, "clip", clip_point, WHITE);

  // Perform perspective division (convert homogeneous to normalized device coordinates)
  // vec4 ndc;
  if (clip_point[3] != 0.0f) {
    ndc[0] = safe_divide(clip_point[0], clip_point[3]);
    ndc[1] = safe_divide(clip_point[1], clip_point[3]);
    ndc[2] = safe_divide(clip_point[2], clip_point[3]);
    ndc[3] = safe_divide(clip_point[3], clip_point[3]);
  }
  // bool r = true;
  // if (ndc[0] < -1.0f || ndc[0] > 1.0f) r = false;
  // if (ndc[1] < -1.0f || ndc[1] > 1.0f) r = false;
  // if (ndc[2] < -1.0f || ndc[2] > 0) r = false;
  if (debug) print_vec4(state, 7, "ndc", ndc, WHITE);

  // Convert normalized device coordinates to screen space
  s32 sx = ((ndc[0] + 1.0f) * 0.5f * W);  // 0..W
  s32 sy = ((1.0f - ndc[1]) * 0.5f * H);  // 0..H
  f32 sz = (ndc[2] + 1.0f) * 0.5f;  // 0..1

  dest[0] = sx;
  dest[1] = sy;
  // dest[2] = MATH_CLAMP(0, sz, 1);
  dest[2] = sz;
  dest[3] = 0;
  // if (sz < FLT_EPSILON) sz = 0;  // clip rounding error
  if (debug) print_vec4(state, 8, "d", dest, LIME);

  // return r;
}

// Function to compute the area of a triangle using its vertex positions
static f32 triangle_area(vec2 p1, vec2 p2, vec2 p3) {
  return 0.5f * ((p1[0] * (p2[1] - p3[1])) + (p2[0] * (p3[1] - p1[1])) + (p3[0] * (p1[1] - p2[1])));
}

static void barycentric_tri_lerp(
    vec2 xy, vec2 a, vec2 b, vec2 c, vec2 uva, vec2 uvb, vec2 uvc, vec2 dest) {
  // Step 1: Calculate the total area of triangle ABC
  f32 area_abc = triangle_area(a, b, c);

  // Step 2: Calculate the sub-area of triangle PBC
  f32 area_pbc = triangle_area(xy, b, c);

  // Step 3: Calculate the sub-area of triangle PCA
  f32 area_pca = triangle_area(xy, c, a);

  // Step 4: Calculate the sub-area of triangle PAB
  f32 area_pab = triangle_area(xy, a, b);

  // Step 5: Calculate the barycentric weights (lambdas)
  f32 lambda_a = area_pbc / area_abc;
  f32 lambda_b = area_pca / area_abc;
  f32 lambda_c = area_pab / area_abc;

  // Step 6: Interpolate the UV coordinates using the barycentric weights
  dest[0] = lambda_a * uva[0] + lambda_b * uvb[0] + lambda_c * uvc[0];  // interpolated U
  dest[1] = lambda_a * uva[1] + lambda_b * uvb[1] + lambda_c * uvc[1];  // interpolated V
}

static f32 calculate_signed_area(vec2 p1, vec2 p2, vec2 p3) {
  // signed area of a triangle (>0 = CCW, <0 = CW)
  return  //
      (p1[0] * (p2[1] - p3[1]) +  //
       p2[0] * (p3[1] - p1[1]) +  //
       p3[0] * (p1[1] - p2[1])) /
      2.0f;
}

static void draw_triangle(
    Engine__State_t* state,
    Bitmap_t* screen,
    f32* zbuffer,
    bool debug,
    vec3 a,
    vec3 b,
    vec3 c,
    vec4 ndcA,
    vec4 ndcB,
    vec4 ndcC,
    bool upper,
    Bitmap_t* texture,
    u32 tx0,
    u32 ty0,
    vec2 uv0,
    vec2 uv1,
    vec2 uv2,
    bool useMask,
    u32 mask,
    u32 color) {
  // copy to avoid input mutation
  vec3 a0, b0, c0;
  vec3_cp(a, a0);
  vec3_cp(b, b0);
  vec3_cp(c, c0);
  vec2 uv00, uv01, uv02;
  vec2_cp(uv0, uv00);
  vec2_cp(uv1, uv01);
  vec2_cp(uv2, uv02);

  // TODO: remove this when geometry clipping is implemented
  // DEBUG: perform backup clipping on each vertex
  vec3_clamp(a0, W, H);
  vec3_clamp(b0, W, H);
  vec3_clamp(c0, W, H);

  if (
      // if all 3 vertices X coord is outside -1..1
      // then triangle is fully outside X (L/R) plane
      (  //
          (ndcA[0] < -ndcA[3] || ndcA[0] > ndcA[3]) &&  //
          (ndcB[0] < -ndcB[3] || ndcB[0] > ndcB[3]) &&  //
          (ndcC[0] < -ndcC[3] || ndcC[0] > ndcC[3])  //
          ) ||
      // if all 3 vertices Y coord is outside -1..1
      // then triangle is fully outside Y (T/B) plane
      (  //
          (ndcA[1] < -ndcA[3] || ndcA[1] > ndcA[3]) &&  //
          (ndcB[1] < -ndcB[3] || ndcB[1] > ndcB[3]) &&  //
          (ndcC[1] < -ndcC[3] || ndcC[1] > ndcC[3])  //
          ) ||
      // if all 3 vertices Z coord is outside -1..0
      // then triangle is fully behind camera or outside znear/zfar plane
      (  //
          (ndcA[2] < -ndcA[3] || ndcA[2] >= 0) &&  //
          (ndcB[2] < -ndcB[3] || ndcB[2] >= 0) &&  //
          (ndcC[2] < -ndcC[3] || ndcC[2] >= 0)  //
          )  //
  ) {
    // only render triangles that are at least partially inside frustum
    return;
  }

  if (calculate_signed_area(a0, b0, c0) > 0) {  // CCW
    // discard (not a front-facing face, relative to camera view)
    return;
  }

  // Sort vertices by y-coordinate (a[1] <= b[1] <= c[1])
  // such that a becomes a reference to the point furthest into the negative Y plane
  if (a0[1] > b0[1]) {
    vec3_swap(a0, b0);
    vec2_swap(uv00, uv01);
  }
  if (b0[1] > c0[1]) {
    vec3_swap(b0, c0);
    vec2_swap(uv01, uv02);
  }
  if (a0[1] > b0[1]) {
    vec3_swap(a0, b0);
    vec2_swap(uv00, uv01);
  }

  upper = true;
  for (f32 y = a0[1]; y <= b0[1]; y++) {  // -n .. +n
    f32 t0 = (y - a0[1]) / (c0[1] - a0[1]);  // 0 .. 1, ya to yc normalized
    f32 t1 = (y - a0[1]) / (b0[1] - a0[1]);  // 0 .. 1, ya to yb normalized
    if (isinf(t0) || isnan(t0)) t0 = 0;
    if (isinf(t1) || isnan(t1)) t1 = 0;
    f32 x0 = lerp(a0[0], c0[0], t0);  // -n .. +n, xa to xc by yc%
    f32 x1 = lerp(a0[0], b0[0], t1);  // -n .. +n, xa to xb by yb%
    if (x0 > x1) {  // -n .. n
      f32 tmpX;
      tmpX = x0, x0 = x1, x1 = tmpX;
    }

    for (f32 x = x0; x <= x1; x++) {
      if (x < 0 || W < x || y < 0 || H < y) continue;
      // Horizontal interpolation factor
      f32 t = (x - x0) / (x1 - x0);  // 0..1

      // Interpolated Z value for this pixel
      f32 z;
      vec2 zz;
      barycentric_tri_lerp(
          (vec2){x, y},  //
          a0,
          b0,
          c0,  //
          (vec2){a0[2], 0},
          (vec2){b0[2], 0},
          (vec2){c0[2], 0},
          zz);
      z = zz[0];

      // Check if the pixel's depth is closer than what's stored in the Z-buffer
      u32 i = y * W + x;
      if (z > zbuffer[i]) {
        // Get the texel color
        vec2 ttt;
        barycentric_tri_lerp(
            (vec2){x, y},  //
            a0,
            b0,
            c0,  //
            uv00,
            uv01,
            uv02,
            ttt);
        u32 tc = Bitmap__Get2DTiledPixel(texture, ttt[0] * 8, (1 - ttt[1]) * 8, 8, tx0, ty0, PINK);

        if (!useMask || tc != mask) {  // bit mask for transparency
          // Update Z-buffer with the new depth value
          zbuffer[i] = z;
          // Draw the Z-buffer (for debugging)
          // u32 zz = f32_zbuf_u8(z);
          // color = 0xff000000 | zz << 16 | zz << 8 | zz;

          // Draw the pixel in the RGBA buffer
          Bitmap__Set2DPixel(screen, x, y, alpha_blend(tc, color));
        }
      }
    }
  }

  // the concept of upper and lower here is nuanced
  // 0 .. 1 .. 2 are divided along y-axis into min ... mid ... max
  // this allows the scanline to loop over y,x, in two rectangular passes
  // it means we can draw any triangle (not only right triangles, or 90deg aligned rotations)

  // this is drawing below the y-middle point (opposite hypotenuse)
  // t0 is the same because the a-c side is the hypotenuse (straight line)
  // but t1 differs because the b-c side is at a different angle/slope than a-b
  upper = false;
  // tmp=a; a=b, b=c, c=tmp(a)
  for (f32 y = b0[1]; y <= c0[1]; y++) {  // -n .. +n
    f32 t0 = (y - a0[1]) / (c0[1] - a0[1]);  // 0 .. 1, diagonal
    f32 t1 = (y - b0[1]) / (c0[1] - b0[1]);  // 0 .. 1, vertical
    if (isinf(t0) || isnan(t0)) t0 = 0;
    if (isinf(t1) || isnan(t1)) t1 = 0;
    f32 x0 = lerp(a0[0], c0[0], t0);  // -n .. +n left edge
    f32 x1 = lerp(b0[0], c0[0], t1);  // -n .. +n right edge
    if (x0 > x1) {  // -n .. n
      f32 tmpX;
      tmpX = x0, x0 = x1, x1 = tmpX;
    }

    for (f32 x = x0; x <= x1; x++) {
      if (x < 0 || W < x || y < 0 || H < y) continue;
      // Horizontal interpolation factor
      f32 t = (x - x0) / (x1 - x0);  // 0..1

      // Interpolated Z value for this pixel
      f32 z;
      vec2 zz;
      barycentric_tri_lerp(
          (vec2){x, y},  //
          a0,
          b0,
          c0,  //
          (vec2){a0[2], 0},
          (vec2){b0[2], 0},
          (vec2){c0[2], 0},
          zz);
      z = zz[0];

      // Check if the pixel's depth is closer than what's stored in the Z-buffer
      u32 i = y * W + x;
      if (z > zbuffer[i]) {
        // Get the texel color
        vec2 ttt;
        barycentric_tri_lerp(
            (vec2){x, y},  //
            a0,
            b0,
            c0,  //
            uv00,
            uv01,
            uv02,
            ttt);
        u32 tc = Bitmap__Get2DTiledPixel(texture, ttt[0] * 8, (1 - ttt[1]) * 8, 8, tx0, ty0, PINK);

        if (!useMask || tc != mask) {  // bit mask for transparency
          // Update Z-buffer with the new depth value
          zbuffer[i] = z;
          // Draw the Z-buffer (for debugging)
          // u32 zz = f32_zbuf_u8(z);
          // color = 0xff000000 | zz << 16 | zz << 8 | zz;

          // Draw the pixel in the RGBA buffer
          Bitmap__Set2DPixel(screen, x, y, alpha_blend(tc, color));
        }
      }
    }
  }

  // if (debug) {
  //   f32 s = Math__sin(state->currentTime / 100);
  //   u32 cmp = Math__map(s, -1, 1, 128, 255);
  //   Bitmap__Set2DPixel(screen, a0[0], a0[1], 0xff000000 | cmp);
  //   Bitmap__DebugText2(
  //       state,
  //       a0[0] + 2,
  //       a0[1] - 3,
  //       0xff0000ff,
  //       0x00000000,
  //       "%+05.2f",
  //       f32_zbuf_u8(a0[2]));
  //   Bitmap__Set2DPixel(screen, b0[0], b0[1], 0xff000000 | cmp << 8);
  //   Bitmap__DebugText2(
  //       state,
  //       b0[0] + 2,
  //       b0[1] - 3,
  //       LIME,
  //       0x00000000,
  //       "%+05.2f",
  //       f32_zbuf_u8(b0[2]));
  //   Bitmap__Set2DPixel(screen, c0[0], c0[1], 0xff000000 | cmp << 16);
  //   Bitmap__DebugText2(
  //       state,
  //       c0[0] + 2,
  //       c0[1] - 3,
  //       BLUE,
  //       0x00000000,
  //       "%+05.2f",
  //       f32_zbuf_u8(c0[2]));
  // }
}

static void draw_line_segment(
    Engine__State_t* state, Bitmap_t* screen, f32 x0, f32 y0, f32 x1, f32 y1, u32 color) {
  // Bresenham's Line Algorithm
  s32 dx = fabsf(x1 - x0);
  s32 dy = fabsf(y1 - y0);
  s32 sx = (x0 < x1) ? 1 : -1;
  s32 sy = (y0 < y1) ? 1 : -1;

  s32 err = dx - dy;
  s32 e2;
  s32 x = x0, y = y0;
  while (1) {
    Bitmap__Set2DPixel(screen, x, y, color);
    if (x == x1 && y == y1) break;

    e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x += sx;
    }
    if (e2 < dx) {
      err += dx;
      y += sy;
    }
  }
}

void Bitmap3D__RenderHorizon(Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Bitmap_t* atlas = &state->local->atlas;
  Bitmap_t* screen = &state->local->screen;
  Player_t* player = (Player_t*)state->local->game->curPlyr;
  W = screen->w, H = screen->h;
  f32 cX = 0, cY = 0, cZ = 0, cRX = 0, cRY = 0;
  cX = player->base.tform->pos.x;
  cY = player->base.tform->pos.y;
  cZ = player->base.tform->pos.z;
  cRX = player->base.tform->rot.y;
  cRY = player->base.tform->rot.x;

  if (0 == cY) {  // grounded
    cY = Math__sin(player->bobPhase) * PLAYER_BOB;
  }

  // View matrix (camera)
  view[0][3] = -cX;
  view[1][3] = -cY;
  view[2][3] = -cZ;

  // Projection matrix (Perspective projection)
  float fovy = glms_rad(60.0f);
  float aspect = W / H;
  float nearZ = 1000.0f;
  float farZ = 0.01f;
  mat4_proj(state, aspect, fovy, nearZ, farZ, projection);

  Bitmap__Fill(&logic->screen, 0, 0, W, H, BLACK);  // wipe

  for (u32 i = 0; i < W * H; i++) {
    logic->zbuf[i] = FLT_MIN;  // wipe zbuf
  }
}

void Bitmap3D__RenderWall(
    Engine__State_t* state, f32 x0, f32 y0, f32 z0, u32 tx0, u32 ty0, u32 color) {
  Logic__State_t* logic = state->local;
  Bitmap_t* atlas = &state->local->atlas;
  Bitmap_t* screen = &state->local->screen;
  Player_t* player = (Player_t*)state->local->game->curPlyr;

  // Model matrix
  model[0][3] = x0;
  model[1][3] = y0;
  model[2][3] = z0;

  // --- POINT INTERPOLATION ----
  Wavefront_t* obj = List__get(logic->game->meshes, MODEL_BOX);

  // render_mesh()
  // Project and draw all triangles that form the faces
  List__Node_t* c = obj->faces->head;
  for (u32 i = 0; i < obj->faces->len; i++) {
    Wavefront__Face_t* f = c->data;
    vec3* v00 = List__get(obj->vertices, f->vertex_idx[0] - 1);
    vec3* v01 = List__get(obj->vertices, f->vertex_idx[1] - 1);
    vec3* v02 = List__get(obj->vertices, f->vertex_idx[2] - 1);
    vec2* uv0 = List__get(obj->texcoords, f->texcoord_idx[0] - 1);
    vec2* uv1 = List__get(obj->texcoords, f->texcoord_idx[1] - 1);
    vec2* uv2 = List__get(obj->texcoords, f->texcoord_idx[2] - 1);
    c = c->next;

    // Project the 3D vertices to 2D screen space
    vec4 v0, v1, v2, ndc0, ndc1, ndc2;
    project(state, false, model, view, projection, *v00, ndc0, v0);
    project(state, false, model, view, projection, *v01, ndc1, v1);
    project(state, false, model, view, projection, *v02, ndc2, v2);

    // Color for this face
    // u8 r = Math__map(Math__triangleWave(v0[1], 10), -1, 1, 128, 255);
    // u8 g = Math__map(Math__triangleWave(v1[1], 10), -1, 1, 128, 255);
    // u8 b = Math__map(Math__triangleWave(v2[1], 10), -1, 1, 128, 255);
    // u8 r = Math__map(Math__triangleWave(i, obj->faces->len), -1, 1, 128, 255);
    // u8 g = Math__map(Math__triangleWave(i + 1, obj->faces->len), -1, 1, 128, 255);
    // u8 b = Math__map(Math__triangleWave(i + 2, obj->faces->len), -1, 1, 128, 255);
    // u32 color = 0xff000000 | b << 16 | g << 8 | r;

    draw_triangle(
        state,
        screen,
        logic->zbuf,
        false,
        v0,
        v1,
        v2,
        ndc0,
        ndc1,
        ndc2,
        true,
        &logic->atlas,
        tx0,
        ty0,
        *uv0,
        *uv1,
        *uv2,
        false,
        0,
        color);
  }

  return;
}

void Bitmap3D__RenderSprite(
    Engine__State_t* state,
    f64 x,
    f64 y,
    f64 z,
    u32 tx0,
    u32 ty0,
    bool useMask,
    u32 mask,
    u32 color) {
  Logic__State_t* logic = state->local;
  Bitmap_t* atlas = &state->local->atlas;
  Bitmap_t* screen = &state->local->screen;

  vec3* pos = (vec3*)&logic->game->curPlyr->tform->pos;
  vec3 mpos = (vec3){x, y, z};
  vec3 to_camera;
  glms_vec3_sub(*pos, mpos,
                to_camera);  // Vector from sprite to camera
  glms_vec3_normalize(to_camera);

  // a 2D billboard that rotates around the Y axis only
  // Get the angle to rotate around the Y axis by projecting the "to_camera" vector on the XZ
  // plane.
  f32 angle = atan2f(to_camera[0], to_camera[2]);
  f32 s = Math__sin((angle));
  // print_vec4(state, 20, (vec4){angle, to_camera[0], to_camera[1], 0}, LIME);
  f32 c = Math__cos((angle));

  // Model matrix
  mat4 model2 = {
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1},
  };
  model2[0][3] = x;
  model2[1][3] = y;
  model2[2][3] = z;
  model2[0][0] = c;
  model2[0][2] = s;
  model2[2][0] = -s;
  model2[2][2] = c;

  // points
  f32 u = 0.5f;
  vec3 v00 = {-u, +u, 0};  // tl
  vec3 v01 = {+u, +u, 0};  // tr
  vec3 v02 = {+u, -u, 0};  // br
  vec3 v03 = {-u, -u, 0};  // bl

  // uv map
  vec2 uv0 = {0, 1};  // tl
  vec2 uv1 = {1, 1};  // tr
  vec2 uv2 = {1, 0};  // br
  vec2 uv3 = {0, 0};  // bl

  // Project the 3D vertices to 2D screen space
  vec4 v0, v1, v2, v3, ndc0, ndc1, ndc2, ndc3;
  project(state, false, model2, view, projection, v00, ndc0, v0);
  project(state, false, model2, view, projection, v01, ndc1, v1);
  project(state, false, model2, view, projection, v02, ndc2, v2);
  project(state, false, model2, view, projection, v03, ndc3, v3);

  // face tri 1
  // upper tri: bl tl tr
  draw_triangle(
      state,
      screen,
      logic->zbuf,
      false,
      v2,
      v1,
      v0,
      ndc2,
      ndc1,
      ndc0,
      true,
      &logic->atlas,
      tx0,
      ty0,
      uv2,
      uv1,
      uv0,
      useMask,
      mask,
      color);

  // face tri 2
  // upper tri: bl tl tr
  draw_triangle(
      state,
      screen,
      logic->zbuf,
      false,
      v0,
      v3,
      v2,
      ndc0,
      ndc3,
      ndc2,
      true,
      &logic->atlas,
      tx0,
      ty0,
      uv0,
      uv3,
      uv2,
      useMask,
      mask,
      color);
}

void Bitmap3D__PostProcessing(Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Level_t* level = logic->game->curLvl;
  Player_t* player = (Player_t*)logic->game->curPlyr;
  u32* buf = (u32*)logic->screen.buf;
  f32 cRX = 0, cRY = 0;
  cRX = player->base.tform->rot.y;
  cRY = player->base.tform->rot.x;

  // fog distance by zbuf
  for (u32 i = 0; i < logic->screen.len; i++) {
    f32 b0 = f32_zbuf_u8(logic->zbuf[i]);
    u32 color = buf[i];
    // cat eyes glow in the dark
    // if (0xff1de6b5 == color) b0 = MATH_CLAMP(0, 1.0f - b0, 1);
    // if (0xff00f2f4 == color) b0 = MATH_CLAMP(0, b0 * 2.4f, 1);
    if (0xff00f2f4 == color || 0xff1de6b5 == color) {
      b0 = 200;
    }
    // blackness of varying alpha overlaid on existing color
    buf[i] = alpha_blend(color, (u32)(255 - b0) << 24);
  }

  // skybox
  if (level->skybox) {
    for (u32 i = 0; i < logic->screen.len; i++) {
      if (FLT_MIN == logic->zbuf[i]) {
        s32 xx = ((s32)floorf((i % (u32)W) - glms_rad(cRX) * 512 / (Math__PI * 2))) & 511;
        s32 yy = ((s32)floorf((i / H) - glms_rad(cRY + 10) * 512 / (Math__PI))) & 511;
        u32 color = Bitmap__Get2DPixel(level->world, xx, yy, BLACK);
        buf[i] = alpha_blend(color, level->ceilCol);
      }
    }
  }

  // player hurt blood spatter
  if (player->base.health->hurtTime > 0) {
    f32 t = 1 - (player->base.health->hurtTime / PLAYER_HURT_ANIM_TIME);
    f32 offs = 1.0f * easeInQuart(t);
    // if (player->base.tags1 & TAG_DEAD) offs = 0.5;
    for (u32 i = 0; i < logic->screen.len; i++) {
      f32 xp = ((i % logic->screen.w) - logic->screen.w / 2.0f) / logic->screen.w * 2.0f;
      f32 yp = (((f32)i / logic->screen.h) - logic->screen.h / 2.0f) / logic->screen.h * 2.0f;

      if (Math__random(0, 1) + offs < sqrtf(xp * xp + yp * yp)) {
        u32 color = (u32)(Math__random(0, 5) / 4.0f) * 0xff000077;
        buf[i] = color;
      }
    }
  }
}