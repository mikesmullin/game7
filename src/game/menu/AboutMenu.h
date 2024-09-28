#ifndef ABOUT_MENU_H
#define ABOUT_MENU_H

#include "../../lib/Engine.h"

Menu_t* AboutMenu__alloc(Arena_t* state);
void AboutMenu__init(Menu_t* menu, Engine__State_t* state);
void AboutMenu__tick(struct Menu_t* menu, void* state);
void AboutMenu__render(struct Menu_t* menu, void* state);

#endif  // ABOUT_MENU_H