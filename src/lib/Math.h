#ifndef MATH_H
#define MATH_H

#include "Base.h"

f64 Math__map(f64 n, f64 input_start, f64 input_end, f64 output_start, f64 output_end);
f64 Math__mod(f64 a, f64 b);
f32 Math__random(f32 a, f32 b);
s32 Math__srandom(s32 a, s32 b);
u32 Math__urandom(u32 a, u32 b);

#endif