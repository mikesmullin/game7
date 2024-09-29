#include "BatSpawnBlock.h"

#include <stdint.h>

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../../lib/List.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "../entities/BatEntity.h"
#include "Block.h"

typedef int32_t s32;

Block_t* BatSpawnBlock__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(BatSpawnBlock_t));
}

void BatSpawnBlock__init(Block_t* block, Engine__State_t* state, f32 x, f32 y) {
  BatSpawnBlock_t* self = (BatSpawnBlock_t*)block;
  Logic__State_t* logic = state->local;
  Block__init(block, state, x, y);
  block->tick = BAT_SPAWN_BLOCK__TICK;
  block->render = BAT_SPAWN_BLOCK__RENDER;
  self->firstTick = true;
  self->spawnCount = 1;               // instances
  self->spawnInterval = 1.0f / 1000;  // per sec
  self->animTime = 0;                 // counter
  self->spawnedCount = 0;
}

void BatSpawnBlock__render(Block_t* block, Engine__State_t* state) {
  BatSpawnBlock_t* self = (BatSpawnBlock_t*)block;
  Logic__State_t* logic = state->local;
}

void BatSpawnBlock__tick(Block_t* block, Engine__State_t* state) {
  BatSpawnBlock_t* self = (BatSpawnBlock_t*)block;
  Logic__State_t* logic = state->local;

  if (self->firstTick) {
    self->firstTick = false;
  }

  self->animTime += state->deltaTime;
  while (self->animTime > self->spawnInterval) {
    self->animTime -= self->spawnInterval;

    // spawn entities (like a particle emitter)
    for (u32 i = 0; i < self->spawnCount; i++) {
      // TODO: associate spawned entities with spawning block?
      Entity_t* entity = BatEntity__alloc(state->arena);
      BatEntity__init(entity, state);
      // TODO: fix the coords being rendered out of order and flipped
      entity->transform.position.x = block->y;
      entity->transform.position.y = 0.370f;
      entity->transform.position.z = -block->x;
      entity->flying = true;
      List__append(state->arena, state->local->game->curLvl->entities, entity);
      self->spawnedCount++;
    }
  }

  Bitmap__DebugText(
      &logic->screen,
      &logic->glyphs0,
      4,
      6 * 20,
      0xff00ff00,
      0,
      "cats %u",
      self->spawnedCount);
}