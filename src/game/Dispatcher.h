#pragma once

typedef struct Engine__State_t Engine__State_t;

typedef enum DispatchFnId {
  BLOCK__TICK,
  BLOCK__RENDER,
  CAT_SPAWN_BLOCK__TICK,
  CAT_SPAWN_BLOCK__RENDER,
  SPAWN_BLOCK__TICK,
  SPAWN_BLOCK__RENDER,
  WALL_BLOCK__TICK,
  WALL_BLOCK__RENDER,

  ENTITY__TICK,
  ENTITY__RENDER,
  CAT_ENTITY__TICK,
  CAT_ENTITY__RENDER,
  PLAYER_ENTITY__TICK,
  PLAYER_ENTITY__RENDER,

  ABOUT_MENU__TICK,
  ABOUT_MENU__RENDER,
  HELP_MENU__TICK,
  HELP_MENU__RENDER,
  TITLE_MENU__TICK,
  TITLE_MENU__RENDER,

  SPRITE__TICK,
  SPRITE__RENDER,
} DispatchFnId;

void Dispatcher__call(DispatchFnId id, void* self, Engine__State_t* state);
