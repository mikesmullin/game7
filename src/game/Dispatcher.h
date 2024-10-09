#pragma once

#include <stdbool.h>
typedef float f32;
typedef double f64;

typedef struct Engine__State_t Engine__State_t;
typedef struct Entity_t Entity_t;
typedef struct OnCollideClosure_t OnCollideClosure;

typedef enum DispatchFnId {
  DISPATCH_NONE,

  BLOCK__TICK,
  BLOCK__RENDER,
  BLOCK__GUI,
  CAT_SPAWN_BLOCK__TICK,
  CAT_SPAWN_BLOCK__RENDER,
  CAT_SPAWN_BLOCK__GUI,
  SPAWN_BLOCK__TICK,
  SPAWN_BLOCK__RENDER,
  SPAWN_BLOCK__GUI,
  WALL_BLOCK__TICK,
  WALL_BLOCK__RENDER,
  WALL_BLOCK__GUI,

  ENTITY__TICK,
  ENTITY__RENDER,
  ENTITY__GUI,
  CAT_ENTITY__TICK,
  CAT_ENTITY__RENDER,
  CAT_ENTITY__GUI,
  CAT_ENTITY__COLLIDE,
  PLAYER_ENTITY__TICK,
  PLAYER_ENTITY__RENDER,
  PLAYER_ENTITY__GUI,

  ABOUT_MENU__TICK,
  ABOUT_MENU__RENDER,
  ABOUT_MENU__GUI,
  HELP_MENU__TICK,
  HELP_MENU__RENDER,
  HELP_MENU__GUI,
  TITLE_MENU__TICK,
  TITLE_MENU__RENDER,
  TITLE_MENU__GUI,

  SPRITE__TICK,
  SPRITE__RENDER,
  SPRITE__GUI,
} DispatchFnId;

void Dispatcher__engine(DispatchFnId id, void* self, Engine__State_t* state);
void Dispatcher__collide(
    DispatchFnId id, void* self, Engine__State_t* state, OnCollideClosure* params);
