#include "../lib/Engine.h"
#include "../lib/Finger.h"
#include "../lib/Math.h"

static Engine__State_t* state;

static bool refresh = false;

static f32 PixelsToUnits(u32 pixels) {
  return (f32)pixels / state->PIXELS_PER_UNIT;
}

static u8 Animate(AnimationState_t* state, f64 deltaTime) {
  state->seek += deltaTime;
  state->seek = Math__mod(state->seek, state->anim->duration);
  state->frame = Math__map(state->seek, 0.0f, state->anim->duration, 0, state->anim->frameCount);
  u8 texId = state->anim->frames[state->frame];
  return texId;
}

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

  state->isVBODirty = true;
  state->isUBODirty[0] = true;
  state->isUBODirty[1] = true;
  state->VEC3_Y_UP[0] = 0;
  state->VEC3_Y_UP[1] = 1;
  state->VEC3_Y_UP[2] = 0;
  state->CANVAS_WH = 100;
  state->PIXELS_PER_UNIT = state->CANVAS_WH;
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

  state->newTexId = 0;
}

__declspec(dllexport) void logic_oninit_compute() {
  state->Audio__LoadAudioFile(state->audioFiles[AUDIO_PICKUP_COIN]);
}

__declspec(dllexport) void logic_onreload() {
  // LOG_DEBUGF("Logic dll loaded.");
  state->Audio__ResumeAudio(AUDIO_PICKUP_COIN, false, 0.2f);

  // setup scene
  glm_vec3_copy((vec3){0, 0, 1}, state->world.cam);
  glm_vec3_copy((vec3){0, 0, 0}, state->world.look);

  glm_vec3_copy((vec3){0, 0, 0}, state->instances[INSTANCE_FLOOR_0].pos);
  glm_vec3_copy((vec3){0, 0, 0}, state->instances[INSTANCE_FLOOR_0].rot);
  glm_vec3_copy(
      (vec3){PixelsToUnits(100), PixelsToUnits(100), 1},
      state->instances[INSTANCE_FLOOR_0].scale);
  state->instances[INSTANCE_FLOOR_0].texId = 0;
  state->instanceCount = 1;

  glm_vec3_copy((vec3){0, 0, 0}, state->instances[INSTANCE_PLAYER_1].pos);
  glm_vec3_copy((vec3){0, 0, 0}, state->instances[INSTANCE_PLAYER_1].rot);
  glm_vec3_copy(
      (vec3){PixelsToUnits(48), PixelsToUnits(48), 1},
      state->instances[INSTANCE_PLAYER_1].scale);
  state->instances[INSTANCE_PLAYER_1].texId = 4;
  state->instanceCount = 2;

  state->instances[INSTANCE_PLAYER_1].pos[0] += 0.10;
  refresh = true;
}

