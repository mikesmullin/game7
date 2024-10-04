#include "CatEntity.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../../lib/Finger.h"
#include "../../lib/Keyboard.h"
#include "../../lib/Math.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "Entity.h"

static const f32 CAT_FLY_SPEED = 0.02f;  // per-second

Entity_t* CatEntity__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(CatEntity_t));
}

void CatEntity__init(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;

  Entity__init(entity, state);

  entity->tick = CAT_ENTITY__TICK;
  entity->render = CAT_ENTITY__RENDER;

  entity->transform.position.x = 0.0f;
  entity->transform.position.y = 0.0f;
  entity->transform.position.z = 0.0f;

  entity->transform.rotation.x = 0.0f;
  entity->transform.rotation.y = 0.0f;
  entity->transform.rotation.z = 0.0f;
  entity->transform.rotation.w = 0.0f;

  self->xa = Math__random(-1, 1);
  self->ya = Math__random(-1, 1);
  self->za = Math__random(-1, 1);
}

void CatEntity__render(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;

  Bitmap3D__RenderSprite(
      state,
      entity->transform.position.x,
      entity->transform.position.y,
      entity->transform.position.z,
      5,
      0xff000000);
}

void CatEntity__gui(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;
}

void CatEntity__tick(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;

  entity->transform.position.x += self->xa * CAT_FLY_SPEED;
  entity->transform.position.y += self->ya * CAT_FLY_SPEED;
  entity->transform.position.z += self->za * CAT_FLY_SPEED;
}