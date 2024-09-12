#include "../lib/Engine.h"
#include "../lib/Finger.h"
#include "../lib/Math.h"

static Engine__State_t* state;

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

  state->newTexId = 0;
}

__declspec(dllexport) void logic_oninit_compute() {
  state->Audio__LoadAudioFile(state->audioFiles[AUDIO_PICKUP_COIN]);
}

__declspec(dllexport) void logic_onreload() {
  LOG_DEBUGF("Logic dll loaded.");
  state->Audio__ResumeAudio(AUDIO_PICKUP_COIN, false, 0.2f);

  // update rgba image texture
  Vulkan__FImage_t fhandle;
  state->Vulkan__FReadImage(&fhandle, state->textureFiles[0]);
  state->Vulkan__UpdateTextureImage(&state->s_Vulkan, &fhandle);
  state->Vulkan__FCloseImage(&fhandle);

  // setup scene
  glm_vec3_copy((vec3){0, 0, 1.5}, state->world.cam);
  glm_vec3_copy((vec3){0, 0, 0}, state->world.look);

  glm_vec3_copy((vec3){0, 0, 0}, state->instances[INSTANCE_FLOOR_0].pos);
  glm_vec3_copy((vec3){0, 0, 0}, state->instances[INSTANCE_FLOOR_0].rot);
  glm_vec3_copy(
      (vec3){PixelsToUnits(100), PixelsToUnits(100), 1},
      state->instances[INSTANCE_FLOOR_0].scale);
  state->instances[INSTANCE_FLOOR_0].texId = 0;
  state->instanceCount = 1;

  state->isUBODirty[0] = true;
  state->isUBODirty[1] = true;
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

  // state->isVBODirty = true;
  // state->isUBODirty[0] = true;
  // state->isUBODirty[1] = true;
}

static Vulkan__FImage_t fhandle;
static u8 rgbabuf[40000];

// on draw
__declspec(dllexport) void logic_onupdate(const f64 deltaTime) {
  // LOG_DEBUGF("Logic dll onupdate.");

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

  // spam RGBA pixel updates to texture buffer
  fhandle.texWidth = 100;
  fhandle.texHeight = 100;
  fhandle.imageSize = 100 * 100 * 4;  // RGBA
  fhandle.pixels = rgbabuf;
  for (u64 i = 0; i < fhandle.imageSize; i += 4) {
    // colorful static
    // rgbabuf[i] = (u8)Math__urandom(0, 255);
    // rgbabuf[i + 1] = (u8)Math__urandom(0, 255);
    // rgbabuf[i + 2] = (u8)Math__urandom(0, 255);
    // rgbabuf[i + 3] = 255;

    // grayscale static
    rgbabuf[i] = (u8)Math__urandom(0, 255);
    rgbabuf[i + 1] = rgbabuf[i];
    rgbabuf[i + 2] = rgbabuf[i];
    rgbabuf[i + 3] = 255;

    // solid random
    // rgbabuf[i] = (0 == i ? (u8)Math__urandom(0, 255) : rgbabuf[0]);
    // rgbabuf[i + 1] = (0 == i ? (u8)Math__urandom(0, 255) : rgbabuf[1]);
    // rgbabuf[i + 2] = (0 == i ? (u8)Math__urandom(0, 255) : rgbabuf[2]);
    // rgbabuf[i + 3] = 255;

    // solid white
    // rgbabuf[i] = 255;

    // white flashing
    // rgbabuf[i] = (0 == i ? (u8)Math__urandom(0, 255) : rgbabuf[0]);
    // rgbabuf[i + 1] = rgbabuf[i];
    // rgbabuf[i + 2] = rgbabuf[i];
    // rgbabuf[i + 3] = 255;
  }
  state->Vulkan__UpdateTextureImage(&state->s_Vulkan, &fhandle);
}