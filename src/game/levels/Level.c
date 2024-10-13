#include "Level.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Engine.h"
#include "../../lib/List.h"
#include "../../lib/Log.h"
#include "../../lib/Math.h"
#include "../Logic.h"
#include "../blocks/CatSpawnBlock.h"
#include "../blocks/SpawnBlock.h"
#include "../blocks/WallBlock.h"
#include "../utils/Bitmap3D.h"
#include "../utils/Dispatcher.h"
#include "../utils/Geometry.h"
#include "../utils/QuadTree.h"

Level_t* Level__alloc(Arena_t* arena) {
  Level_t* level = Arena__Push(arena, sizeof(Level_t));
  level->bmp = Bitmap__Prealloc(arena);
  level->world = Bitmap__Prealloc(arena);
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
        List__append(state->arena, level->entities, block);
      }
    }
  }
}

void Level__render(Level_t* level, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  List__Node_t* node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity_t* entity = node->data;
    Dispatcher__engine(entity->engine->render, entity, state);
    node = node->next;
  }
}

void Level__gui(Level_t* level, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  List__Node_t* node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity_t* entity = node->data;
    Dispatcher__engine(entity->engine->gui, entity, state);
    node = node->next;
  }
}

void Level__tick(Level_t* level, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  // build a QuadTree of all entities
  f32 W = level->width / 2, D = level->depth / 2;
  Rect boundary = {0.0f, 0.0f, W, D};  // Center (0,0), width/height 20x20
  if (NULL == level->qtArena) {
    // TODO: reduce size back to 1MB; instead don't store entities in the tree that can't collide
    level->qtArena = Arena__SubAlloc(state->arena, 1024 * 1024 * 10);  // MB
  }
  Arena__Reset(level->qtArena);
  level->qt = QuadTreeNode_create(level->qtArena, boundary);
  List__Node_t* node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity_t* entity = node->data;
    node = node->next;
    QuadTreeNode_insert(
        level->qtArena,
        level->qt,
        (Point){entity->tform->pos.x, entity->tform->pos.z},
        entity);
  }

  node = level->entities->head;
  for (u32 i = 0; i < level->entities->len; i++) {
    Entity_t* entity = node->data;
    node = node->next;
    Dispatcher__engine(entity->engine->tick, entity, state);
  }
}