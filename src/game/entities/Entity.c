#include "Entity.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../../lib/Geometry.h"
#include "../../lib/List.h"
#include "../../lib/QuadTree.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "../components/BoxCollider2D.h"
#include "../components/CircleCollider2D.h"
#include "../levels/Level.h"

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
  entity->hurtTime = 0;

  entity->transform.position.x = 0;
  entity->transform.position.y = 0;
  entity->transform.position.z = 0;

  entity->transform.rotation.x = 0;
  entity->transform.rotation.y = 0;
  entity->transform.rotation.z = 0;
  entity->transform.rotation.w = 0;
}

void Entity__render(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Entity_t* self = (Entity_t*)entity;
}

void Entity__gui(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Entity_t* self = (Entity_t*)entity;
}

bool Entity__checkCollide(Entity_t* entity, Engine__State_t* state, f32 x, f32 y) {
  if (NULL == entity->components.collider) return false;

  Logic__State_t* logic = state->local;
  Level_t* level = logic->game->curLvl;

  f32 xxa = entity->transform.position.x + entity->xa;
  f32 yya = entity->transform.position.z + entity->za;

  // use quadtree query to find nearby neighbors
  Rect range = {xxa, yya, 0, 0};
  f32 r0, r1;
  if (BOX_COLLIDER_2D == entity->components.collider->type) {
    BoxCollider2DComponent* collider = (BoxCollider2DComponent*)entity->components.collider;
    range.w = collider->hw, range.h = collider->hh;
    r0 = collider->hw;
  } else if (CIRCLE_COLLIDER_2D == entity->components.collider->type) {
    CircleCollider2DComponent* collider = (CircleCollider2DComponent*)entity->components.collider;
    range.w = collider->r, range.h = collider->r;
    r0 = collider->r;
  }

  u32 matchCount = 0;
  void* matchData[10];  // TODO: don't limit search results?
  // TODO: query can be the radius of the entity, to shorten this code?
  QuadTreeNode_query(level->qt, range, 10, matchData, &matchCount);
  for (u32 i = 0; i < matchCount; i++) {
    Entity_t* other = (Entity_t*)matchData[i];
    if (entity == other) continue;
    if (NULL == other->components.collider) continue;

    if (BOX_COLLIDER_2D == other->components.collider->type) {
      BoxCollider2DComponent* collider = (BoxCollider2DComponent*)other->components.collider;
      r1 = collider->hw;
    } else if (CIRCLE_COLLIDER_2D == other->components.collider->type) {
      CircleCollider2DComponent* collider = (CircleCollider2DComponent*)other->components.collider;
      r1 = collider->r;
    }

    if (NULL != other->components.collider) {
      // TODO: use appropriate check: circle_circle, circle_box, box_box
      bool collisionBefore = CircleCollider2D__check(
          entity->transform.position.x,
          entity->transform.position.z,
          r0,
          other->transform.position.x,
          other->transform.position.z,
          r1);
      bool collisionAfter = CircleCollider2D__check(
          xxa,
          yya,
          r0,
          other->transform.position.x,
          other->transform.position.z,
          r1);

      if (collisionBefore || collisionAfter) {
        // notify each participant (onenter, onstay, onexit)
        OnCollideClosure* params =
            &(OnCollideClosure){entity, other, x, y, collisionBefore, collisionAfter, false};
        Dispatcher__collide(other->collide, other, state, params);  // notify target
        if (params->noclip) return false;
        Dispatcher__collide(entity->collide, entity, state, params);  // notify source
        if (params->noclip) return false;
      }
      if (collisionAfter) return true;
    }
  }

  return false;
}

void Entity__move(Entity_t* entity, Engine__State_t* state) {
  if (fabs(entity->xa) < FLT_MIN) entity->xa = 0;
  if (fabs(entity->za) < FLT_MIN) entity->za = 0;

  if (entity->xa != 0) {
    u32 xSteps = (u32)(fabs(entity->xa * 100) + 1);
    for (u32 i = xSteps; i > 0; i--) {
      f32 xxa = entity->xa;
      f32 xd = xxa * i / xSteps;
      if (entity->transform.position.y > 1 ||  // flying = noclip
          !Entity__checkCollide(
              entity,
              state,
              entity->transform.position.x + xd,
              entity->transform.position.z)) {
        entity->transform.position.x += xd;
        break;
      } else {
        entity->xa = 0;
      }
    }
  }

  if (entity->za != 0) {
    u32 zSteps = (u32)(fabs(entity->za * 100) + 1);
    for (u32 i = zSteps; i > 0; i--) {
      f32 zza = entity->za;
      f32 zd = zza * i / zSteps;
      if (entity->transform.position.y > 1 ||  // flying = noclip
          !Entity__checkCollide(
              entity,
              state,
              entity->transform.position.x,
              entity->transform.position.z + zd)) {
        entity->transform.position.z += zd;
        break;
      } else {
        entity->za = 0;
      }
    }
  }
}

void Entity__tick(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Entity_t* self = (Entity_t*)entity;
}
