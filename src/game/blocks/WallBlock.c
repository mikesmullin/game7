#include "WallBlock.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "../components/MeshRenderer.h"
#include "Block.h"

Block_t* WallBlock__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(WallBlock_t));
}

void WallBlock__init(Block_t* block, Engine__State_t* state, f32 x, f32 y) {
  Logic__State_t* logic = state->local;
  Entity_t* entity = (Entity_t*)block;
  WallBlock_t* self = (WallBlock_t*)block;
  Block__init(block, state, x, y);
  entity->engine->render = WALL_BLOCK__RENDER;
  entity->tags1 |= TAG_WALL;
  block->meshId = MODEL_BOX;

  entity->render = Arena__Push(state->arena, sizeof(RendererComponent));
  entity->render->type = MESH_RENDERER;
  entity->render->atlas = ATLAS_TEXTURE;
  entity->render->tx = logic->game->curLvl->wallTex;
  entity->render->ty = 0;
  entity->render->color = logic->game->curLvl->wallCol;
  entity->render->mesh = BOX_MESH;
}

void WallBlock__render(Block_t* block, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Entity_t* entity = (Entity_t*)block;
  WallBlock_t* self = (WallBlock_t*)block;

  MeshRenderer__render(entity, state);
}