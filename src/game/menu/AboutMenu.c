#include "AboutMenu.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Engine.h"
#include "TitleMenu.h"

Menu_t* AboutMenu__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(AboutMenu_t));
}

void AboutMenu__init(Menu_t* menu, Engine__State_t* state) {
  menu->tick = &AboutMenu__tick;
  menu->render = &AboutMenu__render;
}

void AboutMenu__render(struct Menu_t* menu, void* _state) {
  Engine__State_t* state = _state;
  AboutMenu_t* self = (AboutMenu_t*)menu;

  memset(state->local->screen.buf, 0, state->local->screen.len);  // reset black

  Bitmap__DebugText(
      &state->local->screen,
      &state->local->glyphs0,
      4,
      6 * 29,
      0xffffffff,
      0,
      "Made by Mike Smullin");
}

void AboutMenu__tick(struct Menu_t* menu, void* _state) {
  Engine__State_t* state = _state;
  AboutMenu_t* self = (AboutMenu_t*)menu;

  if (state->inputState->use) {
    state->inputState->use = false;

    // TODO: reuse existing TitleMenu instance like a singleton, to avoid memory leak
    state->local->game->menu = TitleMenu__alloc(state->arena);
    TitleMenu__init(state->local->game->menu, state);
  }
}