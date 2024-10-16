#include "Logic.h"

#include <stdio.h>
#include <string.h>

#include "../lib/Arena.h"
#include "../lib/Engine.h"
#include "../lib/Finger.h"
#include "../lib/List.h"
#include "../lib/Log.h"
#include "../lib/Math.h"
#include "../lib/String.h"
#include "Game.h"
#include "entities/Player.h"

Engine__State_t* g_engine;

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

  // vulkan
  state->shaderFiles = List__alloc(state->arena);
  List__append(state->arena, state->shaderFiles, "../assets/shaders/simple_shader.frag.spv");
  List__append(state->arena, state->shaderFiles, "../assets/shaders/simple_shader.vert.spv");

  logic->game = Game__alloc(state->arena);
  Game__init(logic->game, state);
}

__declspec(dllexport) void logic_oninit_compute(Engine__State_t* state) {
  Logic__State_t* logic = state->local;

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
  g_engine = state;
  LOG_DEBUGF("Logic dll loaded state %p at %p", g_engine, &g_engine);

  if (!state->dllLoadedOnce) {
    state->dllLoadedOnce = true;
    // doesn't play audio the first time proc runs
  } else {
    state->Audio__ResumeAudio(state->audio, AUDIO_HURT, false, 1.0f, 0);
  }

  // play player hurt animation (a visual queue on top of audio queue is nice when muted)
  Logic__State_t* logic = state->local;
  Player_t* player = (Player_t*)logic->game->curPlyr;
  if (NULL != player) {
    player->base.health->hurtTime = PLAYER_HURT_ANIM_TIME;
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
static u16 frames = 0;

// on draw
__declspec(dllexport) void logic_onupdate(Engine__State_t* state) {
  Logic__State_t* logic = state->local;

  bool onsecond = false;
  accumulator2 += state->deltaTime;
  frames++;
  if (accumulator2 >= FPS_LOG_TIME_STEP) {
    onsecond = true;

    static char title[100];
    sprintf(
        title,
        "%s | FPS %u P %u R %u pFPS %u A %llu/%lluMB",
        state->WINDOW_TITLE->str,
        frames,  // FPS = measured/counted frames per second
        state->costPhysics,  // P = cost of last physics in ms
        state->costRender,  // R = cost of last render in ms
        // pFPS = potential frames per second (if it wasn't fixed)
        1000 / (state->costPhysics + state->costRender + 1),  // +1 avoids div/0
        // A = Arena memory used/capacity
        ((u64)(state->arena->pos - state->arena->buf)) / 1024 / 1024,
        ((u64)(state->arena->end - state->arena->buf)) / 1024 / 1024);
    state->Window__SetTitle(state->window, title);
    frames = 0;

    while (accumulator2 >= FPS_LOG_TIME_STEP) {
      accumulator2 -= FPS_LOG_TIME_STEP;
    }
  }

  if (logic->isVBODirty) {
    logic->isVBODirty = false;

    state->VulkanWrapper__SetInstanceCount(state->instanceCount);
    state->VulkanWrapper__UpdateVertexBuffer(1, sizeof(state->instances), state->instances);
  }

  u8 frame = state->VulkanWrapper__GetCurrentFrame();
  if (logic->isUBODirty[frame]) {
    logic->isUBODirty[frame] = false;

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

    // TODO: not sure i need one UBO per frame, really
    state->VulkanWrapper__UpdateUniformBuffer(frame, &state->ubo1);
  }

  Game__render(logic->game, state);
  Game__gui(logic->game, state);

  state->VulkanWrapper__UpdateTextureImage(&logic->screen);
}
