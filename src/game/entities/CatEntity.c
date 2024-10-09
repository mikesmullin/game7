#include "CatEntity.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../../lib/Finger.h"
#include "../../lib/Keyboard.h"
#include "../../lib/List.h"
#include "../../lib/Log.h"
#include "../../lib/Math.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "Entity.h"

static const f32 CAT_FLY_SPEED = 0.1f;  // per-second

Entity_t* CatEntity__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(CatEntity_t));
}

void CatEntity__init(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;

  Entity__init(entity, state);

  entity->tick = CAT_ENTITY__TICK;
  entity->render = CAT_ENTITY__RENDER;
  entity->collide = CAT_ENTITY__COLLIDE;

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

  self->tx = Math__urandom2(0, 7);

  BoxCollider2DComponent* collider = Arena__Push(state->arena, sizeof(BoxCollider2DComponent));
  collider->base.type = BOX_COLLIDER_2D;
  entity->components.collider = (ColliderComponent*)collider;
}

void CatEntity__render(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;

  Bitmap3D__RenderSprite(
      state,
      entity->transform.position.x,
      entity->transform.position.y,
      entity->transform.position.z,
      self->tx,
      1,
      0x00000000);
}

void CatEntity__gui(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;
}

bool BoxCollider2D__check(f32 x0, f32 y0, f32 r0, f32 x1, f32 y1, f32 r1) {
  // TODO: use component transforms (then everywhere)

  // Check for overlap using AABB (Axis-Aligned Bounding Box) logic
  // based on projected a position, and existing b position
  bool overlap_x = (x0 - r0 < x1 + r1) && (x0 + r0 > x1 - r1);
  bool overlap_y = (y0 - r0 < y1 + r1) && (y0 + r0 > y1 - r1);

  // If both x and y axes overlap, a collision is detected
  if (overlap_x && overlap_y) {
    return true;
  }

  // No collision detected, movement allowed
  return false;
}

static void turn_around(CatEntity_t* self) {
  self->xa = Math__random(-1, 1);
  self->ya = Math__random(-1, 1);
  self->za = Math__random(-1, 1);
}

void CatEntity__tick(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;

  static f32 lastTurnWait = 3.0f;
  static f32 sinceLastTurn = 0;
  sinceLastTurn += state->deltaTime;

  entity->xa = 0;
  entity->za = 0;
  entity->xa += self->xa * CAT_FLY_SPEED;
  // entity->ya += self->ya * CAT_FLY_SPEED;
  entity->za += self->za * CAT_FLY_SPEED;

  // TODO: move to Level
  Level_t* level = logic->game->curLvl;
  bool collision = false;

  List__Node_t* c = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity_t* other = c->data;
    if (entity == other) continue;
    bool r = BoxCollider2D__check(
        entity->transform.position.x + entity->xa,
        entity->transform.position.z + entity->za,
        entity->r,
        other->transform.position.x,
        other->transform.position.z,
        other->r);
    if (r) {
      if (sinceLastTurn > lastTurnWait) {
        sinceLastTurn = 0;
        turn_around(self);
      }
      collision = true;
      break;
      // return;
    }
    c = c->next;
  }

  if (!collision) {
    c = level->blocks->head;
    for (u32 i = 0; i < level->blocks->len; i++) {
      Block_t* other = c->data;
      if (entity == (void*)other) continue;
      if (!other->blocking) continue;
      bool r = BoxCollider2D__check(
          entity->transform.position.x + entity->xa,
          entity->transform.position.z + entity->za,
          entity->r,
          other->x,
          other->y,
          other->r);
      if (r) {
        if (sinceLastTurn > lastTurnWait) {
          sinceLastTurn = 0;
          turn_around(self);
        }
        collision = true;
        break;
        // return;
      }
      c = c->next;
    }
  }

  if (collision) {
  } else {
    entity->transform.position.x += entity->xa;
    entity->transform.position.z += entity->za;
  }
  entity->xa = 0;
  entity->za = 0;

  // Entity__move(entity, state);
}

bool CatEntity__collide(Entity_t* entity, Engine__State_t* state, Entity_t* other, f64 x, f64 y) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;

  bool collision = Entity__collide(entity, state, other, x, y);
  if (collision) {
    turn_around(self);
  }
  return collision;
}
