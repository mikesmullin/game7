#include "Game.h"

#include "../lib/Arena.h"
#include "../lib/Bitmap.h"
#include "../lib/Bitmap3D.h"
#include "../lib/Engine.h"
#include "../lib/Math.h"
#include "Level.h"
#include "menu/TitleMenu.h"

Game_t* Game__alloc(Engine__State_t* game) {
  Game_t* g = Arena__Push(game->arena, sizeof(Game_t));
  return g;
}

void Game__init(Game_t* game, Engine__State_t* state) {
  game->menu = TitleMenu__alloc(state);
  TitleMenu__init((Menu_t*)game->menu, state);
}

void Game__tick(Game_t* game, Engine__State_t* state) {
  if (NULL == game->menu) {
    return;
  }
  game->menu->tick(
      game->menu,
      state->g_Keyboard__state->wKey,
      state->g_Keyboard__state->sKey,
      state->g_Keyboard__state->aKey,
      state->g_Keyboard__state->dKey,
      state->g_Keyboard__state->eKey,
      state);
}

void Game__render(Game_t* game, Engine__State_t* state) {
  // TODO: need for polymorphism? vtables?
  if (NULL != game->menu) {  // title screen
    game->menu->render(game->menu, &state->local->screen);
  } else {  // in-game
    Bitmap3D__RenderHorizon(state);
    Level__Render(state);
    Bitmap3D__PostProcessing(state);

    // game->local->CANVAS_DEBUG_X = 80;
    // game->local->CANVAS_DEBUG_Y = 40;

    // draw debug cursor
    Bitmap__Set2DPixel(
        &state->local->screen,
        state->local->CANVAS_DEBUG_X,
        state->local->CANVAS_DEBUG_Y,
        Math__urandom() | 0xffff0000 + 0xff993399);

    Bitmap__DebugText(
        &state->local->screen,
        &state->local->glyphs0,
        4,
        6 * 29,
        0xffffffff,
        0,
        "cam x %+06.1f y %+06.1f z %+06.1f r %+06.1f",
        state->local->player.transform.position[0],
        state->local->player.transform.position[1],
        state->local->player.transform.position[2],
        state->local->player.transform.rotation[0]);
  }
}