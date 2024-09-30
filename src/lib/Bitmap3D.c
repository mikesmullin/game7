#include "Bitmap3D.h"

#include "../game/Logic.h"
#include "Color.h"
#include "Easing.h"
#include "Engine.h"
#include "Math.h"

void Bitmap3D__RenderHorizon(Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Bitmap_t* bmp = &state->local->screen;

  Bitmap__Fill(bmp, 0, 0, bmp->w, bmp->h, 0xff000000);  // wipe black

  Bitmap__DebugText2(state, 4, 6 * 20, 0xff00ff00, 0, "debugger");
}

void Bitmap3D__RenderWall(
    Engine__State_t* state, f64 x0, f64 y0, f64 x1, f64 y1, u32 tex, u32 color, f64 tx, f64 ty) {
}

void Bitmap3D__RenderSprite(Engine__State_t* state, f64 x, f64 y, f64 z, u32 tex, u32 color) {
}

void Bitmap3D__PostProcessing(Engine__State_t* state) {
}