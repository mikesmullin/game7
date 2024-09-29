#include "Dispatcher.h"

#include "Sprite.h"
#include "blocks/Block.h"
#include "blocks/CatSpawnBlock.h"
#include "blocks/SpawnBlock.h"
#include "blocks/WallBlock.h"
#include "entities/CatEntity.h"
#include "entities/Entity.h"
#include "entities/Player.h"
#include "menus/AboutMenu.h"
#include "menus/HelpMenu.h"
#include "menus/TitleMenu.h"

static void (*VTABLE[])() = {
    Block__tick,      Block__render,      CatSpawnBlock__tick, CatSpawnBlock__render,
    SpawnBlock__tick, SpawnBlock__render, WallBlock__tick,     WallBlock__render,
    Entity__tick,     Entity__render,     CatEntity__tick,     CatEntity__render,
    Player__tick,     Player__render,     AboutMenu__tick,     AboutMenu__render,
    HelpMenu__tick,   HelpMenu__render,   TitleMenu__tick,     TitleMenu__render,
    Sprite__tick,     Sprite__render,
};

// static / switch / tag / conditional dispatch
// a hot-reload safe, simple alternative to polymorphism
// works because all fns are known at compile-time
void Dispatcher__call(DispatchFnId id, void* self, Engine__State_t* state) {
  return VTABLE[id](self, state);
}