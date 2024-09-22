#ifndef BITMAP3D_H
#define BITMAP3D_H

#include "Engine.h"

void Bitmap3D__Perspective(f32 fov, f32 aspect, f32 nearZ, f32 farZ, mat4 result);
void Bitmap3D__RenderHorizon(Engine__State_t* game);
void Bitmap3D__RenderFloor(Engine__State_t* game);
void Bitmap3D__RenderWall(Engine__State_t* game, s32 x0, s32 y0, s32 x1, s32 y1);
void Bitmap3D__PostProcessing(Engine__State_t* game);

#endif  // BITMAP3D_H
