#include "CatSpawnBlock.h"

#include <stdint.h>

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../../lib/List.h"
#include "../../lib/Math.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "../entities/CatEntity.h"
#include "Block.h"

typedef int32_t s32;

Block_t* CatSpawnBlock__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(CatSpawnBlock_t));
}

void CatSpawnBlock__init(Block_t* block, Engine__State_t* state, f32 x, f32 y) {
  CatSpawnBlock_t* self = (CatSpawnBlock_t*)block;
  Logic__State_t* logic = state->local;
  Block__init(block, state, x, y);
  block->base.engine->tick = CAT_SPAWN_BLOCK__TICK;
  block->base.engine->gui = CAT_SPAWN_BLOCK__GUI;
  block->base.collider = NULL;

  self->firstTick = true;
  self->spawnCount = 1;  // instances
  self->spawnInterval = 1.0f;  // per sec
  self->animTime = 0;  // counter
  self->spawnedCount = 0;
  self->maxSpawnCount = 1;
}

void CatSpawnBlock__gui(Block_t* block, Engine__State_t* state) {
  CatSpawnBlock_t* self = (CatSpawnBlock_t*)block;
  Logic__State_t* logic = state->local;

  Bitmap__DebugText(
      &logic->screen,
      &logic->glyphs0,
      4,
      6 * 20,
      0xff00ff00,
      0xff000000,
      "cats %u",
      self->spawnedCount);
}

void CatSpawnBlock__tick(Block_t* block, Engine__State_t* state) {
  CatSpawnBlock_t* self = (CatSpawnBlock_t*)block;
  Logic__State_t* logic = state->local;

  if (self->firstTick) {
    self->firstTick = false;
  }

  self->animTime += state->deltaTime;
  while (self->animTime > self->spawnInterval) {
    self->animTime -= self->spawnInterval;

    // spawn entities (like a particle emitter)
    for (u32 i = 0; i < self->spawnCount; i++) {
      if (self->spawnedCount >= self->maxSpawnCount) return;
      // TODO: associate spawned entities with spawning block?
      Entity_t* entity = CatEntity__alloc(state->arena);
      CatEntity__init(entity, state);
      entity->tform->pos.x = block->base.tform->pos.x + Math__random(-1, 1);
      entity->tform->pos.y = -0.25f;
      entity->tform->pos.z = block->base.tform->pos.z + Math__random(-1, 1);
      List__append(state->arena, state->local->game->curLvl->entities, entity);
      self->spawnedCount++;
    }
  }
}