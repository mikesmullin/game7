#include "Logic.h"

#include "../lib/Arena.h"
#include "../lib/Array.h"
#include "../lib/Bitmap.h"
#include "../lib/Engine.h"
#include "../lib/Finger.h"
#include "../lib/Log.h"
#include "../lib/Math.h"
#include "Game.h"

// on process start
__declspec(dllexport) void logic_onload(Engine__State_t* state) {
}

// on init (data only)
__declspec(dllexport) void logic_oninit_data(Engine__State_t* state) {
  state->local = Arena__Push(state->arena, sizeof(Logic__State_t));
  Logic__State_t* logic = state->local;

  state->WINDOW_TITLE = "Retro";
  state->ENGINE_NAME = "MS2024";
  u32 dim = 180;
  state->CANVAS_WIDTH = dim;
  state->CANVAS_HEIGHT = dim;
  state->WINDOW_WIDTH = dim * 4;
  state->WINDOW_HEIGHT = dim * 4;
  logic->PIXELS_PER_UNIT = 100;

  logic->WORLD_HEIGHT = 4.0f;  // world height
  logic->ATLAS_TILE_SIZE = 8.0f;

  logic->CANVAS_DEBUG_X = state->CANVAS_WIDTH / 2.0f;
  logic->CANVAS_DEBUG_Y = state->CANVAS_HEIGHT / 2.0f;

  state->PHYSICS_FPS = 50;
  state->RENDER_FPS = 60;
  logic->PLAYER_WALK_SPEED = 1.5f;  // per-second
  logic->PLAYER_FLY_SPEED = 0.5f;   // per-second
  logic->PLAYER_ZOOM_SPEED = 0.1f;  // per-second
  logic->PLAYER_LOOK_SPEED = 0.1f;  // deg/sec

  logic->isVBODirty = true;
  logic->isUBODirty[0] = true;
  logic->isUBODirty[1] = true;
  logic->VEC3_Y_UP[0] = 0;
  logic->VEC3_Y_UP[1] = 1;
  logic->VEC3_Y_UP[2] = 0;
  state->instanceCount = 1;

  state->indices[0] = 0;
  state->indices[1] = 1;
  state->indices[2] = 2;
  state->indices[3] = 2;
  state->indices[4] = 3;
  state->indices[5] = 0;

  state->vertices[0].vertex[0] = -0.5f;
  state->vertices[0].vertex[1] = -0.5f;
  state->vertices[1].vertex[0] = 0.5f;
  state->vertices[1].vertex[1] = -0.5f;
  state->vertices[2].vertex[0] = 0.5f;
  state->vertices[2].vertex[1] = 0.5f;
  state->vertices[3].vertex[0] = -0.5f;
  state->vertices[3].vertex[1] = 0.5f;

  state->shaderFiles[0] = "../assets/shaders/simple_shader.frag.spv";
  state->shaderFiles[1] = "../assets/shaders/simple_shader.vert.spv";

  logic->audioFiles[AUDIO_TITLE] = "../assets/audio/sfx/title.wav";
  logic->audioFiles[AUDIO_PICKUP_COIN] = "../assets/audio/sfx/pickupCoin.wav";
  logic->audioFiles[AUDIO_CLICK] = "../assets/audio/sfx/click.wav";
  logic->audioFiles[AUDIO_POWERUP] = "../assets/audio/sfx/powerUp.wav";

  logic->newTexId = 0;
}

__declspec(dllexport) void logic_oninit_compute(Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  Bitmap__Alloc(
      state->arena,
      &logic->screen,
      state->CANVAS_WIDTH,
      state->CANVAS_HEIGHT,
      4 /*RGBA*/);
  logic->zbuf = Arena__Push(state->arena, state->CANVAS_WIDTH * state->CANVAS_HEIGHT * sizeof(f32));
  logic->zbufWall = Arena__Push(state->arena, state->CANVAS_WIDTH * sizeof(f32));
  logic->debugArena = Arena__SubAlloc(state->arena, 1024 * 50);  // MB

  // load textures
  state->Vulkan__FReadImage(&logic->atlas, "../assets/textures/atlas.png");
  state->Vulkan__FReadImage(&logic->glyphs0, "../assets/textures/glyphs0.png");

  // load audio
  for (u32 i = 0; i < ARRAY_COUNT(logic->audioFiles); i++) {
    state->Audio__LoadAudioFile(logic->audioFiles[i]);
  }

  // setup scene
  glms_vec3_copy((vec3){0, 0, 1.5}, state->world.cam);
  glms_vec3_copy((vec3){0, 0, 0}, state->world.look);

  glms_vec3_copy((vec3){0, 0, 0}, state->instances[INSTANCE_FLOOR_0].pos);
  glms_vec3_copy((vec3){0, 0, 0}, state->instances[INSTANCE_FLOOR_0].rot);
  glms_vec3_copy(
      (vec3){100 / logic->PIXELS_PER_UNIT, 100 / logic->PIXELS_PER_UNIT, 1},  // ABGR
      state->instances[INSTANCE_FLOOR_0].scale);
  state->instances[INSTANCE_FLOOR_0].texId = 0;
  state->instanceCount = 1;

  logic->isUBODirty[0] = true;
  logic->isUBODirty[1] = true;

  logic->game = Game__alloc(state->arena);
  Game__init(logic->game, state);
}

