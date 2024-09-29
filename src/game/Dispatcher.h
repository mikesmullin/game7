#pragma once

typedef struct Engine__State_t Engine__State_t;

typedef enum DispatchFnId {
  BLOCK__RENDER,
  BLOCK__TICK,
  BAT_SPAWN_BLOCK__RENDER,
  BAT_SPAWN_BLOCK__TICK,
  SPAWN_BLOCK__RENDER,
  SPAWN_BLOCK__TICK,
  WALL_BLOCK__RENDER,
  WALL_BLOCK__TICK,

  ENTITY__RENDER,
  ENTITY__TICK,
  BAT_ENTITY__RENDER,
  BAT_ENTITY__TICK,
  PLAYER_ENTITY__RENDER,
  PLAYER_ENTITY__TICK,

  ABOUT_MENU__RENDER,
  ABOUT_MENU__TICK,
  HELP_MENU__RENDER,
  HELP_MENU__TICK,
  TITLE_MENU__RENDER,
  TITLE_MENU__TICK,

  SPRITE__RENDER,
  SPRITE__TICK,
} DispatchFnId;

void Dispatcher__call(DispatchFnId id, void* self, Engine__State_t* state);
