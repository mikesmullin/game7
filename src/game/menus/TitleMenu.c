#include "TitleMenu.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Engine.h"
#include "../../lib/Keyboard.h"
#include "../../lib/List.h"
#include "../../lib/String.h"
#include "../Logic.h"
#include "../levels/Level.h"
#include "../utils/Dispatcher.h"
#include "AboutMenu.h"
#include "HelpMenu.h"

Menu_t* TitleMenu__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(TitleMenu_t));
}

void TitleMenu__init(Menu_t* menu, Engine__State_t* state) {
  TitleMenu_t* self = (TitleMenu_t*)menu;

  menu->tick = TITLE_MENU__TICK;
  // menu->render = TITLE_MENU__RENDER;
  menu->gui = TITLE_MENU__GUI;

  state->Vulkan__FReadImage(&self->bmp, "../assets/textures/title.png");

  self->options = List__alloc(state->arena);
  List__append(state->arena, self->options, String__alloc(state->arena, "New game"));
  List__append(state->arena, self->options, String__alloc(state->arena, "Help"));
  List__append(state->arena, self->options, String__alloc(state->arena, "About"));
  self->selection = 0;
  self->playedAudio = false;
}

void TitleMenu__render(struct Menu_t* menu, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  TitleMenu_t* self = (TitleMenu_t*)menu;
}

void TitleMenu__gui(struct Menu_t* menu, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  TitleMenu_t* self = (TitleMenu_t*)menu;

  Bitmap__Fill(&logic->screen, 0, 0, logic->screen.w, logic->screen.h, 0xff000000);  // wipe black
  Bitmap__Draw(&self->bmp, &logic->screen, 0, 0);

  List__Node_t* c = self->options->head;
  for (u8 i = 0; i < self->options->len; i++) {
    u32 color = 0xff909090;
    if (self->selection == i) {
      color = 0xff80ffff;
    }

    String_t* str = c->data;
    Bitmap__DebugText(&logic->screen, &logic->glyphs0, 20, 6 * i + 120, color, 0, str->str);
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
    state->Audio__ResumeAudio(state->audio, AUDIO_TITLE, false, 1.0f, 0);
  }
  if (state->kbState->fwd && self->selection > 0) {
    self->selection--;
    state->Audio__StopAudio(state->audio, AUDIO_CLICK);
    state->Audio__ResumeAudio(state->audio, AUDIO_CLICK, false, 1.0f, 0);
  }
  if (state->kbState->back && self->selection < self->options->len - 1) {
    self->selection++;
    state->Audio__StopAudio(state->audio, AUDIO_CLICK);
    state->Audio__ResumeAudio(state->audio, AUDIO_CLICK, false, 1.0f, 0);
  }
  if (self->skip || state->kbState->use || state->kbState->up) {
    state->kbState->use = false;
    state->kbState->up = false;
    state->Audio__StopAudio(state->audio, AUDIO_POWERUP);
    state->Audio__ResumeAudio(state->audio, AUDIO_POWERUP, false, 1.0f, 0);

    if (self->skip || 0 == self->selection) {
      logic->game->menu = NULL;
      Level_t* level = Level__alloc(state->arena);
      Level__init(state->arena, level, state);
      level->width = 300;
      level->depth = 300;
      level->height = 10;
      level->wallTex = 5;
      level->wallCol = 0x66ff0000;
      level->skybox = true;
      // level->ceilCol = 0x77000022;  // blood red
      level->ceilCol = 0xaa000000;  // darken
      logic->game->curLvl = level;
      Level__load(level, state, "../assets/textures/level1.png", "../assets/textures/sky.png");
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
