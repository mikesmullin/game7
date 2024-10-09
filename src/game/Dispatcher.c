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

typedef float f32;

static void Dispatch__None() {
}

static void (*VTABLE_ENGINE[])() = {
    Dispatch__None,

    Block__tick,  //
    Block__render,  //
    Block__gui,  //
    CatSpawnBlock__tick,  //
    CatSpawnBlock__render,  //
    CatSpawnBlock__gui,
    SpawnBlock__tick,  //
    SpawnBlock__render,  //
    SpawnBlock__gui,  //
    WallBlock__tick,  //
    WallBlock__render,  //
    WallBlock__gui,  //

    Entity__tick,  //
    Entity__render,  //
    Entity__gui,  //
    CatEntity__tick,  //
    CatEntity__render,  //
    CatEntity__gui,  //
    CatEntity__collide,  //
    Player__tick,  //
    Player__render,  //
    Player__gui,  //

    AboutMenu__tick,  //
    AboutMenu__render,  //
    AboutMenu__gui,  //
    HelpMenu__tick,  //
    HelpMenu__render,  //
    HelpMenu__gui,  //
    TitleMenu__tick,  //
    TitleMenu__render,  //
    TitleMenu__gui,  //

    Sprite__tick,  //
    Sprite__render,  //
    Sprite__gui,  //
};

// static / switch / tag / conditional dispatch
// a hot-reload safe, simple alternative to polymorphism
// works because all fns are known at compile-time
void Dispatcher__engine(DispatchFnId id, void* self, Engine__State_t* state) {
  return VTABLE_ENGINE[id](self, state);
}

void Dispatcher__collide(
    DispatchFnId id, void* self, Engine__State_t* state, OnCollideClosure* params) {
  return VTABLE_ENGINE[id](self, state, params);
}