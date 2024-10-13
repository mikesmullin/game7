#pragma once

#include <math.h>

typedef double f64;

f64 easeInQuart(f64 t) {
  return t * t * t * t;
}

f64 easeOutQuart(f64 t) {
  return 1 - pow(1 - t, 4);
}

f64 easeInOutQuart(f64 t) {  // in/out 0 .. 1
  return t < 0.5 ? 8 * t * t * t * t : 1 - pow(-2 * t + 2, 4) / 2;
}
