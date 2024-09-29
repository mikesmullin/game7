#include "Logic.h"

#include <string.h>

#include "../lib/Arena.h"
#include "../lib/Bitmap.h"
#include "../lib/Engine.h"
#include "../lib/Finger.h"
#include "../lib/List.h"
#include "../lib/Log.h"
#include "../lib/Math.h"
#include "Game.h"

// on init (data only)
__declspec(dllexport) void logic_oninit_data(Engine__State_t* state) {
  state->local = Arena__Push(state->arena, sizeof(Logic__State_t));
  Logic__State_t* logic = state->local;

  state->ENGINE_NAME = "MS2024";
  state->DIMS = 180;
  state->WINDOW_WIDTH = state->DIMS * 4;
  state->WINDOW_HEIGHT = state->DIMS * 4;
  logic->PIXELS_PER_UNIT = 100;
  logic->PLAYER_ZOOM_SPEED = 0.1f;  // per-second

  logic->isVBODirty = true;
  logic->isUBODirty[0] = true;
  logic->isUBODirty[1] = true;
  state->instanceCount = 1;

  memcpy(state->indices, (u16[]){0, 1, 2, 2, 3, 0}, sizeof(u16) * 6);
  f32 u = 1.0f / 2;  // half a unit square
  memcpy(state->vertices, (vec2[]){{-u, -u}, {u, -u}, {u, u}, {-u, u}}, sizeof(vec2) * 4);

  state->shaderFiles = List__alloc(state->arena);
  List__append(state->arena, state->shaderFiles, "../assets/shaders/simple_shader.frag.spv");
  List__append(state->arena, state->shaderFiles, "../assets/shaders/simple_shader.vert.spv");

  logic->game = Game__alloc(state->arena);
  Game__init(logic->game, state);
}

__declspec(dllexport) void logic_oninit_compute(Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  // preload audio assets
  List__Node_t* node = logic->audioFiles->head;
  for (u32 i = 0; i < logic->audioFiles->len; i++) {
    state->Audio__LoadAudioFile(node->data);
    node = node->next;
  }

  // Vulkan scene
  glms_vec3_copy((vec3){0, 0, 1.5}, state->world.cam);
  glms_vec3_copy((vec3){0, 0, 0}, state->world.look);

  glms_vec3_copy((vec3){0, 0, 0}, state->instances[INSTANCE_QUAD1].pos);
  glms_vec3_copy((vec3){0, 0, 0}, state->instances[INSTANCE_QUAD1].rot);
  f32 n = 100.0f / logic->PIXELS_PER_UNIT;
  glms_vec3_copy((vec3){n, n, 1}, state->instances[INSTANCE_QUAD1].scale);
  state->instances[INSTANCE_QUAD1].texId = 0;
  state->instanceCount = 1;

  logic->isUBODirty[0] = true;
  logic->isUBODirty[1] = true;
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
        (vec3){0, 1, 0},  // Y-axis points upwards (GLM default)
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
