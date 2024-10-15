#include "Player.h"

#include <math.h>

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../../lib/Finger.h"
#include "../../lib/Keyboard.h"
// #include "../../lib/Log.h"
#include "../../lib/Math.h"
#include "../Logic.h"
#include "../behaviortrees/Action.h"
#include "../components/Rigidbody2D.h"
#include "../utils/Dispatcher.h"
#include "../utils/Geometry.h"
#include "../utils/QuadTree.h"
#include "Entity.h"

static const f32 PLAYER_WALK_SPEED = 5.0f;  // per-second
static const f32 PLAYER_STRAFE_MOD = 0.5f;  // percent of walk
static const f32 PLAYER_FLY_SPEED = 3.0f;  // per-second
static const f32 PLAYER_LOOK_SPEED = 0.3f;  // deg/sec

Entity_t* Player__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(Player_t));
}

void Player__init(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Player_t* self = (Player_t*)entity;

  Entity__init(entity, state);

  entity->engine->tick = PLAYER_ENTITY__TICK;

  self->input.xAxis = 0.0f;
  self->input.yAxis = 0.0f;
  self->input.zAxis = 0.0f;

  self->camera.fov = 45.0f;
  self->camera.nearZ = 0.1f;
  self->camera.farZ = 1000.0f;

  CircleCollider2DComponent* collider =
      Arena__Push(state->arena, sizeof(CircleCollider2DComponent));
  collider->base.type = CIRCLE_COLLIDER_2D;
  collider->r = 0.5f;
  entity->collider = (ColliderComponent*)collider;

  entity->health = Arena__Push(state->arena, sizeof(HealthComponent));
  entity->health->hp = 100;
  entity->health->hurtTime = 0;

  entity->hear = Arena__Push(state->arena, sizeof(AudioListenerComponent));
}

