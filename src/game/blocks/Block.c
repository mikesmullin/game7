#include "Block.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../Logic.h"

Block_t* Block__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(Block_t));
}

void Block__init(Block_t* block, Engine__State_t* state, f32 x, f32 y) {
  Logic__State_t* logic = state->local;
  block->tick = &Block__tick;
  block->render = &Block__render;
  block->id = ++logic->game->lastBlockUid;
  block->blocking = false;
  block->masked = false;
  block->x = x;
  block->y = y;
}

void Block__render(Block_t* block, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
}

void Block__tick(Block_t* block, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
}