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
  block->tick = WALL_BLOCK__TICK;
  block->render = WALL_BLOCK__RENDER;
  block->meshId = MODEL_BOX;
}

void WallBlock__render(Block_t* block, Engine__State_t* state) {
  WallBlock_t* self = (WallBlock_t*)block;
  Logic__State_t* logic = state->local;
  f32 x = block->x;
  f32 y = block->y;
  u32 tex = logic->game->curLvl->wallTex;
  u32 col = logic->game->curLvl->wallCol;

  // render block mesh/model
  Bitmap3D__RenderWall(
      state,
      x,
      0,
      y,
      (u32[]){//

              0,  // t
              2,  // s
              3,  // w
              0,  // b
              4,  // e
              1,
              0,  // t
              2,  // s
              3,  // w
              0,  // b
              4,  // e
              1,
              1,
              1},
      0,
      col);
}

void WallBlock__gui(Block_t* block, Engine__State_t* state) {
  WallBlock_t* self = (WallBlock_t*)block;
  Logic__State_t* logic = state->local;
}

void WallBlock__tick(Block_t* block, Engine__State_t* state) {
  WallBlock_t* self = (WallBlock_t*)block;
  Logic__State_t* logic = state->local;
}
