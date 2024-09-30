#pragma once

#include "Math.h"

typedef float f32;

f32 deg2rad(f32 deg) {
  return (Math__PI / 180.0f) * deg;
}

// rotate plane along axis
void rot2d(f32 a, f32 b, f32 rSin, f32 rCos, f32* r) {
  // see: https://www.desmos.com/calculator/uvrekqtsis
  // a oscillates along the a-axis the circle intersecting a,y
  // a-axis range +/- a+b+e
  // where e = the extra required for the circle to intersect at a,b
  // b-axis range is exactly the same, but offset by 90 degree phase (obviously)
  r[0] = -(b * rSin) + (a * rCos);  // a-axis
  r[1] = (a * rSin) + (b * rCos);   // b-axis
}

// project Syn (screen) to Tz (texture) coord in perspective
f32 perspective(f32 Syn, f32 camZ, f32 Wh) {
  f32 Tz;
  camZ = MATH_CLAMP(0, camZ, Wh);
  f32 Vhn = ((camZ / Wh) * 2.0f) - 1.0f;  // -1 = floor, 0 = middle, 1 = ceiling
  f32 Vh = Vhn * Wh;
  f32 Ty = 1.0f;
  if (Syn > 0) {
    // floor
  }
  if (Syn < 0) {
    // ceiling
    Vh *= -1.0f;  // determines eye position
    Ty *= -1.0f;  // ensure ceiling is mirrored, not flipped
  }
  Ty *= Syn;
  Tz = (Wh + Vh);  // tile repeat count
  Tz /= Ty;        // perspective projection (depth)
                   //  near = few repeats, far = many repeats
  return Tz;
}
