#ifndef TITLEMENU_H
#define TITLEMENU_H

#include "../../lib/Engine.h"

Menu_t* TitleMenu__alloc(Engine__State_t* state);
void TitleMenu__init(Menu_t* menu, Engine__State_t* state);
void TitleMenu__tick(
    struct Menu_t* menu, bool up, bool down, bool left, bool right, bool use, void* state);
void TitleMenu__render(struct Menu_t* menu, Bitmap_t* target);

#endif  // TITLEMENU_H