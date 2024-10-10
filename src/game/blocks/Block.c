#include "Block.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "../entities/Entity.h"

// blocks are 3d models instantiated from level RGB pixel data

Block_t* Block__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(Block_t));
}

void Block__init(Block_t* block, Engine__State_t* state, f32 x, f32 z) {
  Logic__State_t* logic = state->local;
  Entity__init((Entity_t*)block, state);
  block->masked = false;
  block->base.tform->pos.x = x;
  block->base.tform->pos.z = z;

  BoxCollider2DComponent* collider = Arena__Push(state->arena, sizeof(BoxCollider2DComponent));
  collider->base.type = BOX_COLLIDER_2D;
  f32 sq_r = 0.3f;  // square radius
  collider->hw = sq_r, collider->hh = sq_r;
  block->base.collider = (ColliderComponent*)collider;
}