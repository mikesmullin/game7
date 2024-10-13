#include "CatEntity.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Color.h"
#include "../../lib/Engine.h"
#include "../../lib/Finger.h"
#include "../../lib/Keyboard.h"
#include "../../lib/List.h"
#include "../../lib/Math.h"
#include "../../lib/QuadTree.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "../components/Collider.h"
#include "../components/Rigidbody2D.h"
#include "../components/SpriteRenderer.h"
#include "../stategraphs/SGCat.h"
#include "../stategraphs/StateGraph.h"
#include "Entity.h"

static const f32 CAT_MOVE_SPEED = 0.1f;  // per-second
static f32 lastTurnWait = 2.0f;
static f32 sinceLastTurn = 0;

Entity_t* CatEntity__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(CatEntity_t));
}

void CatEntity__init(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;

  Entity__init(entity, state);

  entity->engine->tick = CAT_ENTITY__TICK;
  entity->engine->render = CAT_ENTITY__RENDER;
  entity->engine->gui = CAT_ENTITY__GUI;
  entity->tags1 |= TAG_CAT;

  entity->tform->pos.x = 0.0f;
  entity->tform->pos.y = 0.0f;
  entity->tform->pos.z = 0.0f;

  entity->tform->rot.x = 0.0f;
  entity->tform->rot.y = 0.0f;
  entity->tform->rot.z = 0.0f;

  self->xa = Math__random(-1, 1);
  self->za = Math__random(-1, 1);

  self->tx = Math__urandom2(0, 7);

  CircleCollider2DComponent* collider =
      Arena__Push(state->arena, sizeof(CircleCollider2DComponent));
  collider->base.type = CIRCLE_COLLIDER_2D;
  collider->r = 0.5f;
  collider->base.collide = CAT_ENTITY__COLLIDE;
  entity->collider = (ColliderComponent*)collider;

  entity->render = Arena__Push(state->arena, sizeof(RendererComponent));
  entity->render->type = SPRITE_RENDERER;
  entity->render->atlas = ATLAS_TEXTURE;
  entity->render->tx = self->tx;
  entity->render->ty = 1;
  entity->render->useMask = true;
  entity->render->mask = BLACK;
  entity->render->color = TRANSPARENT;

  self->sg = Arena__Push(state->arena, sizeof(StateGraph));
  self->sg->currentState = &idle;
  self->sg->entity = entity;

  entity->audio = Arena__Push(state->arena, sizeof(AudioSourceComponent));
}

void CatEntity__render(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;

  SpriteRenderer__render(entity, state);
}

void CatEntity__gui(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;

  Bitmap__DebugText2(
      state,
      6,
      6 * 8,
      WHITE,
      TRANSPARENT,
      "frame %d tx %d ty %d",
      self->sg->currentState->frame,
      entity->render->tx,
      entity->render->ty);
}

void CatEntity__tick(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;

  sinceLastTurn += state->deltaTime;
  if (sinceLastTurn > lastTurnWait) {
    sinceLastTurn = 0;
  }

  // entity->rb->xa = self->xa * CAT_MOVE_SPEED;
  // entity->rb->za = self->za * CAT_MOVE_SPEED;

  StateGraph__tick(self->sg);

  Rigidbody2D__move(entity, state);
}

void CatEntity__collide(Entity_t* entity, Engine__State_t* state, OnCollideClosure* params) {
  CatEntity_t* self = (CatEntity_t*)entity;

  if (params->after) {  // onenter, onstay
    if (TAG_CAT & params->target->tags1) {
      params->noclip = true;
    } else {
      self->xa = Math__random(-1, 1);
      self->za = Math__random(-1, 1);
    }
  }
}