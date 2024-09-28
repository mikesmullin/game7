#include "Level.h"

#include "../../lib/Arena.h"
#include "../../lib/Bitmap.h"
#include "../../lib/Bitmap3D.h"
#include "../../lib/Engine.h"
#include "../../lib/List.h"
#include "../../lib/Log.h"
#include "../../lib/Math.h"
#include "../Logic.h"

Level_t* Level__alloc(Arena_t* arena) {
  Level_t* level = Arena__Push(arena, sizeof(Level_t));
  level->bmp = Bitmap__Prealloc(arena);
  level->blocks = List__alloc(arena);
  level->entities = List__alloc(arena);
  return level;
}

void Level__init(Arena_t* arena, Level_t* level) {
  level->wallTex = 0;
  level->ceilTex = 1;
  level->floorTex = 2;
  level->wallCol = 0;
  level->ceilCol = 0;
  level->floorCol = 0;
  level->firstRender = true;
}

void Level__load(Level_t* level, Engine__State_t* state, char* file) {
  state->Vulkan__FReadImage(level->bmp, file);
}

void Level__render(Level_t* level, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
  for (s32 y = 0; y < level->bmp->h; y++) {
    for (s32 x = 0; x < level->bmp->w; x++) {
      u32 color = Bitmap__Get2DPixel(level->bmp, x, y, 0x00000000);

      if (0xff000000 == color) {         // black; empty space
      } else if (0xffffffff == color) {  // white; wall
        // render pixel as 3d cube of 4 faces (N,S,E,W)
        Bitmap3D__RenderWall2(state, x + 0, y + 0, x + 0, y + 1, 1, 0x00ffffff, 1, 0);
        Bitmap3D__RenderWall2(state, x + 0, y + 1, x + 1, y + 1, 2, 0x00ffffff, 2, 0);
        Bitmap3D__RenderWall2(state, x + 1, y + 1 - 1, x + 0, y + 1 - 1, 3, 0x00ffffff, 3, 0);
        Bitmap3D__RenderWall2(state, x + 0 + 1, y + 1, x + 0 + 1, y + 0, 4, 0x00ffffff, 4, 0);
      } else if (0xff00f2ff == color) {  // yellow; player spawn
        if (level->firstRender) {
          level->firstRender = false;
          // TODO: fix the coords being rendered out of order and flipped
          logic->player->transform.position[0] = y + 0.8f;
          logic->player->transform.position[2] = -x + 0.8f;
          logic->player->transform.rotation[0] = 180.0f;
          LOG_DEBUGF(
              "Teleport player to %+03i %+03i",
              (s32)logic->player->transform.position[0],
              (s32)logic->player->transform.position[2]);
        }
      } else {
        LOG_DEBUGF("Unimplemented Level pixel color %08x", color);
      }
    }
  }
}

void Level__tick(Level_t* level, Engine__State_t* state) {
  Logic__State_t* logic = state->local;
}