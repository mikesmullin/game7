#ifndef PLAYER_H
#define PLAYER_H

#include "../lib/Base.h"
#include "Logic.h"

typedef struct {
  f32 x;
  f32 y;
  f32 rot;
} Player_t;

void Player__Init(Arena_t* arena, Logic__State_t* local);
#endif  // PLAYER_H