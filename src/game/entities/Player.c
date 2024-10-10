#include "Player.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../../lib/Finger.h"
#include "../../lib/Keyboard.h"
#include "../../lib/Math.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "../components/Rigidbody2D.h"
#include "Entity.h"

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

    f32 xm = self->input.xAxis * PLAYER_WALK_SPEED * state->deltaTime;
    f32 zm = self->input.zAxis * PLAYER_WALK_SPEED * state->deltaTime;
    self->bobPhase += sqrt(xm * xm + zm * zm) * 4.0f;

    Rigidbody2D__move(entity, state);
  }

  if (entity->health->hurtTime > 0) {
    entity->health->hurtTime -= state->deltaTime;
    if (entity->health->hurtTime < 0) entity->health->hurtTime = 0;
  }
}