#include <string.h>

#include "../../lib/Arena.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Engine.h"
#include "../../lib/Keyboard.h"
#include "../Logic.h"
#include "TitleMenu.h"

Menu_t* AboutMenu__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(AboutMenu_t));
}

void AboutMenu__render(struct Menu_t* menu, void* _state);
void AboutMenu__tick(struct Menu_t* menu, void* _state);

void AboutMenu__init(Menu_t* menu, Engine__State_t* state) {
  menu->tick = &AboutMenu__tick;
  menu->render = &AboutMenu__render;
}

void AboutMenu__render(struct Menu_t* menu, void* _state) {
  Engine__State_t* state = _state;
  Logic__State_t* logic = state->local;
  AboutMenu_t* self = (AboutMenu_t*)menu;

  memset(logic->screen.buf, 0, logic->screen.len);  // reset black

  Bitmap__DebugText(
      &logic->screen,
      &logic->glyphs0,
      4,
      6 * 29,
      0xffffffff,
      0,
      "Made by Mike Smullin");
}

void AboutMenu__tick(struct Menu_t* menu, void* _state) {
  Engine__State_t* state = _state;
  Logic__State_t* logic = state->local;
  AboutMenu_t* self = (AboutMenu_t*)menu;

  if (state->inputState->use) {
    state->inputState->use = false;

    // TODO: reuse existing TitleMenu instance like a singleton, to avoid memory leak
    logic->game->menu = TitleMenu__alloc(state->arena);
    TitleMenu__init(logic->game->menu, state);
  }
}