#include "TitleMenu.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Engine.h"

Menu_t* TitleMenu__alloc(Engine__State_t* state) {
  return Arena__Push(state->arena, sizeof(TitleMenu_t));
}

void TitleMenu__init(Menu_t* menu, Engine__State_t* state) {
  menu->tick = &TitleMenu__tick;
  menu->render = &TitleMenu__render;
  state->Vulkan__FReadImage(&((TitleMenu_t*)menu)->bmp, "../assets/textures/title.png");
}

void TitleMenu__tick(struct Menu_t* menu, void* _state) {
  Engine__State_t* state = _state;

  if (state->inputState->use) {
    state->inputState->use = false;
    state->local->game->menu = NULL;
  }
}

void TitleMenu__render(struct Menu_t* menu, Bitmap_t* target) {
  Bitmap__Draw(&((TitleMenu_t*)menu)->bmp, target, 0, 0);
}