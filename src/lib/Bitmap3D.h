#ifndef BITMAP3D_H
#define BITMAP3D_H

#include "Engine.h"

void Bitmap3D__Perspective(f32 fov, f32 aspect, f32 nearZ, f32 farZ, mat4 result);
void Bitmap3D__RenderHorizon(Engine__State_t* game);
void Bitmap3D__RenderFloor(Engine__State_t* game);
void Bitmap3D__PostProcessing(Engine__State_t* game);
void Bitmap3D__RenderWall2(
    Engine__State_t* game, f64 x0, f64 y0, f64 x1, f64 y1, u32 tex, u32 color, f64 tx, f64 ty);

#endif  // BITMAP3D_H
