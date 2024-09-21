#include "Player.h"

#include "../lib/Arena.h"

void Player__Init(Arena_t* arena, Player_t** player) {
  *player = Arena__Push(arena, sizeof(Player_t));
  (*player)->x = 0;
  (*player)->y = 0;
  (*player)->z = 1;
  (*player)->rot = 0;
}