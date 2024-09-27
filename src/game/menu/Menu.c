#include "Menu.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"

Menu_t* Menu__alloc(Engine__State_t* game) {
  return Arena__Push(game->arena, sizeof(TitleMenu_t));
}

void Menu__init(Menu_t* menu, Engine__State_t* game) {
  menu->pretend = true;
}
