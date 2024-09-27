#ifndef TITLEMENU_H
#define TITLEMENU_H

#include "../../lib/Engine.h"

TitleMenu_t* TitleMenu__alloc(Engine__State_t* game);
void TitleMenu__init(TitleMenu_t* menu, Engine__State_t* game);
void TitleMenu__render(TitleMenu_t* menu, Engine__State_t* game);
void TitleMenu__tick(TitleMenu_t* menu, Engine__State_t* game);

#endif  // TITLEMENU_H