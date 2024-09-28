#include "Game.h"

#include "../lib/Arena.h"
#include "../lib/Bitmap.h"
#include "../lib/Bitmap3D.h"
#include "../lib/Engine.h"
#include "../lib/Math.h"
#include "Level.h"
#include "Logic.h"
#include "menus/TitleMenu.h"

Game_t* Game__alloc(Arena_t* arena) {
  Game_t* g = Arena__Push(arena, sizeof(Game_t));
  return g;
}

void Game__init(Game_t* game, Engine__State_t* state) {
  game->menu = TitleMenu__alloc(state->arena);
  TitleMenu__init(game->menu, state);
}

void Game__tick(Game_t* game, Engine__State_t* state) {
  if (NULL == game->menu) {
    return;
  }
  game->menu->tick(game->menu, state);
}

void Game__render(Game_t* game, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  // menu system
  if (NULL != game->menu) {
    game->menu->render(game->menu, state);
  }

  // in-game
  else {
    Bitmap3D__RenderHorizon(state);
    Level__Render(state);
    Bitmap3D__PostProcessing(state);

    // game->local->CANVAS_DEBUG_X = 80;
    // game->local->CANVAS_DEBUG_Y = 40;

    // draw debug cursor
    Bitmap__Set2DPixel(
        &logic->screen,
        logic->CANVAS_DEBUG_X,
        logic->CANVAS_DEBUG_Y,
        Math__urandom() | 0xffff0000 + 0xff993399);

    Bitmap__DebugText(
        &logic->screen,
        &logic->glyphs0,
        4,
        6 * 29,
        0xffffffff,
        0,
        "cam x %+06.1f y %+06.1f z %+06.1f r %+06.1f",
        logic->player->transform.position[0],
        logic->player->transform.position[1],
        logic->player->transform.position[2],
        logic->player->transform.rotation[0]);
  }
}