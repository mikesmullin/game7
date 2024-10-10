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
  block->base.engine->render = WALL_BLOCK__RENDER;
  block->base.tags1 |= TAG_WALL;
  block->meshId = MODEL_BOX;
}

void WallBlock__render(Block_t* block, Engine__State_t* state) {
  WallBlock_t* self = (WallBlock_t*)block;
  Logic__State_t* logic = state->local;
  f32 x = block->base.tform->pos.x;
  f32 z = block->base.tform->pos.z;
  u32 tex = logic->game->curLvl->wallTex;
  u32 col = logic->game->curLvl->wallCol;

  // render block mesh/model
  Bitmap3D__RenderWall(state, x, 0, z, tex, 0, col);
}