#include "TitleMenu.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Engine.h"
#include "Menu.h"

Menu_t* TitleMenu__alloc(Engine__State_t* state) {
  return Arena__Push(state->arena, sizeof(TitleMenu_t));
}

void TitleMenu__tick(
    struct Menu_t* menu, bool up, bool down, bool left, bool right, bool use, void* state) {
  if (use) {
    ((Engine__State_t*)state)->local->game->menu = NULL;
  }
}

void TitleMenu__render(struct Menu_t* menu, Bitmap_t* target) {
  Bitmap__Draw(&((TitleMenu_t*)menu)->bmp, target, 0, 0);
}

void TitleMenu__init(Menu_t* menu, Engine__State_t* state) {
  Menu__init(menu, state);
  menu->tick = &TitleMenu__tick;
  menu->render = &TitleMenu__render;
  ((TitleMenu_t*)menu)->selected = true;
  state->Vulkan__FReadImage(&((TitleMenu_t*)menu)->bmp, "../assets/textures/title.png");
}
