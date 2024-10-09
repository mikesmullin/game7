#include "CatEntity.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../../lib/Finger.h"
#include "../../lib/Geometry.h"
#include "../../lib/Keyboard.h"
#include "../../lib/List.h"
#include "../../lib/Log.h"
#include "../../lib/Math.h"
#include "../../lib/QuadTree.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "../components/BoxCollider2D.h"
#include "Entity.h"

static const f32 CAT_MOVE_SPEED = 0.1f;  // per-second

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

static void turn_around(CatEntity_t* self) {
  self->xa = Math__random(-1, 1);
  self->ya = Math__random(-1, 1);
  self->za = Math__random(-1, 1);
}

void CatEntity__tick(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  CatEntity_t* self = (CatEntity_t*)entity;

  static f32 lastTurnWait = 2.0f;
  static f32 sinceLastTurn = 0;
  sinceLastTurn += state->deltaTime;

  entity->xa = 0;
  entity->za = 0;
  entity->xa += self->xa * CAT_MOVE_SPEED;
  // entity->ya += self->ya * CAT_MOVE_SPEED;
  entity->za += self->za * CAT_MOVE_SPEED;

  // TODO: move to Level
  Level_t* level = logic->game->curLvl;
  bool collision = false;

  // use quadtree query to find nearby neighbors
  f32 sr = 2.0f;  // search within this radius
  f32 xxa = entity->transform.position.x + entity->xa;
  f32 yya = entity->transform.position.z + entity->za;
  Rect range = {
      xxa,  // x,y origin
      yya,
      sr,  // w,h radius
      sr};
  void* matchData[100];  // TODO: don't limit search results?
  u32 matchCount = 0;
  // TODO: query can be the radius of the entity, to shorten this code?
  QuadTreeNode_query(level->qt, range, matchData, &matchCount);
  for (u32 i = 0; i < matchCount; i++) {
    void* unk = matchData[i];
    if (entity == unk) continue;
    EntityType type = ((Block_t*)unk)->type;
    collision = false;
    if (ENTITY == type) {
      Entity_t* othere = (Entity_t*)unk;
      // TODO: differentiate types of collider by component name
      if (NULL != othere->components.collider) {
        collision = BoxCollider2D__check(
            xxa,
            yya,
            entity->r,
            othere->transform.position.x,
            othere->transform.position.z,
            othere->r);
      }
    } else if (BLOCK == type) {
      Block_t* otherb = (Block_t*)unk;
      if (!otherb->blocking) continue;
      // if (NULL != otherb->components.collider) {
      collision = BoxCollider2D__check(xxa, yya, entity->r, otherb->x, otherb->y, otherb->r);
      // }
    }
    if (collision) break;
  }
  if (collision) {
    if (sinceLastTurn > lastTurnWait) {
      sinceLastTurn = 0;
      turn_around(self);
    }
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
