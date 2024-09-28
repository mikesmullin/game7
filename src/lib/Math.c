#include "Math.h"

#include <math.h>
#include <stdlib.h>

f64 Math__sin(f64 n) {
  return sin(n);
}

f64 Math__cos(f64 n) {
  return cos(n);
}

f64 Math__tan(f64 n) {
  return tan(n);
}

f64 Math__map(f64 n, f64 input_start, f64 input_end, f64 output_start, f64 output_end) {
  f64 range = 1.0 * (output_end - output_start) / (input_end - input_start);
  return output_start + range * (n - input_start);
}

f64 Math__pow(f64 n, f64 e) {
  return pow(n, e);
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

u32 Math__urandom() {
  u32 low = rand() & 0xffff;   // 16 bits +
  u32 high = rand() & 0xffff;  // 16 bits =
  return (high << 16) | low;   // 32 bits
}

u32 Math__urandom2(u32 a, u32 b) {
  return a + (Math__urandom() % (b - a + 1));
}

f64 Math__triangleWave(f64 x, f64 period) {
  return 2.0 * fabs(2.0 * (x / period - floor(x / period + 0.5))) - 1.0;
}