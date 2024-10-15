#include "Dispatcher.h"

#include "../blocks/Block.h"
#include "../blocks/BreakBlock.h"
#include "../blocks/CatSpawnBlock.h"
#include "../blocks/SpawnBlock.h"
#include "../blocks/WallBlock.h"
#include "../entities/CatEntity.h"
#include "../entities/Entity.h"
#include "../entities/Player.h"
#include "../menus/AboutMenu.h"
#include "../menus/HelpMenu.h"
#include "../menus/TitleMenu.h"
#include "../sprites/Sprite.h"

typedef float f32;

static void Dispatch__None() {
}

static void (*VTABLE_ENGINE[])() = {
    Dispatch__None,

    CatSpawnBlock__tick,  //
    CatSpawnBlock__gui,
    SpawnBlock__tick,  //
    WallBlock__render,  //
    BreakBlock__render,  //
    BreakBlock__tick,

    CatEntity__tick,  //
    CatEntity__render,  //
    CatEntity__gui,  //
    CatEntity__collide,  //
    Player__tick,  //

    AboutMenu__tick,  //
    AboutMenu__render,  //
    AboutMenu__gui,  //
    HelpMenu__tick,  //
    HelpMenu__render,  //
    HelpMenu__gui,  //
    TitleMenu__tick,  //
    TitleMenu__render,  //
    TitleMenu__gui,  //
};

// static / switch / tag / conditional dispatch
// a hot-reload safe, simple alternative to polymorphism
// works because all fns are known at compile-time
void Dispatcher__engine(DispatchFnId id, void* self, Engine__State_t* state) {
  return VTABLE_ENGINE[id](self, state);
}

// TODO: these fns should all just take one single closure pointer
void Dispatcher__collide(
    DispatchFnId id, void* self, Engine__State_t* state, OnCollideClosure* params) {
  return VTABLE_ENGINE[id](self, state, params);
}