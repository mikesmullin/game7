#include "Level.h"

#include "../lib/Base.h"
#include "../lib/Bitmap.h"
#include "../lib/Bitmap3D.h"
#include "../lib/Engine.h"
#include "../lib/Math.h"

void Level__Load(Engine__State_t* game, u8 id) {
  game->local->currentLevel = 0 == id ? game->local->currentLevel : id;
  game->local->levelLoaded = false;
}

void Level__Render(Engine__State_t* game) {
  if (1 == game->local->currentLevel) {
    for (s32 y = 0; y < game->local->level1.h; y++) {
      for (s32 x = 0; x < game->local->level1.w; x++) {
        u32 color = Bitmap__Get2DPixel(&game->local->level1, x, y, 0x00000000);

        if (0xff000000 == color) {         // black; empty space
        } else if (0xffffffff == color) {  // white; wall
          // render pixel as 3d cube of 4 faces (N,S,E,W)
          Bitmap3D__RenderWall2(game, x + 0, y + 0, x + 0, y + 1, 1, 0x00ffffff, 1, 0);
          Bitmap3D__RenderWall2(game, x + 0, y + 1, x + 1, y + 1, 2, 0x00ffffff, 2, 0);
          Bitmap3D__RenderWall2(game, x + 1, y + 1 - 1, x + 0, y + 1 - 1, 3, 0x00ffffff, 3, 0);
          Bitmap3D__RenderWall2(game, x + 0 + 1, y + 1, x + 0 + 1, y + 0, 4, 0x00ffffff, 4, 0);
        } else if (0xff00f2ff == color) {  // yellow; player spawn
          if (!game->local->levelLoaded) {
            game->local->levelLoaded = true;
            // TODO: fix the coords being rendered out of order and flipped
            game->local->player.transform.position[0] = y;
            game->local->player.transform.position[2] = -x;
            LOG_DEBUGF("Teleport player to %+03i %+03i", y, -x);
          }
        } else {
          LOG_DEBUGF("Unrecognized Level pixel color %08x", color);
        }
      }
    }
  }
}

void Level__RNG(Engine__State_t* game) {
  static bool level[100][100];
  static bool filled = false;
  if (!filled) {
    filled = true;
    for (u32 y = 0; y < 100; y++) {
      for (u32 x = 0; x < 100; x++) {
        level[x][y] = false;
      }
    }
    for (u32 i = 0; i < 2000; i++) {
      u32 x = Math__urandom2(0, 100);
      u32 y = Math__urandom2(0, 100);
      level[x][y] = true;
    }
  }
  for (u32 y = 0; y < 100; y++) {
    for (u32 x = 0; x < 100; x++) {
      if (level[x][y]) {
        Bitmap3D__RenderWall2(game, x + 0, y + 0, x + 0, y + 1, 1, 0x00ffffff, 1, 0);
        Bitmap3D__RenderWall2(game, x + 0, y + 1, x + 1, y + 1, 2, 0x00ffffff, 2, 0);
        Bitmap3D__RenderWall2(game, x + 1, y + 1 - 1, x + 0, y + 1 - 1, 3, 0x00ffffff, 3, 0);
        Bitmap3D__RenderWall2(game, x + 0 + 1, y + 1, x + 0 + 1, y + 0, 4, 0x00ffffff, 4, 0);
      }
    }
  }
}