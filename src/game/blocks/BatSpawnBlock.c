#include "BatSpawnBlock.h"

#include <stdint.h>

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../../lib/List.h"
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

    // spawn entity
    Entity_t* entity = BatEntity__alloc(state->arena);
    BatEntity__init(entity, state);
    // TODO: fix the coords being rendered out of order and flipped
    entity->transform.position.x = block->y;
    entity->transform.position.y = 0.370f;
    entity->transform.position.z = -block->x;
    entity->flying = true;
    List__append(state->arena, state->local->game->curLvl->entities, entity);
  }
}
