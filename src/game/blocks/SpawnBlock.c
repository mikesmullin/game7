#include "SpawnBlock.h"

#include <stdint.h>

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../../lib/Log.h"
#include "../Logic.h"
#include "Block.h"

typedef int32_t s32;

Block_t* SpawnBlock__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(SpawnBlock_t));
}

void SpawnBlock__init(Block_t* block, Engine__State_t* state, f32 x, f32 y) {
  SpawnBlock_t* self = (SpawnBlock_t*)block;
  Logic__State_t* logic = state->local;
  Block__init(block, state, x, y);
  block->tick = &SpawnBlock__tick;
  block->render = &SpawnBlock__render;
  self->firstTick = true;
  logic->game->currentLevel->spawner = self;
}

void SpawnBlock__render(Block_t* block, Engine__State_t* state) {
  SpawnBlock_t* self = (SpawnBlock_t*)block;
  Logic__State_t* logic = state->local;
}

void SpawnBlock__tick(Block_t* block, Engine__State_t* state) {
  SpawnBlock_t* self = (SpawnBlock_t*)block;
  Logic__State_t* logic = state->local;

  if (self->firstTick) {
    self->firstTick = false;

    state->Audio__StopAudio(AUDIO_POWERUP);
    state->Audio__ResumeAudio(AUDIO_POWERUP, false, 1.0f);

    // TODO: fix the coords being rendered out of order and flipped
    logic->player->transform.position[0] = block->y + 0.8f;
    logic->player->transform.position[1] = 0.370f;
    logic->player->transform.position[2] = -block->x + 0.8f;
    logic->player->transform.rotation[0] = 180.0f;

    LOG_DEBUGF(
        "Teleport player to %+03i %+03i %+03i",
        (s32)logic->player->transform.position[0],
        (s32)logic->player->transform.position[1],
        (s32)logic->player->transform.position[2]);
  }
}
