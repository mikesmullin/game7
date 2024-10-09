#include "Game.h"

#include "../lib/Arena.h"
#include "../lib/Bitmap.h"
#include "../lib/Bitmap3D.h"
#include "../lib/Engine.h"
#include "../lib/Finger.h"
#include "../lib/Keyboard.h"
#include "../lib/List.h"
#include "../lib/Math.h"
#include "../lib/String.h"
#include "../lib/Wavefront.h"
#include "Dispatcher.h"
#include "Logic.h"
#include "entities/Player.h"
#include "levels/Level.h"
#include "menus/TitleMenu.h"

Game_t* Game__alloc(Arena_t* arena) {
  Game_t* g = Arena__Push(arena, sizeof(Game_t));
  return g;
}

void Game__init(Game_t* game, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  game->menu = TitleMenu__alloc(state->arena);
  TitleMenu__init(game->menu, state);
  ((TitleMenu_t*)game->menu)->skip = true;  // debug: skip title screen
  game->curLvl = NULL;
  game->curPlyr = NULL;
  game->lastUid = 0;

  // window
  state->WINDOW_TITLE = String__alloc(state->arena, "Retro");
  state->CANVAS_WIDTH = state->DIMS;
  state->CANVAS_HEIGHT = state->DIMS;
  state->PHYSICS_FPS = 50;
  state->RENDER_FPS = 60;

  // debug
  logic->CANVAS_DEBUG_X = state->CANVAS_WIDTH / 2.0f;
  logic->CANVAS_DEBUG_Y = state->CANVAS_HEIGHT / 2.0f;

  Bitmap__Alloc(
      state->arena,
      &logic->screen,
      state->CANVAS_WIDTH,
      state->CANVAS_HEIGHT,
      4 /*RGBA*/);
  logic->zbuf = Arena__Push(state->arena, state->CANVAS_WIDTH * state->CANVAS_HEIGHT * sizeof(f32));
  logic->zbufWall = Arena__Push(state->arena, state->CANVAS_WIDTH * sizeof(f32));

  // preload textures
  state->Vulkan__FReadImage(&logic->atlas, "../assets/textures/atlas.png");
  state->Vulkan__FReadImage(&logic->glyphs0, "../assets/textures/glyphs0.png");

  // preload audio
  state->Audio__LoadAudioFile(state->arena, state->audio, "../assets/audio/sfx/title.wav");
  state->Audio__LoadAudioFile(state->arena, state->audio, "../assets/audio/sfx/pickupCoin.wav");
  state->Audio__LoadAudioFile(state->arena, state->audio, "../assets/audio/sfx/click.wav");
  state->Audio__LoadAudioFile(state->arena, state->audio, "../assets/audio/sfx/powerUp.wav");
  state->Audio__LoadAudioFile(state->arena, state->audio, "../assets/audio/sfx/chop.wav");
  state->Audio__LoadAudioFile(state->arena, state->audio, "../assets/audio/sfx/punch.wav");
  state->Audio__LoadAudioFile(state->arena, state->audio, "../assets/audio/sfx/hurt.wav");
  state->Audio__LoadAudioFile(state->arena, state->audio, "../assets/audio/sfx/bash.wav");

  // preload meshes
  logic->game->meshes = List__alloc(state->arena);
  Wavefront_t* obj = Wavefront__parse_obj(state->arena, "../assets/models/box.obj");
  List__append(state->arena, logic->game->meshes, obj);
}

void Game__tick(Game_t* game, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  if (!state->mouseCaptured && state->mState->btn1) {
    state->mState->btn1 = false;
    state->Window__CaptureMouse(true);
    state->mouseCaptured = true;
  }

  if (state->mouseCaptured) {
    if (state->kbState->escape) {  // ESC
      state->kbState->escape = false;
      state->Window__CaptureMouse(false);
      state->mouseCaptured = false;
    }
  }

  // menu system
  if (NULL != game->menu) {
    if (NULL == game->curPlyr) {
      game->curPlyr = Player__alloc(state->arena);
      Player__init(game->curPlyr, state);
    }
    Dispatcher__engine(game->menu->tick, game->menu, state);
  }

  // in-game
  else {
    Dispatcher__engine(game->curPlyr->tick, game->curPlyr, state);
    Level__tick(logic->game->curLvl, state);
  }
}

void Game__render(Game_t* game, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  // menu system
  if (NULL != game->menu) {
    Dispatcher__engine(game->menu->render, game->menu, state);
  }

  // in-game
  else {
    Bitmap3D__RenderHorizon(state);
    Level__render(logic->game->curLvl, state);
    Bitmap3D__PostProcessing(state);
  }
}

void Game__gui(Game_t* game, Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  // menu system
  if (NULL != game->menu) {
    Dispatcher__engine(game->menu->gui, game->menu, state);
  }

  // in-game
  else {
    Level__gui(logic->game->curLvl, state);

    // game->local->CANVAS_DEBUG_X = 80;
    // game->local->CANVAS_DEBUG_Y = 40;

    // draw debug cursor
    Bitmap__Set2DPixel(
        &logic->screen,
        logic->CANVAS_DEBUG_X,
        logic->CANVAS_DEBUG_Y,
        Math__urandom() | 0xffff0000 + 0xff993399);

    Bitmap__DebugText(
        &logic->screen,
        &logic->glyphs0,
        4,
        6 * 29,
        0xffffffff,
        0,
        "cam x %+06.1f y %+06.1f z %+06.1f r %+06.1f",
        logic->game->curPlyr->transform.position.x,
        logic->game->curPlyr->transform.position.y,
        logic->game->curPlyr->transform.position.z,
        logic->game->curPlyr->transform.rotation.y);
  }
}