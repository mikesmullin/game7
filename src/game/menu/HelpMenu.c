#include "HelpMenu.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Engine.h"
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
  HelpMenu_t* self = (HelpMenu_t*)menu;

  memset(state->local->screen.buf, 0, state->local->screen.len);  // reset black

  char* lines[] = {
				"Use W,A,S,D to move, and",
				"the arrow keys to turn.",
				"",
				"The 1-8 keys select",
				"items from the inventory",
				"",
				"Space uses items",
  };
  for (u8 i=0; i<ARRAY_COUNT(lines); i++) {
    Bitmap__DebugText(
        &state->local->screen,
        &state->local->glyphs0,
        4,
        6 * i + 4,
        0xffffffff,
        0,
        lines[i]);
  }
}

void HelpMenu__tick(struct Menu_t* menu, void* _state) {
  Engine__State_t* state = _state;
  HelpMenu_t* self = (HelpMenu_t*)menu;

  if (state->inputState->use) {
    state->inputState->use = false;

    state->local->game->menu = TitleMenu__alloc(state->arena);
    TitleMenu__init(state->local->game->menu, state);
  }
}