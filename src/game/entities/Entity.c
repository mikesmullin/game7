#include "Entity.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../../lib/List.h"
#include "../Dispatcher.h"
#include "../Logic.h"

Entity_t* Entity__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(Entity_t));
}

void Entity__init(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  entity->tick = ENTITY__TICK;
  entity->render = ENTITY__RENDER;
  entity->gui = ENTITY__GUI;

  // entity->level = logic->game->curLvl;
  // entity->sprites = List__alloc(state->arena);

  entity->id = 0;
  entity->flying = false;
  entity->removed = false;
  entity->hurtTime = 0;
  entity->dead = false;

  entity->transform.position.x = 0;
  entity->transform.position.y = 0;
  entity->transform.position.z = 0;

  entity->transform.rotation.x = 0;
  entity->transform.rotation.y = 0;
  entity->transform.rotation.z = 0;
  entity->transform.rotation.w = 0;
}

void Entity__render(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Entity_t* self = (Entity_t*)entity;
}

void Entity__gui(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Entity_t* self = (Entity_t*)entity;
}

void Entity__tick(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Entity_t* self = (Entity_t*)entity;
}
