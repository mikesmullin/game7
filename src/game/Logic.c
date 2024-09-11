#include "../lib/Engine.h"

static Engine__State_t* state;

static bool refresh = false;

// on process start
__declspec(dllexport) void logic_boot(Engine__State_t* _state) {
  state = _state;
}

// on init (data only)
// TODO: move to external data file
__declspec(dllexport) void logic_oninit_data() {
  state->WINDOW_TITLE = "Retro";
  state->ENGINE_NAME = "MS2024";
  state->WINDOW_WIDTH = 800;
  state->WINDOW_HEIGHT = 800;

  state->PHYSICS_FPS = 50;
  state->RENDER_FPS = 60;
  state->PLAYER_WALK_SPEED = 1.0f / 3;  // per-second
  state->PLAYER_ZOOM_SPEED = 1.0f / 8;  // per-second

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

  state->textureFiles[0] = "../assets/textures/font.bmp";

  state->audioFiles[0] = "../assets/audio/sfx/pickupCoin.wav";

  state->ANIM_VIKING_IDLE_FRONT.duration = 3.0f;
  state->ANIM_VIKING_IDLE_FRONT.frameCount = 2;
  state->ANIM_VIKING_IDLE_FRONT.frames[0] = 3;
  state->ANIM_VIKING_IDLE_FRONT.frames[1] = 4;

  state->ANIM_VIKING_IDLE_LEFT.duration = 1.0f;
  state->ANIM_VIKING_IDLE_LEFT.frameCount = 1;
  state->ANIM_VIKING_IDLE_LEFT.frames[0] = 5;

  state->ANIM_VIKING_WALK_LEFT.duration = (1.0f / 3.75) * 7;
  state->ANIM_VIKING_WALK_LEFT.frameCount = 7;
  // state->ANIM_VIKING_WALK_LEFT.frames = {6, 7, 8, 9, 10, 9, 8, 7};
  state->ANIM_VIKING_WALK_LEFT.frames[0] = 6;
  state->ANIM_VIKING_WALK_LEFT.frames[1] = 7;
  state->ANIM_VIKING_WALK_LEFT.frames[2] = 8;
  state->ANIM_VIKING_WALK_LEFT.frames[3] = 9;
  state->ANIM_VIKING_WALK_LEFT.frames[4] = 8;
  state->ANIM_VIKING_WALK_LEFT.frames[5] = 7;
  state->ANIM_VIKING_WALK_LEFT.frames[6] = 10;

  state->ANIM_VIKING_WALK_FRONT = (Animation_t){
      .duration = (1.0f / 3.75) * 8,
      .frameCount = 8,
      .frames = {11, 12, 13, 14, 15, 14, 13, 12}};

  state->playerAnimationState.facing = FRONT;
  state->playerAnimationState.state = IDLE;
  state->playerAnimationState.anim = &state->ANIM_VIKING_IDLE_FRONT;

  // state->playerAnimationState.facing = LEFT;
  // state->playerAnimationState.state = WALK;
  // state->playerAnimationState.anim = &state->ANIM_VIKING_WALK_LEFT;

  // state->playerAnimationState.facing = FRONT;
  // state->playerAnimationState.state = WALK;
  // state->playerAnimationState.anim = &state->ANIM_VIKING_WALK_FRONT;

  state->playerAnimationState.frame = 0;
  state->playerAnimationState.seek = 0.0f;
}

__declspec(dllexport) void logic_oninit_compute() {
  state->Audio__LoadAudioFile(state->audioFiles[AUDIO_PICKUP_COIN]);
}

__declspec(dllexport) void logic_onreload() {
  // LOG_DEBUGF("Logic dll loaded.");
  state->Audio__ResumeAudio(AUDIO_PICKUP_COIN, false, 0.2f);

  state->instances[INSTANCE_PLAYER_1].pos[0] += 0.10;
  refresh = true;
}

