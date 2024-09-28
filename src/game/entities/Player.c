#include "Player.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../Logic.h"

Entity_t* Player__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(Player_t));
}

void Player__init(Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Player_t* self = (Player_t*)entity;

  entity->tick = &Player__tick;
  entity->render = &Player__render;

  self->input.xAxis = 0.0f;
  self->input.yAxis = 0.0f;
  self->input.zAxis = 0.0f;

  self->camera.fov = 45.0f;
  self->camera.nearZ = 0.1f;
  self->camera.farZ = 1000.0f;

  entity->transform.position[0] = 0.0f;
  entity->transform.position[1] = 0.370f;
  entity->transform.position[2] = 0.0f;

  entity->transform.rotation[0] = 180.0f;
  entity->transform.rotation[1] = 0.0f;
  entity->transform.rotation[2] = 0.0f;
  entity->transform.rotation[3] = 0.0f;
}

void Player__render(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Player_t* self = (Player_t*)entity;
}

void Player__tick(struct Entity_t* entity, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Player_t* self = (Player_t*)entity;
}