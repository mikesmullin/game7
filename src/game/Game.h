#ifndef GAME_H
#define GAME_H

#include "../lib/Engine.h"

Game_t* Game__alloc(Engine__State_t* game);
void Game__init(Game_t* game, Engine__State_t* state);
void Game__tick(Game_t* game, Engine__State_t* state);
void Game__render(Game_t* game, Engine__State_t* state);

#endif  // GAME_H