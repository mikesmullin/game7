#ifndef MENU_H
#define MENU_H

#include "../../lib/Engine.h"

Menu_t* Menu__alloc(Engine__State_t* game);
void Menu__init(Menu_t* menu, Engine__State_t* game);

#endif  // MENU_H