#include "TitleMenu.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Engine.h"
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

  self->options = str8n__allocf(state->arena, NULL, "New game", 9);
  self->options = str8n__allocf(state->arena, self->options, "Help", 5);
  self->options = str8n__allocf(state->arena, self->options, "About", 6);
  self->optionsLength = 3;
  self->selection = 0;
  self->playedAudio = false;
}

void TitleMenu__render(struct Menu_t* menu, void* _state) {
  Engine__State_t* state = _state;
  TitleMenu_t* self = (TitleMenu_t*)menu;

  Bitmap__Draw(&self->bmp, &state->local->screen, 0, 0);

  u8 i;
  String8Node* c;
  for (i = 0, c = self->options; i < self->optionsLength; i++, c = c->next) {
    u32 color = 0xff909090;
    if (self->selection == i) {
      color = 0xff80ffff;
    }

    Bitmap__DebugText(
        &state->local->screen,
        &state->local->glyphs0,
        20,
        6 * i + 120,
        color,
        0,
        c->string->str);
  }

  Bitmap__DebugText(
      &state->local->screen,
      &state->local->glyphs0,
      4,
      6 * 29,
      0xffffffff,
      0,
      "Copyright (C) 2024 Mike Smullin");
}

void TitleMenu__tick(struct Menu_t* menu, void* _state) {
  Engine__State_t* state = _state;
  TitleMenu_t* self = (TitleMenu_t*)menu;

  if (!self->playedAudio) {
    self->playedAudio = true;
    state->Audio__StopAudio(AUDIO_TITLE);
    state->Audio__ResumeAudio(AUDIO_TITLE, false, 1.0f);
  }
  if (state->inputState->fwd && self->selection > 0) {
    self->selection--;
    state->Audio__StopAudio(AUDIO_CLICK);
    state->Audio__ResumeAudio(AUDIO_CLICK, false, 1.0f);
  }
  if (state->inputState->back && self->selection < self->optionsLength - 1) {
    self->selection++;
    state->Audio__StopAudio(AUDIO_CLICK);
    state->Audio__ResumeAudio(AUDIO_CLICK, false, 1.0f);
  }
  if (state->inputState->use) {
    state->inputState->use = false;
    state->Audio__StopAudio(AUDIO_POWERUP);
    state->Audio__ResumeAudio(AUDIO_POWERUP, false, 1.0f);

    if (0 == self->selection) {
      state->local->game->menu = NULL;
    } else if (1 == self->selection) {
      state->local->game->menu = HelpMenu__alloc(state->arena);
      HelpMenu__init(state->local->game->menu, state);
    } else if (2 == self->selection) {
      state->local->game->menu = AboutMenu__alloc(state->arena);
      AboutMenu__init(state->local->game->menu, state);
    }
  }

  state->inputState->fwd = false;
  state->inputState->back = false;
}
