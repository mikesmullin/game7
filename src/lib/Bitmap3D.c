#include "Bitmap3D.h"

#include "Base.h"
#include "Engine.h"
#include "Math.h"

static u8 atlas_tile_size = 8 - 1;
static u8 atlas_dim = 64;
static u32 W;
static u32 H;
static u32 PS = 8;  // pixel super sample factor

void Bitmap3D__Perspective(f32 fov, f32 aspect, f32 nearZ, f32 farZ, mat4 result) {
  // Calculate the perspective projection matrix
  float tan_half_fov = Math__tan(fov / 2.0f);  // Calculate tan(fov/2)

  // Set the matrix to zero
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) result[i][j] = 0.0f;

  // Set the relevant values for the projection matrix
  result[0][0] = 1.0f / (aspect * tan_half_fov);
  result[1][1] = 1.0f / tan_half_fov;
  result[2][2] = -(farZ + nearZ) / (farZ - nearZ);
  result[2][3] = -1.0f;
  result[3][2] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
  result[3][3] = 0.0f;
}

// Function to multiply a 4x4 matrix by a 4D vector
void mat4_mulv(const mat4 matrix, const vec4 vector, vec4 result) {
  // Optimized loop for multiplying each row of the matrix by the vector
  for (u32 i = 0; i < 4; i++) {
    result[i] = matrix[i][0] * vector[0] + matrix[i][1] * vector[1] + matrix[i][2] * vector[2] +
                matrix[i][3] * vector[3];
  }
}

// Perspective projection and draw function with camera
void Bitmap3D__Set3DPixel(
    Bitmap_t* bmp, f32 x, f32 y, f32 z, u32 color, mat4 model, mat4 view, mat4 projection) {
  // Create a vec4 for the point in model space (homogeneous coordinates)
  vec4 model_point = {x, y, z, 1.0f};

  // Transform the point by the model matrix (from model space to world space)
  vec4 world_point;
  glm_mat4_mulv(model, model_point, world_point);

  // Transform the point by the view (camera) matrix (from world space to camera space)
  vec4 camera_point;
  glm_mat4_mulv(view, world_point, camera_point);

  // Apply perspective projection (from camera space to clip space)
  vec4 clip_point;
  glm_mat4_mulv(projection, camera_point, clip_point);

  // Perform perspective division (convert homogeneous to normalized device coordinates)
  if (clip_point[3] != 0.0f) {
    clip_point[0] /= clip_point[3];
    clip_point[1] /= clip_point[3];
    clip_point[2] /= clip_point[3];
  }

  // Convert normalized device coordinates to screen space
  int screen_x = (int)((clip_point[0] + 1.0f) * 0.5f * W);
  int screen_y = (int)((1.0f - clip_point[1]) * 0.5f * H);

  // Draw the pixel in the RGBA buffer
  Bitmap__Set2DPixel(bmp, screen_x, screen_y, 0xffff00ff /*color*/);
  // Bitmap__Set2DPixel(bmp, (x * 2 - 1) * W, (y * 2 - 1) * H, 0xffff00ff);
}

void Bitmap3D__RenderHorizon(Engine__State_t* game) {
  W = game->CANVAS_WIDTH;
  H = game->CANVAS_HEIGHT;

  // Model matrix: Identity at first
  mat4 model;
  glm_mat4_identity(model);
  vec3 translation = {0.0f, 0.0f, -2.0f};
  glm_translate(model, translation);
  glm_rotate(
      model,
      glm_rad(Math__sin(game->local->currentTime / 1000) * 90.0f),
      (vec3){0.0f, 1.0f, 0.0f});

  // View matrix (camera setup)
  mat4 view;
  glm_mat4_identity(view);
  vec3 cameraPos = {0.0f, 0.0f, 3.0f};  // Camera position
  glm_vec3_add(cameraPos, game->local->player.transform.position, cameraPos);
  vec3 cameraTarget = {0.0f, 0.0f, 0.0f};  // Look at the origin
  glm_vec3_add(cameraTarget, game->local->player.transform.rotation, cameraTarget);
  vec3 up = {0.0f, 1.0f, 0.0f};  // Up direction
  glm_lookat(cameraPos, cameraTarget, up, view);

  // Projection matrix (Perspective projection)
  mat4 projection;
  float fovy = glm_rad(45.0f);  // Field of view (Y-axis)
  float aspect = 1.0f;          // Aspect ratio (window width / height)
  float nearZ = 0.1f;
  float farZ = 1000.0f;
  glm_perspective(fovy, aspect, nearZ, farZ, projection);

  // glm_vec3_add(
  //     game->local->player.transform.position,
  //     game->local->player.transform.rotation,
  //     center);
  // glm_lookat(
  //     game->local->player.transform.position,
  //     center /*(vec3){0.0f, 0.0f, 0.0f}*/,
  //     game->local->VEC3_Y_UP,  // Y-axis points upwards (GLM default)
  //     game->local->player.camera.view);

  // glm_translate(game->local->player.camera.view, game->local->player.transform.position);
  // glm_rotate(game->local->player.camera.view, );
  // Bitmap3D__Perspective(
  //     glm_rad(game->local->player.camera.fov),  // half the actual 90deg fov
  //     (f32)game->CANVAS_WIDTH / game->CANVAS_HEIGHT,
  //     game->local->player.camera.nearZ,
  //     game->local->player.camera.farZ,
  //     game->local->player.camera.projection);

  memset(game->local->screen.buf, 0, game->local->screen.len);

  for (f32 z = -1.0f; z <= 1.0f; z += 0.25f) {
    for (f32 y = -1.0f; y <= 1.0f; y += 0.25f) {
      for (f32 x = -1.0f; x <= 1.0f; x += 0.25f) {
        u32 color = (u32)0xff000000 | (((u32)((z + 1.0f) * 0.5f) << 16)) |
                    (((u32)((y + 1.0f) * 0.5f) << 8)) | ((u32)((x + 1.0f) * 0.5f));
        Bitmap3D__Set3DPixel(&game->local->screen, x, y, z, color, model, view, projection);
        // game->local->player.camera.view,
        // game->local->player.camera.projection);
      }
    }
  }

  // Bitmap3D__RenderWall(game, 1, 1, 2, 2);

  // Bitmap3D__RenderFloor(game);
}

void Bitmap3D__RenderWall(Engine__State_t* game, s32 x0, s32 y0, s32 x1, s32 y1) {
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
  return;
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