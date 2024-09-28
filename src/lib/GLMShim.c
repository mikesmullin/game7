// #include "GLMShim.h"

// #include <math.h>
// #include <string.h>

// #include "Math.h"

// // Converts degrees to radians
// f32 glm_rad(f32 degrees) {
//   return degrees * (Math__PI / 180.0f);
// }

// // Copies a vec3
// void glm_vec3_copy(vec3 src, vec3* dest) {
//   memcpy(dest, &src, sizeof(vec3));
// }

// // Copies a vec2
// void glm_vec2_copy(vec2 src, vec2* dest) {
//   memcpy(dest, &src, sizeof(vec2));
// }

// // Normalizes a vec3
// void glm_vec3_normalize(vec3* v) {
//   f32 len = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
//   if (len > 0.0f) {
//     v->x /= len;
//     v->y /= len;
//     v->z /= len;
//   }
// }

// // Cross product of two vec3
// void glm_vec3_cross(vec3 a, vec3 b, vec3* result) {
//   result->x = a.y * b.z - a.z * b.y;
//   result->y = a.z * b.x - a.x * b.z;
//   result->z = a.x * b.y - a.y * b.x;
// }

// // // Scales a vec3 by a scalar value
// // void glm_vec3_scale(vec3* v, f32 scale) {
// //   v->x *= scale;
// //   v->y *= scale;
// //   v->z *= scale;
// // }

// // Scales a vec3 by a scalar value
// void glm_vec3_scale(vec3 v, f32 scale, vec3* result) {
//   result->x = v.x * scale;
//   result->y = v.y * scale;
//   result->z = v.z * scale;
// }

// // Adds two vec3
// void glm_vec3_add(vec3 a, vec3 b, vec3* result) {
//   result->x = a.x + b.x;
//   result->y = a.y + b.y;
//   result->z = a.z + b.z;
// }

// // Creates a "look at" matrix (4x4)
// void glm_lookat(vec3 eye, vec3 center, vec3 up, mat4 result) {
//   vec3 f, s, u;

//   // Calculate the forward vector (center - eye)
//   f.x = center.x - eye.x;
//   f.y = center.y - eye.y;
//   f.z = center.z - eye.z;
//   glm_vec3_normalize(&f);

//   // Calculate the side vector (cross product of forward and up)
//   glm_vec3_cross(f, up, &s);
//   glm_vec3_normalize(&s);

//   // Calculate the up vector (cross product of side and forward)
//   glm_vec3_cross(s, f, &u);

//   result.a.x = s.x;
//   result.a.y = u.x;
//   result.a.z = -f.x;
//   result.a.w = 0.0f;

//   result.b.x = s.y;
//   result.b.y = u.y;
//   result.b.z = -f.y;
//   result.b.w = 0.0f;

//   result.c.x = s.z;
//   result.c.y = u.z;
//   result.c.z = -f.z;
//   result.c.w = 0.0f;

//   result.d.x = -(s.x * eye.x + s.y * eye.y + s.z * eye.z);
//   result.d.y = -(u.x * eye.x + u.y * eye.y + u.z * eye.z);
//   result.d.z = f.x * eye.x + f.y * eye.y + f.z * eye.z;
//   result.d.w = 1.0f;
// }

// // Creates a perspective projection matrix (4x4)
// void glm_perspective(f32 fov, f32 aspect, f32 nearz, f32 farz, mat4 result) {
//   f32 tanHalfFov = tanf(glm_rad(fov) / 2.0f);

//   result.a.x = 1.0f / (aspect * tanHalfFov);
//   result.a.y = 0.0f;
//   result.a.z = 0.0f;
//   result.a.w = 0.0f;

//   result.b.x = 0.0f;
//   result.b.y = 1.0f / tanHalfFov;
//   result.b.z = 0.0f;
//   result.b.w = 0.0f;

//   result.c.x = 0.0f;
//   result.c.y = 0.0f;
//   result.c.z = -(farz + nearz) / (farz - nearz);
//   result.c.w = -1.0f;

//   result.d.x = 0.0f;
//   result.d.y = 0.0f;
//   result.d.z = -(2.0f * farz * nearz) / (farz - nearz);
//   result.d.w = 0.0f;
// }