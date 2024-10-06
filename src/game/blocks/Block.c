#include "Block.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../Dispatcher.h"
#include "../Logic.h"

// blocks are like Unity Behaviors,
// except they are instantiated from RGB pixel data

Block_t* Block__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(Block_t));
}

void Block__init(Block_t* block, Engine__State_t* state, f32 x, f32 y) {
  Logic__State_t* logic = state->local;
  block->tick = BLOCK__TICK;
  block->render = BLOCK__RENDER;
  block->collide = BLOCK__COLLIDE;
  block->id = ++logic->game->lastUid;
  block->blocking = true;
  block->masked = false;
  block->x = x;
  block->y = y;
}

void Block__render(Block_t* block, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
}

void Block__gui(Block_t* block, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
}

void Block__tick(Block_t* block, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
}

bool Block__collide(Block_t* block, Engine__State_t* state, Entity_t* entity, f64 x, f64 y) {
  return block->blocking;
}