__declspec(dllexport) void logic_onkey() {
  // LOG_DEBUGF(
  //     "SDL_KEY{UP,DOWN} state "
  //     "code %u location %u pressed %u alt %u "
  //     "ctrl %u shift %u meta %u",
  //     state->g_Keyboard__state->code,
  //     state->g_Keyboard__state->location,
  //     state->g_Keyboard__state->pressed,
  //     state->g_Keyboard__state->altKey,
  //     state->g_Keyboard__state->ctrlKey,
  //     state->g_Keyboard__state->shiftKey,
  //     state->g_Keyboard__state->metaKey);

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
  // LOG_DEBUGF(
  //     "SDL_FINGER state "
  //     "event %s "
  //     "clicks %u pressure %2.5f finger %u "
  //     "x %u y %u x_rel %d y_rel %d wheel_x %2.5f wheel_y %2.5f "
  //     "button_l %d button_m %d button_r %d button_x1 %d button_x2 %d ",
  //     (state->g_Finger__state->event == FINGER_UP       ? "UP"
  //      : state->g_Finger__state->event == FINGER_DOWN   ? "DOWN"
  //      : state->g_Finger__state->event == FINGER_MOVE   ? "MOVE"
  //      : state->g_Finger__state->event == FINGER_SCROLL ? "SCROLL"
  //                                        : ""),
  //     state->g_Finger__state->clicks,
  //     state->g_Finger__state->pressure,
  //     state->g_Finger__state->finger,
  //     state->g_Finger__state->x,
  //     state->g_Finger__state->y,
  //     state->g_Finger__state->x_rel,
  //     state->g_Finger__state->y_rel,
  //     state->g_Finger__state->wheel_x,
  //     state->g_Finger__state->wheel_y,
  //     state->g_Finger__state->button_l,
  //     state->g_Finger__state->button_m,
  //     state->g_Finger__state->button_r,
  //     state->g_Finger__state->button_x1,
  //     state->g_Finger__state->button_x2);

  if (FINGER_SCROLL == state->g_Finger__state->event) {
    // TODO: how to animate camera zoom with spring damping/smoothing?
    // TODO: how to move this into physics callback? or is it better not to?
    state->world.cam[2] +=
        -state->g_Finger__state->wheel_y * state->PLAYER_ZOOM_SPEED /* deltaTime*/;
    state->isUBODirty[0] = true;
    state->isUBODirty[1] = true;
  }

  else if (false && FINGER_DOWN == state->g_Finger__state->event) {
    // TODO: how to move this into physics callback? or is it better not to?
    // TODO: animate player walk-to, before placing-down
    // TODO: convert window x,y to world x,y

    vec3 pos = (vec3){state->g_Finger__state->x, state->g_Finger__state->y, 0.0f};
    mat4 pvMatrix;
    glm_mat4_mul(state->ubo1.proj, state->ubo1.view, pvMatrix);
    vec4 viewport = (vec4){0, 0, state->s_Window.width, state->s_Window.height};
    vec3 dest;
    glm_unproject(pos, pvMatrix, viewport, dest);

    state->instances[state->instanceCount].pos[0] = dest[0];
    state->instances[state->instanceCount].pos[1] = dest[1];
    state->instances[state->instanceCount].pos[2] = 0.0f;  // dest[2];

    state->instances[state->instanceCount].scale[0] = PixelsToUnits(350 / 2);
    state->instances[state->instanceCount].scale[1] = PixelsToUnits(420 / 2);
    state->instances[state->instanceCount].scale[2] = 1.0f;
    state->instances[state->instanceCount].texId = 2;  // wood-wall 1
    state->instanceCount++;
    state->isVBODirty = true;

    // Audio__PlayAudio(AUDIO_PICKUP_COIN, false, 1.0f);
  }
}

static f64 accumulator1 = 0.0f;
static const f32 FILE_CHECK_MONITOR_TIME_STEP = 1.0f / 4;  // 4 checks per second

// on physics
__declspec(dllexport) void logic_onfixedupdate(const f64 deltaTime) {
  // LOG_DEBUGF("Logic dll onfixedupdate.");

  accumulator1 += deltaTime;
  if (accumulator1 >= FILE_CHECK_MONITOR_TIME_STEP) {
    if (state->check_load_logic()) {
      logic_onreload();
    }

    while (accumulator1 >= FILE_CHECK_MONITOR_TIME_STEP) {
      accumulator1 -= FILE_CHECK_MONITOR_TIME_STEP;
    }
  }

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

  // character frame animation
  state->newTexId = Animate(&state->playerAnimationState, 0 /*deltaTime*/);
  if (state->instances[1].texId != state->newTexId) {
    state->instances[1].texId = state->newTexId;
    state->isVBODirty = true;
  }

  if (state->isVBODirty) {
    state->isVBODirty = false;

    state->s_Vulkan.m_instanceCount = state->instanceCount;
    state->Vulkan__UpdateVertexBuffer(
        &state->s_Vulkan,
        1,
        sizeof(state->instances),
        state->instances);
  }

  if (state->isUBODirty[state->s_Vulkan.m_currentFrame]) {
    state->isUBODirty[state->s_Vulkan.m_currentFrame] = false;

    glm_lookat(
        state->world.cam,
        state->world.look,
        state->VEC3_Y_UP,  // Y-axis points upwards (GLM default)
        state->ubo1.view);

    state->s_Vulkan.m_aspectRatio = state->world.aspect;  // sync viewport

    glm_perspective(
        glm_rad(45.0f),  // half the actual 90deg fov
        state->world.aspect,
        0.1f,  // TODO: adjust clipping range for z depth?
        10.0f,
        state->ubo1.proj);

    // glm_ortho(-0.5f, +0.5f, -0.5f, +0.5f, 0.1f, 10.0f, ubo1.proj);
    glm_vec2_copy(state->world.user1, state->ubo1.user1);
    glm_vec2_copy(state->world.user2, state->ubo1.user2);

    // TODO: not sure i make use of one UBO per frame, really
    state->Vulkan__UpdateUniformBuffer(
        &state->s_Vulkan,
        state->s_Vulkan.m_currentFrame,
        &state->ubo1);
  }
}