#include "Player.h"

#include "../lib/Arena.h"
#include "Logic.h"

void Player__Init(Logic__State_t* local) {
  // local->player = Arena__Push(local->arena, sizeof(Player_t));

  local->player.camera.fov = 90.0f;
  local->player.camera.nearZ = 0.1f;
  local->player.camera.farZ = 1000.0f;

  local->player.transform.position[0] = 0.0f;
  local->player.transform.position[1] = 0.0f;
  local->player.transform.position[2] = 33.0f;

  local->player.transform.rotation[0] = 0.0f;
  local->player.transform.rotation[1] = 1.0f;
  local->player.transform.rotation[2] = 0.0f;
}