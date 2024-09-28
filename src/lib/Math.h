#ifndef MATH_H
#define MATH_H

#include <stdint.h>
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
typedef double f64;

#define Math__PI 3.14159265358979323846
#define MATH_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MATH_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MATH_CLAMP(min, n, max) (((n) < (min)) ? (min) : ((max) < (n)) ? (max) : (n))

f64 Math__sin(f64 n);
f64 Math__cos(f64 n);
f64 Math__tan(f64 n);
f64 Math__map(f64 n, f64 input_start, f64 input_end, f64 output_start, f64 output_end);
f64 Math__pow(f64 n, f64 e);
f64 Math__mod(f64 a, f64 b);
f32 Math__random(f32 a, f32 b);
s32 Math__srandom(s32 a, s32 b);
u32 Math__urandom();
u32 Math__urandom2(u32 a, u32 b);
f64 Math__triangleWave(f64 x, f64 period);
f64 Math__fmod(f64 n, f64 max);

#endif