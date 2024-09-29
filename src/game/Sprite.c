#include "Sprite.h"

#include "../lib/Arena.h"
#include "../lib/Engine.h"
#include "Dispatcher.h"
#include "Logic.h"


Sprite_t* Sprite__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(Sprite_t));
}

void Sprite__init(Sprite_t* sprite, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Sprite_t* self = (Sprite_t*)sprite;

  sprite->tick = SPRITE__TICK;
  sprite->render = SPRITE__RENDER;
  sprite->transform.position.x = 0.0f;
  sprite->transform.position.y = 0.0f;
  sprite->transform.position.z = 0.0f;
  sprite->transform.rotation.x = 0.0f;
  sprite->transform.rotation.y = 0.0f;
  sprite->transform.rotation.z = 0.0f;
  sprite->transform.rotation.w = 0.0f;
  sprite->tex = 0;
  sprite->color = 0xffff0ff;
  sprite->removed = false;
}

void Sprite__render(struct Sprite_t* sprite, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Sprite_t* self = (Sprite_t*)sprite;
}

void Sprite__tick(struct Sprite_t* sprite, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Sprite_t* self = (Sprite_t*)sprite;
}