#include "Level.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../../lib/List.h"
#include "../../lib/Log.h"
#include "../../lib/Math.h"
#include "../Dispatcher.h"
#include "../Logic.h"
#include "../blocks/CatSpawnBlock.h"
#include "../blocks/SpawnBlock.h"
#include "../blocks/WallBlock.h"

Level_t* Level__alloc(Arena_t* arena) {
  Level_t* level = Arena__Push(arena, sizeof(Level_t));
  level->bmp = Bitmap__Prealloc(arena);
  level->world = Bitmap__Prealloc(arena);
  level->blocks = List__alloc(arena);
  level->entities = List__alloc(arena);
  return level;
}

void Level__init(Arena_t* arena, Level_t* level, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  level->skybox = false;
  level->wallTex = 0;
  level->ceilTex = 1;
  level->floorTex = 2;
  level->wallCol = 0;
  level->ceilCol = 0;
  level->floorCol = 0;
  level->spawner = NULL;
}

Block_t* Level__makeBlock(Engine__State_t* state, u32 col, f32 x, f32 y) {
  if (0xff000000 == col) {  // black; empty space
    return NULL;
  }
  if (0xffffffff == col) {  // white
    Block_t* block = WallBlock__alloc(state->arena);
    WallBlock__init(block, state, x, y);
    return block;
  }
  if (0xff00f2ff == col) {  // yellow
    Block_t* block = SpawnBlock__alloc(state->arena);
    SpawnBlock__init(block, state, x, y);
    return block;
  }
  if (0xff241ced == col) {  // red
    Block_t* block = CatSpawnBlock__alloc(state->arena);
    CatSpawnBlock__init(block, state, x, y);
    return block;
  }

  LOG_DEBUGF("Unimplemented Level Block pixel color %08x", col);
  return NULL;
}

void Level__load(Level_t* level, Engine__State_t* state, char* levelFile, char* worldFile) {
  Logic__State_t* logic = state->local;

  state->Vulkan__FReadImage(level->bmp, levelFile);
  state->Vulkan__FReadImage(level->world, worldFile);

  for (s32 y = 0; y < level->bmp->h; y++) {
    for (s32 x = 0; x < level->bmp->w; x++) {
      u32 color = Bitmap__Get2DPixel(level->bmp, x, y, 0x00000000);
      Block_t* block = Level__makeBlock(state, color, x, y);
      if (NULL != block) {
        List__append(state->arena, level->blocks, block);
      }
    }
  }
}

void Level__render(Level_t* level, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  List__Node_t* node = level->blocks->head;
  for (u32 i = 0; i < level->blocks->len; i++) {
    Block_t* block = node->data;
    Dispatcher__engine(block->render, block, state);
    node = node->next;
  }

  node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity_t* entity = node->data;
    Dispatcher__engine(entity->render, entity, state);
    node = node->next;
  }
}

void Level__gui(Level_t* level, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  List__Node_t* node = level->blocks->head;
  for (u32 i = 0; i < level->blocks->len; i++) {
    Block_t* block = node->data;
    Dispatcher__engine(block->gui, block, state);
    node = node->next;
  }

  node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity_t* entity = node->data;
    Dispatcher__engine(entity->gui, entity, state);
    node = node->next;
  }
}

void Level__tick(Level_t* level, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  List__Node_t* node = level->blocks->head;
  for (u32 i = 0; i < level->blocks->len; i++) {
    Block_t* block = node->data;
    node = node->next;
    Dispatcher__engine(block->tick, block, state);
  }

  node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity_t* entity = node->data;
    node = node->next;
    Dispatcher__engine(entity->tick, entity, state);
  }
}

Block_t* Level__getBlock(Level_t* level, f32 x, f32 z) {
  f32 W = level->width / 2, D = level->depth / 2;
  if (x < -W || W < x || z < -D || D < z) {
    return NULL;
  }

  // TODO: would be faster if we use array lookup
  List__Node_t* node = level->blocks->head;
  for (u32 i = 0; i < level->blocks->len; i++) {
    Block_t* block = node->data;
    node = node->next;

    if (block->x == x && block->y == z) {
      return block;
    }
  }
  return NULL;
}