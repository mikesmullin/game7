#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef uint32_t u32;
typedef float f32;
typedef double f64;
typedef struct Engine__State_t Engine__State_t;

void Bitmap3D__RenderHorizon(Engine__State_t* state);
void Bitmap3D__RenderSprite(
    Engine__State_t* state,
    f64 x,
    f64 y,
    f64 z,
    u32 tx0,
    u32 ty0,
    bool useMask,
    u32 mask,
    u32 color);
void Bitmap3D__PostProcessing(Engine__State_t* game);
void Bitmap3D__RenderWall(
    Engine__State_t* state, f32 x0, f32 y0, f32 z0, u32 tx0, u32 ty0, u32 color);