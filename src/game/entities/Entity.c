#include "Entity.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../Logic.h"

Entity_t* Entity__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(Entity_t));
}

void Entity__init(Entity_t* entity, Engine__State_t* state) {
  entity->tick = ENTITY__TICK;
  entity->render = ENTITY__RENDER;
}

void Entity__render(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Entity_t* self = (Entity_t*)entity;
}

void Entity__tick(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Entity_t* self = (Entity_t*)entity;
}