__declspec(dllexport) void logic_onreload(Engine__State_t* state) {
  LOG_DEBUGF("Logic dll loaded.");

  if (!state->dllLoadedOnce) {
    state->dllLoadedOnce = true;
    // doesn't play audio the first time proc runs
  } else {
    state->Audio__ResumeAudio(AUDIO_PICKUP_COIN, false, 1.0f);
  }
}

// on physics
__declspec(dllexport) void logic_onfixedupdate(Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  if (0 != state->mState->wheely) {
    // TODO: how to animate camera zoom with spring damping/smoothing?
    // TODO: how to move this into physics callback? or is it better not to?
    state->world.cam[2] += -state->mState->wheely * logic->PLAYER_ZOOM_SPEED /* deltaTime*/;

    state->mState->wheely = 0;
    logic->isUBODirty[0] = true;
    logic->isUBODirty[1] = true;
  }

  Game__tick(logic->game, state);
}

static f64 accumulator2 = 0.0f;
static const f32 FPS_LOG_TIME_STEP = 1.0f;  // every second
static f64 accumulator3 = 0.0f;
static const f32 DEBUG_LOG_TIME_STEP = 1 * 60.0f;  // every 5sec
static u16 frames = 0;

// on draw
__declspec(dllexport) void logic_onupdate(Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  bool onsecond = false;
  accumulator2 += state->deltaTime;
  frames++;
  if (accumulator2 >= FPS_LOG_TIME_STEP) {
    onsecond = true;
    // LOG_DEBUGF("%dfps", frames);
    frames = 0;

    while (accumulator2 >= FPS_LOG_TIME_STEP) {
      accumulator2 -= FPS_LOG_TIME_STEP;
    }
  }

  bool on5sec = false;
  accumulator3 += state->deltaTime;
  if (accumulator3 >= DEBUG_LOG_TIME_STEP) {
    on5sec = true;

    while (accumulator3 >= DEBUG_LOG_TIME_STEP) {
      accumulator3 -= DEBUG_LOG_TIME_STEP;
    }
  }

  if (logic->isVBODirty) {
    logic->isVBODirty = false;

    state->VulkanWrapper__SetInstanceCount(state->instanceCount);
    state->VulkanWrapper__UpdateVertexBuffer(1, sizeof(state->instances), state->instances);
  }

  if (logic->isUBODirty[state->VulkanWrapper__GetCurrentFrame()]) {
    logic->isUBODirty[state->VulkanWrapper__GetCurrentFrame()] = false;

    // 3d cam
    glms_lookat(
        state->world.cam,
        state->world.look,
        logic->VEC3_Y_UP,  // Y-axis points upwards (GLM default)
        state->ubo1.view);

    state->VulkanWrapper__SetAspectRatio(state->world.aspect);  // sync viewport

    glms_perspective(
        glms_rad(45.0f),  // half the actual 90deg fov
        state->world.aspect,
        0.1f,  // TODO: adjust clipping range for z depth?
        10.0f,
        state->ubo1.proj);

    // glms_ortho(-0.5f, +0.5f, -0.5f, +0.5f, 0.1f, 10.0f, ubo1.proj);
    glms_vec2_copy(state->world.user1, state->ubo1.user1);
    glms_vec2_copy(state->world.user2, state->ubo1.user2);

    // TODO: not sure i make use of one UBO per frame, really
    state->VulkanWrapper__UpdateUniformBuffer(
        state->VulkanWrapper__GetCurrentFrame(),
        &state->ubo1);
  }

  Game__render(logic->game, state);

  state->VulkanWrapper__UpdateTextureImage(&logic->screen);
}