__declspec(dllexport) void logic_onkey() {
  // LOG_DEBUGF("Logic dll onkey.");

  if (41 == state->g_Keyboard__state->code) {  // ESC
    state->s_Window.quit = true;
  }

  // character locomotion controls
  if (119 == state->g_Keyboard__state->location) {  // W
    state->playerAnimationState.facing = BACK;
    state->playerAnimationState.state = state->g_Keyboard__state->pressed ? WALK : IDLE;
    // state->playerAnimationState.anim = &ANIM_VIKING_WALK_BACK;
    state->playerAnimationState.anim = &state->ANIM_VIKING_WALK_FRONT;
  } else if (97 == state->g_Keyboard__state->location) {  // A
    state->playerAnimationState.facing = LEFT;
    state->playerAnimationState.state = state->g_Keyboard__state->pressed ? WALK : IDLE;
    state->playerAnimationState.anim = &state->ANIM_VIKING_WALK_LEFT;
    // instances[INSTANCE_PLAYER_1].scale[0] = +instances[INSTANCE_PLAYER_1].scale[0];
  } else if (115 == state->g_Keyboard__state->location) {  // S
    state->playerAnimationState.facing = FRONT;
    state->playerAnimationState.state = state->g_Keyboard__state->pressed ? WALK : IDLE;
    state->playerAnimationState.anim = &state->ANIM_VIKING_WALK_FRONT;
  } else if (100 == state->g_Keyboard__state->location) {  // D
    state->playerAnimationState.facing = RIGHT;
    state->playerAnimationState.state = state->g_Keyboard__state->pressed ? WALK : IDLE;
    state->playerAnimationState.anim = &state->ANIM_VIKING_WALK_LEFT;
    // instances[INSTANCE_PLAYER_1].scale[0] = -instances[INSTANCE_PLAYER_1].scale[0];
  }
  if (WALK == state->playerAnimationState.state) {
    // Audio__ResumeAudio(AUDIO_PICKUP_COIN, false, 10.0f);
  } else if (IDLE == state->playerAnimationState.state) {
    // Audio__StopAudio(AUDIO_PICKUP_COIN);

    if (BACK == state->playerAnimationState.facing) {
      // state->playerAnimationState.anim = &ANIM_VIKING_IDLE_BACK;
      state->playerAnimationState.anim = &state->ANIM_VIKING_IDLE_FRONT;
    } else if (LEFT == state->playerAnimationState.facing) {
      state->playerAnimationState.anim = &state->ANIM_VIKING_IDLE_LEFT;
    } else if (FRONT == state->playerAnimationState.facing) {
      state->playerAnimationState.anim = &state->ANIM_VIKING_IDLE_FRONT;
    } else if (RIGHT == state->playerAnimationState.facing) {
      // state->playerAnimationState.anim = &ANIM_VIKING_IDLE_RIGHT;
      state->playerAnimationState.anim = &state->ANIM_VIKING_IDLE_LEFT;
    }
  }
}

__declspec(dllexport) void logic_onfinger() {
  // LOG_DEBUGF("Logic dll onfinger.");
}

// on physics
__declspec(dllexport) void logic_onfixedupdate(const f64 deltaTime) {
  // LOG_DEBUGF("Logic dll onfixedupdate.");

  if (refresh || WALK == state->playerAnimationState.state) {
    refresh = false;
    if (LEFT == state->playerAnimationState.facing) {
      state->instances[INSTANCE_PLAYER_1].pos[0] -= state->PLAYER_WALK_SPEED * deltaTime;
    } else if (RIGHT == state->playerAnimationState.facing) {
      state->instances[INSTANCE_PLAYER_1].pos[0] += state->PLAYER_WALK_SPEED * deltaTime;
    } else if (BACK == state->playerAnimationState.facing) {
      state->instances[INSTANCE_PLAYER_1].pos[1] -= state->PLAYER_WALK_SPEED * deltaTime;
    } else if (FRONT == state->playerAnimationState.facing) {
      state->instances[INSTANCE_PLAYER_1].pos[1] += state->PLAYER_WALK_SPEED * deltaTime;
    }
    state->isVBODirty = true;

    state->world.cam[0] = state->instances[INSTANCE_PLAYER_1].pos[0];
    state->world.cam[1] = state->instances[INSTANCE_PLAYER_1].pos[1];
    state->world.look[0] = state->instances[INSTANCE_PLAYER_1].pos[0];
    state->world.look[1] = state->instances[INSTANCE_PLAYER_1].pos[1];
    state->isUBODirty[0] = true;
    state->isUBODirty[1] = true;
  }
}

// on draw
__declspec(dllexport) void logic_onupdate(const f64 deltaTime) {
  // LOG_DEBUGF("Logic dll onupdate.");
}