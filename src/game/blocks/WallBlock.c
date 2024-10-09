#include "WallBlock.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../../lib/Log.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "Block.h"

Block_t* WallBlock__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(WallBlock_t));
}

void WallBlock__init(Block_t* block, Engine__State_t* state, f32 x, f32 y) {
  WallBlock_t* self = (WallBlock_t*)block;
  Block__init(block, state, x, y);
  block->base.tick = WALL_BLOCK__TICK;
  block->base.render = WALL_BLOCK__RENDER;
  block->base.type = ENTITY_WALL;
  block->meshId = MODEL_BOX;
}

void WallBlock__render(Block_t* block, Engine__State_t* state) {
  WallBlock_t* self = (WallBlock_t*)block;
  Logic__State_t* logic = state->local;
  f32 x = block->base.transform.position.x;
  f32 z = block->base.transform.position.z;
  u32 tex = logic->game->curLvl->wallTex;
  u32 col = logic->game->curLvl->wallCol;

  // render block mesh/model
  Bitmap3D__RenderWall(state, x, 0, z, tex, 0, col);
}

void WallBlock__gui(Block_t* block, Engine__State_t* state) {
  WallBlock_t* self = (WallBlock_t*)block;
  Logic__State_t* logic = state->local;
}

void WallBlock__tick(Block_t* block, Engine__State_t* state) {
  WallBlock_t* self = (WallBlock_t*)block;
  Logic__State_t* logic = state->local;
}
