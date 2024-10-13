#include "Sprite.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../Logic.h"
#include "Dispatcher.h"

Sprite_t* Sprite__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(Sprite_t));
}

void Sprite__init(Sprite_t* sprite, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Sprite_t* self = (Sprite_t*)sprite;

  sprite->pos.x = 0.0f;
  sprite->pos.y = 0.0f;
  sprite->pos.z = 0.0f;
  sprite->rot.x = 0.0f;
  sprite->rot.y = 0.0f;
  sprite->rot.z = 0.0f;
  sprite->tex = 0;
  sprite->color = 0xffff0ff;
}