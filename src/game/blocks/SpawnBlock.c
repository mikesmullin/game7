#include "SpawnBlock.h"

#include <stdint.h>

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../Logic.h"
#include "../utils/Bitmap3D.h"
#include "../utils/Dispatcher.h"
#include "Block.h"

typedef int32_t s32;

Block_t* SpawnBlock__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(SpawnBlock_t));
}

void SpawnBlock__init(Block_t* block, Engine__State_t* state, f32 x, f32 y) {
  SpawnBlock_t* self = (SpawnBlock_t*)block;
  Logic__State_t* logic = state->local;
  Block__init(block, state, x, y);
  block->base.engine->tick = SPAWN_BLOCK__TICK;
  block->base.collider = NULL;

  self->firstTick = true;
  logic->game->curLvl->spawner = self;
}

void SpawnBlock__tick(Block_t* block, Engine__State_t* state) {
  SpawnBlock_t* self = (SpawnBlock_t*)block;
  Logic__State_t* logic = state->local;

  if (self->firstTick) {
    self->firstTick = false;

    state->Audio__StopAudio(state->audio, AUDIO_POWERUP);
    state->Audio__ResumeAudio(state->audio, AUDIO_POWERUP, false, 1.0f, 0);

    logic->game->curPlyr->tform->pos.x = block->base.tform->pos.x;
    logic->game->curPlyr->tform->pos.y = 0;
    logic->game->curPlyr->tform->pos.z = block->base.tform->pos.z;
    logic->game->curPlyr->tform->rot.x = 0;
    logic->game->curPlyr->tform->rot.y = 0;
  }
}
