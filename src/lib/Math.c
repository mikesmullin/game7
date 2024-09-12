#include "Math.h"

#include <math.h>

f64 Math__map(f64 n, f64 input_start, f64 input_end, f64 output_start, f64 output_end) {
  f64 range = 1.0 * (output_end - output_start) / (input_end - input_start);
  return output_start + range * (n - input_start);
}

f64 Math__mod(f64 a, f64 b) {
  while (isnormal(a) && a > b) {
    a = MATH_MAX(0, a - b);
  }
  return a;
}

f32 Math__random(f32 a, f32 b) {
  return a + (((f32)rand()) / (f32)RAND_MAX) * (b - a);
}

s32 Math__srandom(s32 a, s32 b) {
  return a + (((s32)rand()) / (s32)RAND_MAX) * (b - a);
}

u32 Math__urandom(u32 a, u32 b) {
  return a + ((rand() / (f32)RAND_MAX) * (b - a));
}