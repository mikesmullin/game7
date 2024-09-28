#ifndef GAME_H
#define GAME_H

typedef struct Arena_t Arena_t;
typedef struct Game_t Game_t;
typedef struct Engine__State_t Engine__State_t;

Game_t* Game__alloc(Arena_t* game);
void Game__init(Game_t* game, Engine__State_t* state);
void Game__tick(Game_t* game, Engine__State_t* state);
void Game__render(Game_t* game, Engine__State_t* state);

#endif  // GAME_H