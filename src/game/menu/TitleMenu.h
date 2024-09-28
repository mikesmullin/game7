#ifndef TITLE_MENU_H
#define TITLE_MENU_H

#include "../../lib/Engine.h"

Menu_t* TitleMenu__alloc(Arena_t* state);
void TitleMenu__init(Menu_t* menu, Engine__State_t* state);
void TitleMenu__tick(struct Menu_t* menu, void* state);
void TitleMenu__render(struct Menu_t* menu, void* state);

#endif  // TITLE_MENU_H