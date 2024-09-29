#include "Dispatcher.h"

#include "../lib/Log.h"
#include "Sprite.h"
#include "blocks/BatSpawnBlock.h"
#include "blocks/Block.h"
#include "blocks/SpawnBlock.h"
#include "blocks/WallBlock.h"
#include "entities/BatEntity.h"
#include "entities/Entity.h"
#include "entities/Player.h"
#include "menus/AboutMenu.h"
#include "menus/HelpMenu.h"
#include "menus/TitleMenu.h"

// static / switch / tag / conditional dispatch
// a hot-reload safe, simple alternative to polymorphism
// works because all fns are known at compile-time
void Dispatcher__call(DispatchFnId id, void* self, Engine__State_t* state) {
  if (id == BLOCK__TICK) return Block__tick(self, state);
  if (id == BLOCK__RENDER) return Block__render(self, state);
  if (id == BAT_SPAWN_BLOCK__TICK) return BatSpawnBlock__tick(self, state);
  if (id == BAT_SPAWN_BLOCK__RENDER) return BatSpawnBlock__render(self, state);
  if (id == SPAWN_BLOCK__TICK) return SpawnBlock__tick(self, state);
  if (id == SPAWN_BLOCK__RENDER) return SpawnBlock__render(self, state);
  if (id == WALL_BLOCK__TICK) return WallBlock__tick(self, state);
  if (id == WALL_BLOCK__RENDER) return WallBlock__render(self, state);

  if (id == ENTITY__TICK) return Entity__tick(self, state);
  if (id == ENTITY__RENDER) return Entity__render(self, state);
  if (id == BAT_ENTITY__TICK) return BatEntity__tick(self, state);
  if (id == BAT_ENTITY__RENDER) return BatEntity__render(self, state);
  if (id == PLAYER_ENTITY__TICK) return Player__tick(self, state);
  if (id == PLAYER_ENTITY__RENDER) return Player__render(self, state);

  if (id == ABOUT_MENU__TICK) return AboutMenu__tick(self, state);
  if (id == ABOUT_MENU__RENDER) return AboutMenu__render(self, state);
  if (id == HELP_MENU__TICK) return HelpMenu__tick(self, state);
  if (id == HELP_MENU__RENDER) return HelpMenu__render(self, state);
  if (id == TITLE_MENU__TICK) return TitleMenu__tick(self, state);
  if (id == TITLE_MENU__RENDER) return TitleMenu__render(self, state);

  if (id == SPRITE__TICK) return Sprite__tick(self, state);
  if (id == SPRITE__RENDER) return Sprite__render(self, state);

  ASSERT_CONTEXT(0, "DispatchFnId not implemented.");
}