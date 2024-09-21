#ifndef PLAYER_H
#define PLAYER_H

#include "../lib/Arena.h"
#include "../lib/Base.h"

typedef struct {
  f32 x;
  f32 y;
  f32 z;
  f32 rot;
} Player_t;

void Player__Init(Arena_t* arena, Player_t** player);

#endif  // PLAYER_H