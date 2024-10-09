#include "Block.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "../entities/Entity.h"

// blocks are like Unity Behaviors,
// except they are instantiated from RGB pixel data

Block_t* Block__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(Block_t));
}

void Block__init(Block_t* block, Engine__State_t* state, f32 x, f32 z) {
  Logic__State_t* logic = state->local;
  Entity__init((Entity_t*)block, state);
  block->base.tick = BLOCK__TICK;
  block->base.render = BLOCK__RENDER;
  block->masked = false;
  block->base.transform.position.x = x;
  block->base.transform.position.z = z;

  BoxCollider2DComponent* collider = Arena__Push(state->arena, sizeof(BoxCollider2DComponent));
  collider->base.type = BOX_COLLIDER_2D;
  f32 sq_r = 0.3f;  // square radius
  collider->hw = sq_r, collider->hh = sq_r;
  block->base.components.collider = (ColliderComponent*)collider;
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
