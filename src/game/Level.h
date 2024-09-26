#ifndef LEVEL_H
#define LEVEL_H

#include "../lib/Base.h"
#include "../lib/Engine.h"

void Level__Load(Engine__State_t* game, u8 id);
void Level__Render(Engine__State_t* game);
void Level__RNG(Engine__State_t* game);

#endif  // LEVEL_H