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
  ((TitleMenu_t*)game->menu)->skip = false;  // debug: skip title screen
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

  Arena_t* a = state->arena;
  Bitmap__Alloc(a, &logic->screen, state->CANVAS_WIDTH, state->CANVAS_HEIGHT, 4 /*RGBA*/);
  logic->zbuf = Arena__Push(a, state->CANVAS_WIDTH * state->CANVAS_HEIGHT * sizeof(f32));
  logic->zbufWall = Arena__Push(a, state->CANVAS_WIDTH * sizeof(f32));

  char* ASSETS_DIR = Path__join(a, "..", "assets", 0);

  // preload textures
  char* TEXTURES_DIR = Path__join(a, ASSETS_DIR, "textures", 0);
  state->Vulkan__FReadImage(&logic->atlas, Path__join(a, TEXTURES_DIR, "atlas.png", 0));
  state->Vulkan__FReadImage(&logic->glyphs0, Path__join(a, TEXTURES_DIR, "glyphs0.png", 0));

  // preload audio
  char* AUDIO_DIR = Path__join(a, ASSETS_DIR, "audio", 0);
  char* SFX_DIR = Path__join(a, AUDIO_DIR, "sfx", 0);
  state->Audio__LoadAudioFile(a, state->audio, Path__join(a, SFX_DIR, "title.wav", 0));
  state->Audio__LoadAudioFile(a, state->audio, Path__join(a, SFX_DIR, "pickupCoin.wav", 0));
  state->Audio__LoadAudioFile(a, state->audio, Path__join(a, SFX_DIR, "click.wav", 0));
  state->Audio__LoadAudioFile(a, state->audio, Path__join(a, SFX_DIR, "powerUp.wav", 0));
  state->Audio__LoadAudioFile(a, state->audio, Path__join(a, SFX_DIR, "chop.wav", 0));
  state->Audio__LoadAudioFile(a, state->audio, Path__join(a, SFX_DIR, "punch.wav", 0));
  state->Audio__LoadAudioFile(a, state->audio, Path__join(a, SFX_DIR, "hurt.wav", 0));
  state->Audio__LoadAudioFile(a, state->audio, Path__join(a, SFX_DIR, "bash.wav", 0));

  // preload meshes
  char* MODELS_DIR = Path__join(a, ASSETS_DIR, "models", 0);
  logic->game->meshes = List__alloc(a);
  Wavefront_t* obj = Wavefront__parse_obj(a, Path__join(a, MODELS_DIR, "box.obj", 0));
  List__append(a, logic->game->meshes, obj);
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
    Dispatcher__engine(game->curPlyr->engine->tick, game->curPlyr, state);
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
        logic->game->curPlyr->tform->pos.x,
        logic->game->curPlyr->tform->pos.y,
        logic->game->curPlyr->tform->pos.z,
        logic->game->curPlyr->tform->rot.y);
  }
}