#include "Sprite.h"

#include "../lib/Arena.h"
#include "../lib/Engine.h"
#include "Logic.h"

Sprite_t* Sprite__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(Sprite_t));
}

void Sprite__init(Sprite_t* sprite, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Sprite_t* self = (Sprite_t*)sprite;

  sprite->tick = &Sprite__tick;
  sprite->render = &Sprite__render;
  sprite->transform.position[0] = 0.0f;
  sprite->transform.position[1] = 0.0f;
  sprite->transform.position[1] = 0.0f;
  sprite->transform.rotation[0] = 0.0f;
  sprite->transform.rotation[1] = 0.0f;
  sprite->transform.rotation[2] = 0.0f;
  sprite->transform.rotation[3] = 0.0f;
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