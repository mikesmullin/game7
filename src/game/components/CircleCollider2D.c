#include "CircleCollider2D.h"

bool CircleCollider2D__check(f32 x0, f32 y0, f32 r0, f32 x1, f32 y1, f32 r1) {
  // TODO: use component transforms (then everywhere)

  // Check for overlap using AABB (Axis-Aligned Bounding Box)
  // based on projected a position, and existing b position
  bool overlap_x = (x0 - r0 < x1 + r1) && (x0 + r0 > x1 - r1);
  bool overlap_y = (y0 - r0 < y1 + r1) && (y0 + r0 > y1 - r1);

  // If both x and y axes overlap, a collision is detected
  if (overlap_x && overlap_y) {
    return true;
  }

  // No collision detected, movement allowed
  return false;
}