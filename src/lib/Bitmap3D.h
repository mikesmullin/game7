#ifndef BITMAP3D_H
#define BITMAP3D_H

#include <stdint.h>
typedef uint32_t u32;
typedef float f32;
typedef double f64;
typedef struct Engine__State_t Engine__State_t;

void Bitmap3D__RenderHorizon(Engine__State_t* game);
void Bitmap3D__RenderFloor(Engine__State_t* game);
void Bitmap3D__PostProcessing(Engine__State_t* game);
void Bitmap3D__RenderWall2(
    Engine__State_t* game, f64 x0, f64 y0, f64 x1, f64 y1, u32 tex, u32 color, f64 tx, f64 ty);

#endif  // BITMAP3D_H
