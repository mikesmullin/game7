#include "TitleMenu.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Engine.h"
#include "../../lib/Keyboard.h"
#include "../../lib/List.h"
#include "../../lib/String.h"
#include "../Logic.h"
#include "../levels/Level.h"
#include "AboutMenu.h"
#include "HelpMenu.h"

Menu_t* TitleMenu__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(TitleMenu_t));
}

void TitleMenu__init(Menu_t* menu, Engine__State_t* state) {
  TitleMenu_t* self = (TitleMenu_t*)menu;

  menu->tick = &TitleMenu__tick;
  menu->render = &TitleMenu__render;

  state->Vulkan__FReadImage(&self->bmp, "../assets/textures/title.png");

  self->options = List__alloc(state->arena);
  List__append(state->arena, self->options, "New game");
  List__append(state->arena, self->options, "Help");
  List__append(state->arena, self->options, "About");
  self->selection = 0;
  self->playedAudio = false;
}

void TitleMenu__render(struct Menu_t* menu, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  TitleMenu_t* self = (TitleMenu_t*)menu;

  Bitmap__Draw(&self->bmp, &logic->screen, 0, 0);

  List__Node_t* c = self->options->head;
  for (u8 i = 0; i < self->options->len; i++) {
    u32 color = 0xff909090;
    if (self->selection == i) {
      color = 0xff80ffff;
    }

    Bitmap__DebugText(&logic->screen, &logic->glyphs0, 20, 6 * i + 120, color, 0, c->data);
    c = c->next;
  }

  Bitmap__DebugText(
      &logic->screen,
      &logic->glyphs0,
      4,
      6 * 29,
      0xffffffff,
      0,
      "Copyright (C) 2024 Mike Smullin");
}

void TitleMenu__tick(struct Menu_t* menu, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  TitleMenu_t* self = (TitleMenu_t*)menu;

  if (!self->playedAudio) {
    self->playedAudio = true;
    state->Audio__StopAudio(state->audio, AUDIO_TITLE);
    state->Audio__ResumeAudio(state->audio, AUDIO_TITLE, false, 1.0f);
  }
  if (state->kbState->fwd && self->selection > 0) {
    self->selection--;
    state->Audio__StopAudio(state->audio, AUDIO_CLICK);
    state->Audio__ResumeAudio(state->audio, AUDIO_CLICK, false, 1.0f);
  }
  if (state->kbState->back && self->selection < self->options->len - 1) {
    self->selection++;
    state->Audio__StopAudio(state->audio, AUDIO_CLICK);
    state->Audio__ResumeAudio(state->audio, AUDIO_CLICK, false, 1.0f);
  }
  if (state->kbState->use) {
    state->kbState->use = false;
    state->Audio__StopAudio(state->audio, AUDIO_POWERUP);
    state->Audio__ResumeAudio(state->audio, AUDIO_POWERUP, false, 1.0f);

    if (0 == self->selection) {
      logic->game->menu = NULL;
      logic->game->curLvl = Level__alloc(state->arena);
      Level__init(state->arena, logic->game->curLvl, state);
      logic->game->curLvl->wallCol = 0x330000ff;
      Level__load(logic->game->curLvl, state, "../assets/textures/level1.png");
    } else if (1 == self->selection) {
      logic->game->menu = HelpMenu__alloc(state->arena);
      HelpMenu__init(logic->game->menu, state);
    } else if (2 == self->selection) {
      logic->game->menu = AboutMenu__alloc(state->arena);
      AboutMenu__init(logic->game->menu, state);
    }
  }

  state->kbState->fwd = false;
  state->kbState->back = false;
}