void Player__tick(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Player_t* self = (Player_t*)entity;

  if (!state->mouseCaptured) {
    state->mState->x = 0;
    state->mState->y = 0;
  } else {
    if (0 != state->mState->x) {  // yaw (rotate around Y-axis)
      logic->game->curPlyr->tform->rot.y += -state->mState->x * PLAYER_LOOK_SPEED;
      logic->game->curPlyr->tform->rot.y = Math__fmod(logic->game->curPlyr->tform->rot.y, 360.0f);
      state->mState->x = 0;
    }

    if (0 != state->mState->y) {  // pitch (rotate around X-axis)
      logic->game->curPlyr->tform->rot.x += -state->mState->y * PLAYER_LOOK_SPEED;
      logic->game->curPlyr->tform->rot.x = Math__fmod(logic->game->curPlyr->tform->rot.x, 360.0f);
      state->mState->y = 0;
    }

    if (true == state->kbState->reload) {  // R
      state->kbState->reload = false;
      logic->game->curLvl->spawner->firstTick = true;  // tp to spawn
    }

    // W-S Forward/Backward axis
    if (state->kbState->fwd && state->kbState->back) {
      self->input.zAxis = 0.0f;
    } else if (state->kbState->fwd) {
      self->input.zAxis = -1.0f;  // -Z_FWD
    } else if (state->kbState->back) {
      self->input.zAxis = +1.0f;
    } else {
      self->input.zAxis = 0.0f;
    }

    // A-D Left/Right axis
    if (state->kbState->left && state->kbState->right) {
      self->input.xAxis = 0.0f;
    } else if (state->kbState->left) {
      self->input.xAxis = -1.0f;
    } else if (state->kbState->right) {
      self->input.xAxis = +1.0f;  // +X_RIGHT
    } else {
      self->input.xAxis = 0.0f;
    }

    // Q-E Up/Down axis
    if (state->kbState->up && state->kbState->down) {
      self->input.yAxis = 0.0f;
    } else if (state->kbState->up) {
      self->input.yAxis = +1.0f;  // +Y_UP
    } else if (state->kbState->down) {
      self->input.yAxis = -1.0f;
    } else {
      self->input.yAxis = 0.0f;
    }

    // Direction vectors for movement
    v3 forward, right, front;

    // Convert yaw to radians for direction calculation
    f32 yaw_radians = glms_rad(entity->tform->rot.y);

    // Calculate the front vector based on yaw only (for movement along the XZ plane)
    front.x = Math__sin(yaw_radians);
    front.y = 0.0f;
    front.z = Math__cos(yaw_radians);
    glms_v3_normalize(&front);

    // Calculate the right vector (perpendicular to the front vector)
    glms_v3_cross(front, (v3){0.0f, 1.0f, 0.0f}, &right);
    glms_v3_normalize(&right);

    // apply forward/backward motion
    v3 pos;
    // TODO: can manipulate this to simulate slipping/ice
    entity->rb->xa = 0;
    entity->rb->za = 0;
    if (0 != self->input.zAxis) {
      glms_v3_scale(front, self->input.zAxis * PLAYER_WALK_SPEED * state->deltaTime, &forward);
      glms_v3_add(entity->tform->pos, forward, &pos);
      entity->rb->xa += pos.x - entity->tform->pos.x;
      entity->rb->za += pos.z - entity->tform->pos.z;
    }

    // apply left/right motion
    if (0 != self->input.xAxis) {
      glms_v3_scale(
          right,
          -self->input.xAxis * PLAYER_WALK_SPEED * PLAYER_STRAFE_MOD * state->deltaTime,
          &forward);
      glms_v3_add(entity->tform->pos, forward, &pos);
      entity->rb->xa += pos.x - entity->tform->pos.x;
      entity->rb->za += pos.z - entity->tform->pos.z;
    }

    // apply up/down motion
    if (0 != self->input.yAxis) {
      entity->tform->pos.y += self->input.yAxis * PLAYER_FLY_SPEED * state->deltaTime;

      entity->tform->pos.y = MATH_CLAMP(0, entity->tform->pos.y, logic->game->curLvl->height);
    }

    static const f32 SQRT_TWO = 1.414214f;

    f32 xm = self->input.xAxis;
    f32 zm = self->input.zAxis;
    f32 d = sqrtf(xm * xm + zm * zm);
    if (0 != self->input.zAxis && 0 != self->input.xAxis) {
      // normalize diagonal movement, so it is not faster
      entity->rb->xa /= SQRT_TWO;
      entity->rb->za /= SQRT_TWO;
    }

    // headbob
    if (d != 0) {
      if (xm != 0 && zm != 0) {
        // normalize diagonal movement, so it is not faster
        d /= SQRT_TWO;
      }
      d *= PLAYER_WALK_SPEED * state->deltaTime * 4.0f;
      self->bobPhase += d;
    }

    Rigidbody2D__move(entity, state);

    // Space (Use)
    if (state->kbState->use) {
      state->kbState->use = false;

      // find nearest cat
      // TODO: make into reusable FindNearestEntity() type of fn
      u32 matchCount = 0;
      void* matchData[10];  // TODO: don't limit search results?
      Rect range = {entity->tform->pos.x, entity->tform->pos.z, 2, 2};
      QuadTreeNode_query(logic->game->curLvl->qt, range, 10, matchData, &matchCount);

      for (u32 i = 0; i < matchCount; i++) {
        Entity_t* other = (Entity_t*)matchData[i];
        if (entity == other) continue;
        if (!(other->tags1 & TAG_CAT)) continue;

        CatEntity_t* cat = (CatEntity_t*)other;
        Action action = {.type = ACTION_USE, .actor = entity, .target = other};

        // Action__PerformBuffered(other, &action);
        if (NULL != cat->sg) {
          if (NULL != cat->sg->actions) {
            cat->sg->actions(cat->sg, &action);
          }
        }
      }
    }
  }

  if (entity->health->hurtTime > 0) {
    entity->health->hurtTime -= state->deltaTime;
    if (entity->health->hurtTime < 0) entity->health->hurtTime = 0;
  }
}