#ifndef GLM_SHIM
#define GLM_SHIM

#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>

// typedef float f32;
// typedef struct vec2 {
//   f32 x, y;
// } vec2;

// typedef struct vec3 {
//   f32 x, y, z;
// } vec3;

// typedef struct vec4 {
//   f32 x, y, z, w;
// } vec4;

// typedef struct mat4 {
//   vec4 a, b, c, d;
// } mat4;

// f32 glm_rad(f32 degrees);
// void glm_vec3_copy(vec3 src, vec3* dest);
// void glm_vec2_copy(vec2 src, vec2* dest);
// void glm_vec3_normalize(vec3* v);
// void glm_vec3_cross(vec3 a, vec3 b, vec3* result);
// // void glm_vec3_scale(vec3* v, f32 scale);
// void glm_vec3_scale(vec3 v, f32 scale, vec3* result);
// void glm_vec3_add(vec3 a, vec3 b, vec3* result);
// void glm_lookat(vec3 eye, vec3 center, vec3 up, mat4 result);
// void glm_perspective(f32 fov, f32 aspect, f32 near, f32 far, mat4 result);

#endif  // GLM_SHIM
