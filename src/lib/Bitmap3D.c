#include "Bitmap3D.h"

#include "../game/Logic.h"
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
  // print_mat4(state, 16, rot1);
  // print_mat4(state, 16 + 4, rot2);
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
  mat4 rot1;
  glm_rotate_z((mat4)GLM_MAT4_IDENTITY_INIT, glms_rad(deg), rot1);
  mat4 rot2 = {
      {c, -s, 0, 0},  //
      {s, c, 0, 0},   //
      {0, 0, 1, 0},   //
      {0, 0, 0, 1},   //
  };
  glms_mat4_mulv(rot2, vc, dest);
}

void mat4_proj(Engine__State_t* state, f32 aspect, f32 fovy, f32 nearZ, f32 farZ, mat4 dest) {
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
f64 lerp(f64 a, f64 b, f64 t) {
  return a + t * (b - a);
}

bool project(Engine__State_t* state, vec3 v0, vec3 dest) {
  Logic__State_t* logic = state->local;
  Bitmap_t* screen = &state->local->screen;
  Player_t* player = (Player_t*)state->local->game->curPlyr;

  f32 cX = 0, cY = 0, cZ = 0, cRX, cRY;
  cX = player->base.transform.position.x;
  cY = player->base.transform.position.y;
  cZ = player->base.transform.position.z;
  cRX = player->base.transform.rotation.y;
  cRY = player->base.transform.rotation.x;

  mat4 model = {
      {1, 0, 0, 0},   //
      {0, 1, 0, 0},   //
      {0, 0, 1, -3},  //
      {0, 0, 0, 1},   //
  };
  mat4 view = {
      {1, 0, 0, -cX},  //
      {0, 1, 0, -cY},  //
      {0, 0, 1, -cZ},  //
      {0, 0, 0, 1},    //
  };
  mat4 projection;
  float fovy = glms_rad(90.0f);
  float aspect = W / H;
  float nearZ = 100.0f;
  float farZ = 3.0f;
  // glm_perspective(fovy, aspect, nearZ, farZ, projection);
  mat4_proj(state, aspect, fovy, nearZ, farZ, projection);

  // Create a vec4 for the point in model space (homogeneous coordinates)
  vec4 model_point = {v0[0], v0[1], v0[2], 1.0f};

  // Transform the point by the model matrix (from model space to world space)
  vec4 world_point;
  glms_mat4_mulv(model, model_point, world_point);

  // Transform the point by the view (camera) matrix (from world space to camera space)
  vec4 camera_point;
  glms_mat4_mulv(view, world_point, camera_point);
  mat4_roty(state, camera_point, cRX, camera_point);
  mat4_rotx(state, camera_point, cRY, camera_point);

  // Apply perspective projection (from camera space to clip space)
  vec4 clip_point;
  glms_mat4_mulv(projection, camera_point, clip_point);

  // Perform perspective division (convert homogeneous to normalized device coordinates)
  vec4 ndc;
  if (clip_point[3] != 0.0f) {
    ndc[0] = clip_point[0] / clip_point[3];
    ndc[1] = clip_point[1] / clip_point[3];
    ndc[2] = clip_point[2] / clip_point[3];
  }
  // if (ndc[0] < -1.0f || ndc[0] > 1.0f) return false;
  // if (ndc[1] < -1.0f || ndc[1] > 1.0f) return false;
  // if (ndc[2] < -1.0f || ndc[2] > 1.0f) return false;

  // Convert normalized device coordinates to screen space
  s32 sx = (s32)((ndc[0] + 1.0f) * 0.5f * W);
  s32 sy = (s32)((1.0f - ndc[1]) * 0.5f * H);

  dest[0] = sx;
  dest[1] = sy;
  dest[2] = ndc[2];

  return true;
}

void draw_triangle(Bitmap_t* screen, f32* zbuffer, vec3 a, vec3 b, vec3 c, bool upper, u32 color) {
  for (f32 y = a[1]; y <= b[1]; y++) {
    f32 t0 = upper ? (y - a[1]) / (c[1] - a[1]) : (y - c[1]) / (b[1] - c[1]);
    f32 t1 = (y - a[1]) / (b[1] - a[1]);
    f32 x0 = upper ? lerp(a[0], c[0], t0) : lerp(c[0], b[0], t0);
    f32 x1 = lerp(a[0], b[0], t1);
    f32 z0_interpolated = upper ? lerp(a[2], c[2], t0) : lerp(b[2], c[2], t0);
    f32 z1_interpolated = upper ? lerp(a[2], b[2], t1) : lerp(a[2], c[2], t1);

    if (x0 > x1) {
      f32 tmp = x0;
      x0 = x1;
      x1 = tmp;
      f32 tmpZ = z0_interpolated;
      z0_interpolated = z1_interpolated;
      z1_interpolated = tmpZ;
    }
    for (f32 x = x0; x <= x1; x++) {
      // Horizontal interpolation factor
      f32 t = (x - x0) / (x1 - x0);
      // Interpolated Z value for this pixel
      f32 z = lerp(z0_interpolated, z1_interpolated, t);

      // Check if the pixel's depth is closer than what's stored in the Z-buffer
      u32 i = y * W + x;
      if (z > zbuffer[i]) {
        // Update Z-buffer with the new depth value
        zbuffer[i] = z;
        // Draw the Z-buffer (for debugging)
        // u32 cmp = Math__map(z, FLT_MIN, FLT_MAX, 255, 128);
        // Bitmap__Set2DPixel(screen, x, y, 0xff000000 | cmp << 16 | cmp << 8 | cmp);

        // Draw the pixel in the RGBA buffer
        Bitmap__Set2DPixel(screen, x, y, color);
      }
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
  cX = player->base.transform.position.x;
  cY = player->base.transform.position.y;
  cZ = player->base.transform.position.z;
  cRX = player->base.transform.rotation.y;
  cRY = player->base.transform.rotation.x;
  logic->game->curLvl->spawner->base.x = 0.0f;
  logic->game->curLvl->spawner->base.y = 0.0f;

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
  f32 xa = 0, ya = 0, za = -3;
  // xa = Math__map(s1, -1, 1, -1, 1);
  // ya = Math__map(c1, -1, 1, -1, 1);
  // za = Math__map(c2, -1, 1, -10, 0);
  mat4 view = {
      {1, 0, 0, -cX},  //
      {0, 1, 0, -cY},  //
      {0, 0, 1, -cZ},  //
      {0, 0, 0, 1},    //
  };
  mat4 translate1 = {
      {1, 0, 0, xa},  //
      {0, 1, 0, ya},  //
      {0, 0, 1, za},  //
      {0, 0, 0, 1},   //
  };
  f32 s2 = Math__triangleWave(state->currentTime, 1000 * 1);
  f32 sz1 = 1.0;
  sz1 = Math__map(s2, -1, 1, 0.5, 2);
  mat4 scale1 = {
      {sz1, 0, 0, 0},  //
      {0, sz1, 0, 0},  //
      {0, 0, sz1, 0},  //
      {0, 0, 0, 1},    //
  };
  // print_mat4(state, 16 + 4, view);

  // Projection matrix (Perspective projection)
  mat4 projection;
  float fovy = glms_rad(90.0f);
  float aspect = W / H;
  float nearZ = 10.0f;
  float farZ = 1.0f;
  // glm_perspective(fovy, aspect, nearZ, farZ, projection);
  mat4_proj(state, aspect, fovy, nearZ, farZ, projection);
  // print_mat4(state, 16 + 8, projection);

  // --- POINT INTERPOLATION ----
  Wavefront_t* obj = List__get(logic->game->meshes, MODEL_BOX);
  // Wavefront__print_obj(obj);

  f32 zbuffer[(u32)(W * H)];
  for (u32 i = 0; i < W * H; i++) {
    zbuffer[i] = FLT_MIN;  // Initialize with a very large value
  }

  // render_mesh()
  // Project and draw all triangles that form the faces
  List__Node_t* c = obj->faces->head;
  for (s32 i = 0; i < obj->faces->len; i++) {
    Wavefront__Face_t* f = c->data;
    vec3* v00 = List__get(obj->vertices, f->vertex_idx[0] - 1);
    vec3* v01 = List__get(obj->vertices, f->vertex_idx[1] - 1);
    vec3* v02 = List__get(obj->vertices, f->vertex_idx[2] - 1);
    c = c->next;

    // Project the 3D vertices to 2D screen space
    vec3 v0, v1, v2;
    project(state, *v00, v0);
    project(state, *v01, v1);
    project(state, *v02, v2);

    // Perform basic clipping on each vertex
    if (v0[0] < 0) v0[0] = 0;
    if (v0[0] >= W) v0[0] = W - 1;
    if (v0[1] < 0) v0[1] = 0;
    if (v0[1] >= H) v0[1] = H - 1;

    if (v1[0] < 0) v1[0] = 0;
    if (v1[0] >= W) v1[0] = W - 1;
    if (v1[1] < 0) v1[1] = 0;
    if (v1[1] >= H) v1[1] = H - 1;

    if (v2[0] < 0) v2[0] = 0;
    if (v2[0] >= W) v2[0] = W - 1;
    if (v2[1] < 0) v2[1] = 0;
    if (v2[1] >= H) v2[1] = H - 1;

    // // Calculate the bounding box for the triangle
    // f32 min_x = glm_min(v0[0], glm_min(v1[0], v2[0]));
    // f32 max_x = glm_max(v0[0], glm_max(v1[0], v2[0]));
    // f32 min_y = glm_min(v0[1], glm_min(v1[1], v2[1]));
    // f32 max_y = glm_max(v0[1], glm_max(v1[1], v2[1]));

    // // Calculate the width and height of the bounding box
    // f32 width = max_x - min_x;
    // f32 height = max_y - min_y;

    // // Check if the bounding box is too large (greater than 180x180 pixels)
    // if (width > W || height > H) {
    //   continue;  // Skip rendering this triangle
    // }

    // Color for this face
    u8 r = Math__map(Math__triangleWave(v0[1], 10), -1, 1, 128, 255);
    u8 g = Math__map(Math__triangleWave(v1[1], 10), -1, 1, 128, 255);
    u8 b = Math__map(Math__triangleWave(v2[1], 10), -1, 1, 128, 255);
    u32 color = 0xff000000 | b << 16 | g << 8 | r;

    // Draw the triangle for this face
    // draw_triangle(p0, p1, p2, color);

    // Sort vertices by y-coordinate (v0[1] <= v1[1] <= v2[1])
    if (v0[1] > v1[1]) {
      vec2 temp = (vec2){v0[0], v0[1]};
      v0[0] = v1[0], v0[1] = v1[1];
      v1[0] = temp[0], v1[1] = temp[1];
    }
    if (v1[1] > v2[1]) {
      vec2 temp = (vec2){v1[0], v1[1]};
      v1[0] = v2[0], v1[1] = v2[1];
      v2[0] = temp[0], v2[1] = temp[1];
    }
    if (v0[1] > v1[1]) {
      vec2 temp = (vec2){v0[0], v0[1]};
      v0[0] = v1[0], v0[1] = v1[1];
      v1[0] = temp[0], v1[1] = temp[1];
    }

    // Draw the upper part of the triangle (from v0 to v1)
    draw_triangle(screen, zbuffer, v0, v1, v2, true, color);

    // Draw the lower part of the triangle (from v1 to v2)
    draw_triangle(screen, zbuffer, v1, v2, v0, false, color);
  }

  return;
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
        if (color == WHITE) {
          color = (u32)(state->currentTime / (1000.0f / 8)) % 2 ? WHITE : BLACK;
          // color =
          // (u32)0xff000000 | Math__urandom(128, 255) << 16 | 128 << 8 | Math__urandom(128, 255);
        }

        // Create a vec4 for the point in model space (homogeneous coordinates)
        vec4 model_point = {x, y, z, 1.0f};

        // correct orientation will have: White Top-Right-Front(toward your eye) corner
        //
        //                            BGR
        // 80ff80 = Green -+-  80ff80 -+- +Y_UP
        // 80ffff = Teal -++   ffff80 ++- +Z_FWD
        // 808080 = Grey ---   808080 --- -X_RIGHT
        // 8080ff = Purple --+ ff8080 -++
        // ff8080 = Red +--    8080ff --+
        // ff80ff = Pink +-+   ff80ff +-+
        // ffff80 = Yellow ++  80ffff -++
        // ffffff = White +++  ffffff +++
        //                            ZYX

        // test: translate
        // vec4 mp0;
        // glm_vec4_copy(model_point, mp0);
        // glms_mat4_mulv(translate1, mp0, model_point);

        // f32 deg = Math__map(Math__sin(state->currentTime / (1000 * 1)), -1, 1, 0, 180);
        // mat4_rotx(state, model_point, deg, model_point);
        // mat4_roty(state, model_point, deg, model_point);
        // mat4_rotz(state, model_point, deg, model_point);

        // Transform the point by the model matrix (from model space to world space)
        vec4 world_point;
        glms_mat4_mulv(model, model_point, world_point);

        // Transform the point by the view (camera) matrix (from world space to camera space)
        vec4 camera_point;
        glms_mat4_mulv(view, world_point, camera_point);
        mat4_roty(state, camera_point, cRX, camera_point);
        mat4_rotx(state, camera_point, cRY, camera_point);

        // Apply perspective projection (from camera space to clip space)
        vec4 clip_point;
        glms_mat4_mulv(projection, camera_point, clip_point);

        // Perform perspective division (convert homogeneous to normalized device coordinates)
        vec4 ndc;
        if (clip_point[3] != 0.0f) {
          ndc[0] = clip_point[0] / clip_point[3];
          ndc[1] = clip_point[1] / clip_point[3];
          ndc[2] = clip_point[2] / clip_point[3];
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
        s32 sy = (s32)((1.0f - ndc[1]) * 0.5f * H);

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