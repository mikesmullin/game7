#include "GLMShim.h"

#include <math.h>
#include <string.h>

#include "Math.h"

// Converts degrees to radians
f32 glms_rad(f32 degrees) {
  // return glm_rad(degrees);
  return degrees * (Math__PI / 180.0f);
}

// Copies a vec3
void glms_vec3_copy(vec3 src, vec3 dest) {
  // glm_vec3_copy(src, dest);
  memcpy(dest, src, sizeof(vec3));
}

// Copies a vec2
void glms_vec2_copy(vec2 src, vec2 dest) {
  // glm_vec2_copy(src, dest);
  memcpy(dest, src, sizeof(vec2));
}

// Normalizes a vec3
void glms_vec3_normalize(vec3 v) {
  // glm_vec3_normalize(v);
  f32 len = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  if (len > 0.0f) {
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
  }
}

// Cross product of two vec3
void glms_vec3_cross(vec3 a, vec3 b, vec3 result) {
  // glm_vec3_cross(a, b, result);
  result[0] = a[1] * b[2] - a[2] * b[1];
  result[1] = a[2] * b[0] - a[0] * b[2];
  result[2] = a[0] * b[1] - a[1] * b[0];
}

// Scales a vec3 by a scalar value
void glms_vec3_scale(vec3 v, f32 scale, vec3 result) {
  // glm_vec3_scale(v, scale, result);
  result[0] = v[0] * scale;
  result[1] = v[1] * scale;
  result[2] = v[2] * scale;
}

// Adds two vec3
void glms_vec3_add(vec3 a, vec3 b, vec3 result) {
  // glm_vec3_add(a, b, result);
  result[0] = a[0] + b[0];
  result[1] = a[1] + b[1];
  result[2] = a[2] + b[2];
}

// Creates a "look at" matrix (4x4)
void glms_lookat(vec3 eye, vec3 center, vec3 up, mat4 result) {
  // glm_lookat(eye, center, up, result);
  vec3 f, s, u;

  // Calculate the forward vector (center - eye)
  f[0] = center[0] - eye[0];
  f[1] = center[1] - eye[1];
  f[2] = center[2] - eye[2];
  glms_vec3_normalize(f);

  // Calculate the side vector (cross product of forward and up)
  glms_vec3_cross(f, up, s);
  glms_vec3_normalize(s);

  // Calculate the up vector (cross product of side and forward)
  glms_vec3_cross(s, f, u);

  result[0][0] = s[0];
  result[0][1] = u[0];
  result[0][2] = -f[0];
  result[0][3] = 0.0f;

  result[1][0] = s[1];
  result[1][1] = u[1];
  result[1][2] = -f[1];
  result[1][3] = 0.0f;

  result[2][0] = s[2];
  result[2][1] = u[2];
  result[2][2] = -f[2];
  result[2][3] = 0.0f;

  result[3][0] = -(s[0] * eye[0] + s[1] * eye[1] + s[2] * eye[2]);
  result[3][1] = -(u[0] * eye[0] + u[1] * eye[1] + u[2] * eye[2]);
  result[3][2] = f[0] * eye[0] + f[1] * eye[1] + f[2] * eye[2];
  result[3][3] = 1.0f;
}

// Creates a perspective projection matrix (4x4)
void glms_perspective(f32 fovy, f32 aspect, f32 nearZ, f32 farZ, mat4 dest) {
  float f, fn;

  f = 1.0f / tanf(fovy * 0.5f);
  fn = 1.0f / (nearZ - farZ);

  dest[0][0] = f / aspect;
  dest[0][1] = 0;
  dest[0][2] = 0;
  dest[0][3] = 0;
  dest[1][0] = 0;
  dest[1][1] = f;
  dest[1][2] = 0;
  dest[1][3] = 0;
  dest[2][0] = 0;
  dest[2][1] = 0;
  dest[2][2] = (nearZ + farZ) * fn;
  dest[2][3] = -1.0f;
  dest[3][0] = 0;
  dest[3][1] = 0;
  dest[3][2] = 2.0f * nearZ * farZ * fn;
  dest[3][3] = 0;
}