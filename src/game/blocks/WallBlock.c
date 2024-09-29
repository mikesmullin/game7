#include "WallBlock.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
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
}

void WallBlock__render(Block_t* block, Engine__State_t* state) {
  WallBlock_t* self = (WallBlock_t*)block;
  Logic__State_t* logic = state->local;
  f32 x = block->x;
  f32 y = block->y;
  u32 tex = logic->game->curLvl->wallTex;
  u32 col = logic->game->curLvl->wallCol;

  // render pixel as 3d cube of 4 faces (N,S,E,W)
  Bitmap3D__RenderWall2(state, x + 0, y + 0, x + 0, y + 1, 1, col, tex, 0);
  Bitmap3D__RenderWall2(state, x + 0, y + 1, x + 1, y + 1, 2, col, tex, 0);
  Bitmap3D__RenderWall2(state, x + 1, y + 1 - 1, x + 0, y + 1 - 1, 3, col, tex, 0);
  Bitmap3D__RenderWall2(state, x + 0 + 1, y + 1, x + 0 + 1, y + 0, 4, col, tex, 0);
}

void WallBlock__tick(Block_t* block, Engine__State_t* state) {
  WallBlock_t* self = (WallBlock_t*)block;
  Logic__State_t* logic = state->local;
}
