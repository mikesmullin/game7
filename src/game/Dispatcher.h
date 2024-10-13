#pragma once

#include <stdbool.h>
typedef float f32;
typedef double f64;

typedef struct Engine__State_t Engine__State_t;
typedef struct Entity_t Entity_t;
typedef struct OnCollideClosure_t OnCollideClosure;

typedef enum DispatchFnId {
  DISPATCH_NONE,

  CAT_SPAWN_BLOCK__TICK,
  CAT_SPAWN_BLOCK__GUI,
  SPAWN_BLOCK__TICK,
  WALL_BLOCK__RENDER,

  CAT_ENTITY__TICK,
  CAT_ENTITY__RENDER,
  CAT_ENTITY__GUI,
  CAT_ENTITY__COLLIDE,
  PLAYER_ENTITY__TICK,

  ABOUT_MENU__TICK,
  ABOUT_MENU__RENDER,
  ABOUT_MENU__GUI,
  HELP_MENU__TICK,
  HELP_MENU__RENDER,
  HELP_MENU__GUI,
  TITLE_MENU__TICK,
  TITLE_MENU__RENDER,
  TITLE_MENU__GUI,
} DispatchFnId;

void Dispatcher__engine(DispatchFnId id, void* self, Engine__State_t* state);
void Dispatcher__collide(
    DispatchFnId id, void* self, Engine__State_t* state, OnCollideClosure* params);
