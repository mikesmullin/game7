#include "BreakBlock.h"

#include "../../lib/Arena.h"
#include "../../lib/Engine.h"
#include "../Logic.h"
#include "../components/AudioSource.h"
#include "../components/MeshRenderer.h"
#include "../sprites/RubbleSprite.h"
#include "../utils/Bitmap3D.h"
#include "../utils/Dispatcher.h"
#include "Block.h"

Block_t* BreakBlock__alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(BreakBlock_t));
}

void BreakBlock__init(Block_t* block, Engine__State_t* state, f32 x, f32 y) {
  Logic__State_t* logic = state->local;
  Entity_t* entity = (Entity_t*)block;
  BreakBlock_t* self = (BreakBlock_t*)block;
  Block__init(block, state, x, y);
  entity->engine->render = BREAK_BLOCK__RENDER;
  entity->engine->tick = BREAK_BLOCK__TICK;
  entity->tags1 |= TAG_BRICK;
  block->meshId = MODEL_BOX;

  entity->render = Arena__Push(state->arena, sizeof(RendererComponent));
  entity->render->type = MESH_RENDERER;
  entity->render->atlas = ATLAS_TEXTURE;
  entity->render->tx = logic->game->curLvl->wallTex + 1;
  entity->render->ty = 0;
  entity->render->color = logic->game->curLvl->wallCol;
  entity->render->mesh = BOX_MESH;

  self->sg = Arena__Push(state->arena, sizeof(StateGraph));
  self->sg->currentState = 0;
  self->sg->entity = entity;

  for (u32 i = 0; i < 32; i++) {
    RubbleSprite* sprite = Arena__Push(state->arena, sizeof(RubbleSprite));
    RubbleSprite__init(sprite, state);
    sprite->base.pos.x = entity->tform->pos.x;
    sprite->base.pos.y = entity->tform->pos.y;
    sprite->base.pos.z = entity->tform->pos.z;
    sprite->base.color = logic->game->curLvl->wallCol;
    self->sprites[i] = sprite;
  }
}

void BreakBlock__render(Block_t* block, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Entity_t* entity = (Entity_t*)block;
  BreakBlock_t* self = (BreakBlock_t*)block;

  if (TAG_BROKEN & entity->tags1) {
    for (u32 i = 0; i < 32; i++) {
      RubbleSprite* sprite = self->sprites[i];
      RubbleSprite__render(sprite, state);
    }
  } else {
    MeshRenderer__render(entity, state);
  }
}
void BreakBlock__tick(Block_t* block, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  Entity_t* entity = (Entity_t*)block;
  BreakBlock_t* self = (BreakBlock_t*)block;

  if (TAG_BROKEN & entity->tags1) {
    for (u32 i = 0; i < 32; i++) {
      RubbleSprite* sprite = self->sprites[i];
      RubbleSprite__tick(sprite, state);
    }
  }
}

void BreakBlock__callSGAction(StateGraph* sg, Action* action) {
  Entity_t* entity = (Entity_t*)sg->entity;
  BreakBlock_t* self = (BreakBlock_t*)entity;

  if (ACTION_USE == action->type) {
    if (!(TAG_BROKEN & entity->tags1)) {
      entity->tags1 |= TAG_BROKEN;
      AudioSource__play(sg->entity, AUDIO_BASH);
    }
  }
}
