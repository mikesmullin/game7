#include "SpawnBlock.h"

#include <stdint.h>

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../Dispatcher.h"
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
  block->base.tick = SPAWN_BLOCK__TICK;
  block->base.render = SPAWN_BLOCK__RENDER;
  block->base.components.collider = NULL;

  self->firstTick = true;
  logic->game->curLvl->spawner = self;
}

void SpawnBlock__render(Block_t* block, Engine__State_t* state) {
  SpawnBlock_t* self = (SpawnBlock_t*)block;
  Logic__State_t* logic = state->local;
}

void SpawnBlock__gui(Block_t* block, Engine__State_t* state) {
  SpawnBlock_t* self = (SpawnBlock_t*)block;
  Logic__State_t* logic = state->local;
}

void SpawnBlock__tick(Block_t* block, Engine__State_t* state) {
  SpawnBlock_t* self = (SpawnBlock_t*)block;
  Logic__State_t* logic = state->local;

  if (self->firstTick) {
    self->firstTick = false;

    state->Audio__StopAudio(state->audio, AUDIO_POWERUP);
    state->Audio__ResumeAudio(state->audio, AUDIO_POWERUP, false, 1.0f);

    logic->game->curPlyr->transform.position.x = block->base.transform.position.x;
    logic->game->curPlyr->transform.position.y = 3;
    logic->game->curPlyr->transform.position.z = block->base.transform.position.z;
    logic->game->curPlyr->transform.rotation.x = 0;
    logic->game->curPlyr->transform.rotation.y = 0;
  }
}
