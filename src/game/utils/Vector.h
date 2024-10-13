#pragma once

#include "../../lib/Math.h"

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
  r[0] = (a * rSin) - (b * rCos);  // a-axis
  r[1] = (b * rCos) + (a * rSin);  // b-axis
}
