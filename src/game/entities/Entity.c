#include "Entity.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../../lib/List.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "../levels/Level.h"

Entity_t* Entity__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(Entity_t));
}

void Entity__init(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  entity->id = ++logic->game->lastUid;

  entity->tform = Arena__Push(state->arena, sizeof(TransformComponent));
  entity->tform->pos.x = 0;
  entity->tform->pos.y = 0;
  entity->tform->pos.z = 0;
  entity->tform->rot.x = 0;
  entity->tform->rot.y = 0;
  entity->tform->rot.z = 0;

  entity->rb = Arena__Push(state->arena, sizeof(Rigidbody2DComponent));
  entity->rb->xa = 0;
  entity->rb->za = 0;

  entity->engine = Arena__Push(state->arena, sizeof(EngineComponent));
  entity->engine->tick = DISPATCH_NONE;
  entity->engine->render = DISPATCH_NONE;
  entity->engine->gui = DISPATCH_NONE;
}
