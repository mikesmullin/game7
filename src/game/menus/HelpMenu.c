#include "HelpMenu.h"

#include <string.h>

#include "../../lib/Arena.h"
#include "../../lib/Array.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Engine.h"
#include "../../lib/Keyboard.h"
#include "../Logic.h"
#include "TitleMenu.h"

Menu_t* HelpMenu__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(HelpMenu_t));
}

void HelpMenu__init(Menu_t* menu, Engine__State_t* state) {
  menu->tick = &HelpMenu__tick;
  menu->render = &HelpMenu__render;
}

void HelpMenu__render(struct Menu_t* menu, void* _state) {
  Engine__State_t* state = _state;
  Logic__State_t* logic = state->local;
  HelpMenu_t* self = (HelpMenu_t*)menu;

  memset(logic->screen.buf, 0, logic->screen.len);  // reset black

  char* lines[] = {
      "Use W,A,S,D to move, and",
      "the arrow keys to turn.",
      "",
      "The 1-8 keys select",
      "items from the inventory",
      "",
      "Space uses items",
  };
  for (u8 i = 0; i < ARRAY_COUNT(lines); i++) {
    Bitmap__DebugText(&logic->screen, &logic->glyphs0, 4, 6 * i + 4, 0xffffffff, 0, lines[i]);
  }
}

void HelpMenu__tick(struct Menu_t* menu, void* _state) {
  Engine__State_t* state = _state;
  Logic__State_t* logic = state->local;
  HelpMenu_t* self = (HelpMenu_t*)menu;

  if (state->inputState->use) {
    state->inputState->use = false;

    logic->game->menu = TitleMenu__alloc(state->arena);
    TitleMenu__init(logic->game->menu, state);
  }
}