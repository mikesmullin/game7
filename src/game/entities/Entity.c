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

  entity->tick = ENTITY__TICK;
  entity->render = ENTITY__RENDER;
  entity->gui = ENTITY__GUI;
  entity->collide = ENTITY__COLLIDE;

  // entity->level = logic->game->curLvl;
  // entity->sprites = List__alloc(state->arena);

  entity->id = 0;
  entity->flying = false;
  entity->removed = false;
  entity->hurtTime = 0;
  entity->dead = false;
  entity->r = 0.3f;

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
  Logic__State_t* logic = state->local;
  Level_t* level = logic->game->curLvl;

  s32 x0 = (s32)(floorf(x + 0.5 - entity->r));
  s32 x1 = (s32)(floorf(x + 0.5 + entity->r));
  s32 y0 = (s32)(floorf(y + 0.5 - entity->r));
  s32 y1 = (s32)(floorf(y + 0.5 + entity->r));

  // blocks can only collide while on ground
  if (entity->transform.position.y < 1) {
    // blocks are stationary;
    // a simple neighbor check
    Block_t* b;
    b = Level__getBlock(level, x0, y0);
    if (NULL != b && Dispatcher__collide(b->collide, b, state, entity, x, y)) {
      Dispatcher__collide(entity->collide, entity, state, b, x, y);
      return true;
    }
    b = Level__getBlock(level, x1, y0);
    if (NULL != b && Dispatcher__collide(b->collide, b, state, entity, x, y)) {
      Dispatcher__collide(entity->collide, entity, state, b, x, y);
      return true;
    }
    b = Level__getBlock(level, x0, y1);
    if (NULL != b && Dispatcher__collide(b->collide, b, state, entity, x, y)) {
      Dispatcher__collide(entity->collide, entity, state, b, x, y);
      return true;
    }
    b = Level__getBlock(level, x1, y1);
    if (NULL != b && Dispatcher__collide(b->collide, b, state, entity, x, y)) {
      Dispatcher__collide(entity->collide, entity, state, b, x, y);
      return true;
    }
  }

  // however entities can move around;
  // we must visit all entities
  List__Node_t* c = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity_t* e = c->data;
    c = c->next;
    if (e == entity) continue;  // no self-intersection

    // check & notify each entity
    bool collision = false;
    collision &= Dispatcher__collide(e->collide, e, state, entity, x, y);
    collision &= Dispatcher__collide(entity->collide, entity, state, e, x, y);
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
      if (!Entity__checkCollide(
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
      if (!Entity__checkCollide(
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

bool Entity__collide(Entity_t* self, Engine__State_t* state, Entity_t* other, f64 x, f64 y) {
  // Check for overlap using AABB (Axis-Aligned Bounding Box) logic
  bool overlap_x = (self->transform.position.x - self->r < x + other->r) &&
                   (self->transform.position.x + self->r > x - other->r);
  bool overlap_y = (self->transform.position.y - self->r < y + other->r) &&
                   (self->transform.position.y + self->r > y - other->r);

  // If both x and y axes overlap, a collision is detected
  if (overlap_x && overlap_y) {
    return true;
  }

  // No collision detected, movement allowed
  return false;
}
