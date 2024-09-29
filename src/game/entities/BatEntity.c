#include "BatEntity.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../../lib/Finger.h"
#include "../../lib/Keyboard.h"
#include "../../lib/Math.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "Entity.h"

static const f32 BAT_FLY_SPEED = 0.5f;  // per-second

Entity_t* BatEntity__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(BatEntity_t));
}

void BatEntity__init(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  BatEntity_t* self = (BatEntity_t*)entity;

  Entity__init(entity, state);

  entity->tick = BAT_ENTITY__TICK;
  entity->render = BAT_ENTITY__RENDER;

  entity->transform.position.x = 0.0f;
  entity->transform.position.y = 0.0f;
  entity->transform.position.z = 0.0f;

  entity->transform.rotation.x = 0.0f;
  entity->transform.rotation.y = 0.0f;
  entity->transform.rotation.z = 0.0f;
  entity->transform.rotation.w = 0.0f;
}

void BatEntity__render(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  BatEntity_t* self = (BatEntity_t*)entity;

  Bitmap3D__RenderSprite(
      state,
      entity->transform.position.x,
      entity->transform.position.y,
      entity->transform.position.z,
      5,
      0xff000000);
}

void BatEntity__tick(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  BatEntity_t* self = (BatEntity_t*)entity;
}