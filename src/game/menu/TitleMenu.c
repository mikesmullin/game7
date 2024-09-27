#include "TitleMenu.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Engine.h"
#include "Menu.h"

TitleMenu_t* TitleMenu__alloc(Engine__State_t* game) {
  return Arena__Push(game->arena, sizeof(TitleMenu_t));
}

void TitleMenu__init(TitleMenu_t* menu, Engine__State_t* game) {
  menu->menu = Menu__alloc(game);
  Menu__init(menu->menu, game);
  menu->selected = true;
  game->Vulkan__FReadImage(&menu->bmp, "../assets/textures/title.png");
}

void TitleMenu__render(TitleMenu_t* menu, Engine__State_t* game) {
  Bitmap__Draw(&menu->bmp, &game->local->screen, 0, 0);
}

void TitleMenu__tick(TitleMenu_t* menu, Engine__State_t* game) {
  if (game->g_Keyboard__state->eKey) {
    game->local->game->menu = NULL;
  }
}