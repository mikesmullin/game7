#include <stdio.h>
#include <windows.h>

#include "../lib/Engine.h"

__declspec(dllexport) void logic_init(Engine__State_t* state) {  // on init, before systems
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

  state->ANIM_VIKING_WALK_FRONT.duration = (1.0f / 3.75) * 8;
  state->ANIM_VIKING_WALK_FRONT.frameCount = 8;
  state->ANIM_VIKING_WALK_FRONT.frames[0] = 11;
  state->ANIM_VIKING_WALK_FRONT.frames[1] = 12;
  state->ANIM_VIKING_WALK_FRONT.frames[2] = 13;
  state->ANIM_VIKING_WALK_FRONT.frames[3] = 14;
  state->ANIM_VIKING_WALK_FRONT.frames[4] = 15;
  state->ANIM_VIKING_WALK_FRONT.frames[5] = 14;
  state->ANIM_VIKING_WALK_FRONT.frames[6] = 13;
  state->ANIM_VIKING_WALK_FRONT.frames[7] = 12;

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

__declspec(dllexport) void logic_init2(Engine__State_t* state) {  // on init, after systems
  // Audio__LoadAudioFile(state->audioFiles[AUDIO_PICKUP_COIN]);
  // Audio__PlayAudio(AUDIO_PICKUP_COIN, false, 1.0f);
}

__declspec(dllexport) void logic_init3(Engine__State_t* state) {  // on reload
  printf("Logic dll loaded!! %d\n", state->pFreeClass(2));
}

__declspec(dllexport) void logic_draw(const Engine__State_t* state) {
}

__declspec(dllexport) void logic_update(Engine__State_t* state) {
}
