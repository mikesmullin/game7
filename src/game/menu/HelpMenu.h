#ifndef HELP_MENU_H
#define HELP_MENU_H

#include "../../lib/Engine.h"

Menu_t* HelpMenu__alloc(Arena_t* state);
void HelpMenu__init(Menu_t* menu, Engine__State_t* state);
void HelpMenu__tick(struct Menu_t* menu, void* state);
void HelpMenu__render(struct Menu_t* menu, void* state);

#endif  // HELP_MENU_H