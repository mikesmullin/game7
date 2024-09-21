#include "Player.h"

void Player__Init(Arena_t* arena, Logic__State_t* local) {
  local->player = Arena__Push(arena, sizeof(Player_t));
}