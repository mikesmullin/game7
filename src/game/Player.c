#include "Player.h"

#include "Logic.h"

void Player__Init(Logic__State_t* local) {
  // local->player = Arena__Push(local->arena, sizeof(Player_t));

  local->player.input.xAxis = 0.0f;
  local->player.input.yAxis = 0.0f;
  local->player.input.zAxis = 0.0f;

  local->player.camera.fov = 45.0f;
  local->player.camera.nearZ = 0.1f;
  local->player.camera.farZ = 1000.0f;

  // was: 0 8 0 0

  local->player.transform.position[0] = 1.113f;
  local->player.transform.position[1] = 2.222f;
  local->player.transform.position[2] = 5.728f;

  local->player.transform.rotation[0] = 254.6f;
  local->player.transform.rotation[1] = 0.0f;
  local->player.transform.rotation[2] = 0.0f;
  local->player.transform.rotation[3] = 0.0f;
}