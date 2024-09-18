#include "Logic.h"

#include "../lib/Arena.h"
#include "../lib/Bitmap.h"
#include "../lib/Bitmap3D.h"
#include "../lib/Engine.h"
#include "../lib/Finger.h"
#include "../lib/Math.h"
#include "../lib/String.h"
#include "ui/Screen.h"

static Arena_t* arena;
static Engine__State_t* _G;
static Logic__State_t* local;

static f32 PixelsToUnits(u32 pixels) {
  return (f32)pixels / local->PIXELS_PER_UNIT;
}

static u8 Animate(AnimationState_t* state, f64 deltaTime) {
  state->seek += deltaTime;
  state->seek = Math__mod(state->seek, state->anim->duration);
  state->frame = Math__map(state->seek, 0.0f, state->anim->duration, 0, state->anim->frameCount);
  u8 texId = state->anim->frames[state->frame];
  return texId;
}

// on process start
__declspec(dllexport) void logic_onload(Arena_t* a, Engine__State_t* _state) {
  arena = a;
  _G = _state;
  local = _G->localState;
}

// on init (data only)
__declspec(dllexport) void logic_oninit_data() {
  local = Arena__Push(arena, sizeof(Logic__State_t));
  _G->localState = local;

  _G->WINDOW_TITLE = "Retro";
  _G->ENGINE_NAME = "MS2024";
  _G->WINDOW_WIDTH = 320;
  _G->WINDOW_HEIGHT = 320;

  Bitmap__Construct(
      &local->screen,
      _G->WINDOW_WIDTH,
      _G->WINDOW_HEIGHT,
      4 /*RGBA*/,
      local->screenBuf);

  _G->PHYSICS_FPS = 50;
  _G->RENDER_FPS = 60;
  local->PLAYER_WALK_SPEED = 1.0f / 3;  // per-second
  local->PLAYER_ZOOM_SPEED = 1.0f / 8;  // per-second

  local->isVBODirty = true;
  local->isUBODirty[0] = true;
  local->isUBODirty[1] = true;
  local->VEC3_Y_UP[0] = 0;
  local->VEC3_Y_UP[1] = 1;
  local->VEC3_Y_UP[2] = 0;
  local->CANVAS_WH = 100;
  local->PIXELS_PER_UNIT = local->CANVAS_WH;
  _G->instanceCount = 1;

  _G->indices[0] = 0;
  _G->indices[1] = 1;
  _G->indices[2] = 2;
  _G->indices[3] = 2;
  _G->indices[4] = 3;
  _G->indices[5] = 0;

  _G->vertices[0].vertex[0] = -0.5f;
  _G->vertices[0].vertex[1] = -0.5f;
  _G->vertices[1].vertex[0] = 0.5f;
  _G->vertices[1].vertex[1] = -0.5f;
  _G->vertices[2].vertex[0] = 0.5f;
  _G->vertices[2].vertex[1] = 0.5f;
  _G->vertices[3].vertex[0] = -0.5f;
  _G->vertices[3].vertex[1] = 0.5f;

  _G->shaderFiles[0] = "../assets/shaders/simple_shader.frag.spv";
  _G->shaderFiles[1] = "../assets/shaders/simple_shader.vert.spv";

  _G->textureFiles[0] = "../assets/textures/atlas.png";

  local->audioFiles[0] = "../assets/audio/sfx/pickupCoin.wav";

  local->newTexId = 0;
  local->debugArena = Arena__SubAlloc(arena, 1024 * 50);  // MB
}

__declspec(dllexport) void logic_oninit_compute() {
  _G->Audio__LoadAudioFile(local->audioFiles[AUDIO_PICKUP_COIN]);

  // setup scene
  glm_vec3_copy((vec3){0, 0, 1.5}, _G->world.cam);
  glm_vec3_copy((vec3){0, 0, 0}, _G->world.look);

  glm_vec3_copy((vec3){0, 0, 0}, _G->instances[INSTANCE_FLOOR_0].pos);
  glm_vec3_copy((vec3){0, 0, 0}, _G->instances[INSTANCE_FLOOR_0].rot);
  glm_vec3_copy(
      (vec3){PixelsToUnits(100), PixelsToUnits(100), 1},  // ABGR
      _G->instances[INSTANCE_FLOOR_0].scale);
  _G->instances[INSTANCE_FLOOR_0].texId = 0;
  _G->instanceCount = 1;

  local->isUBODirty[0] = true;
  local->isUBODirty[1] = true;
}

__declspec(dllexport) void logic_onreload() {
  LOG_DEBUGF("Logic dll loaded.");
  _G->Audio__ResumeAudio(AUDIO_PICKUP_COIN, false, 1.0f);

  // compose brush
  Bitmap__Construct(&local->brush, 64, 64, 4 /*RGBA*/, local->brushBuf);
  srand(_G->Time__Now());
  for (u64 i = 0; i < local->brush.w * local->brush.h; i++) {
    ((u32*)local->brush.buf)[i] = Math__urandom() * (Math__urandom2(0, 5) / 4);
  }

  // update rgba image texture
  // Bitmap_t atlas;
  // state->Vulkan__FReadImage(&atlas, state->textureFiles[0]);
  // state->Vulkan__UpdateTextureImage(&state->s_Vulkan, &atlas);
  // state->Vulkan__FCloseImage(&atlas);
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

  if (41 == _G->g_Keyboard__state->code) {  // ESC
    _G->s_Window.quit = true;
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

  if (FINGER_SCROLL == _G->g_Finger__state->event) {
    // TODO: how to animate camera zoom with spring damping/smoothing?
    // TODO: how to move this into physics callback? or is it better not to?
    _G->world.cam[2] += -_G->g_Finger__state->wheel_y * local->PLAYER_ZOOM_SPEED /* deltaTime*/;
    local->isUBODirty[0] = true;
    local->isUBODirty[1] = true;
  }
}

// on physics
__declspec(dllexport) void logic_onfixedupdate(const f64 deltaTime) {
  // LOG_DEBUGF("Logic dll onfixedupdate.");

  // state->isVBODirty = true;
  // state->isUBODirty[0] = true;
  // state->isUBODirty[1] = true;
}

static f64 accumulator2 = 0.0f;
static const f32 FPS_LOG_TIME_STEP = 1.0f;  // every second
static f64 accumulator3 = 0.0f;
static const f32 DEBUG_LOG_TIME_STEP = 60.0f;  // every 5sec
static u16 frames = 0;

s32 invert_endianness(s32 value) {
  return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000) |
         ((value << 24) & 0xFF000000);
}

// on draw
__declspec(dllexport) void logic_onupdate(const f64 deltaTime) {
  // LOG_DEBUGF("Logic dll onupdate.");

  bool onsecond = false;
  accumulator2 += deltaTime;
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
  accumulator3 += deltaTime;
  if (accumulator3 >= DEBUG_LOG_TIME_STEP) {
    on5sec = true;

    while (accumulator3 >= DEBUG_LOG_TIME_STEP) {
      accumulator3 -= DEBUG_LOG_TIME_STEP;
    }
  }

  if (local->isVBODirty) {
    local->isVBODirty = false;

    _G->s_Vulkan.m_instanceCount = _G->instanceCount;
    _G->Vulkan__UpdateVertexBuffer(&_G->s_Vulkan, 1, sizeof(_G->instances), _G->instances);
  }

  if (local->isUBODirty[_G->s_Vulkan.m_currentFrame]) {
    local->isUBODirty[_G->s_Vulkan.m_currentFrame] = false;

    glm_lookat(
        _G->world.cam,
        _G->world.look,
        local->VEC3_Y_UP,  // Y-axis points upwards (GLM default)
        _G->ubo1.view);

    _G->s_Vulkan.m_aspectRatio = _G->world.aspect;  // sync viewport

    glm_perspective(
        glm_rad(45.0f),  // half the actual 90deg fov
        _G->world.aspect,
        0.1f,  // TODO: adjust clipping range for z depth?
        10.0f,
        _G->ubo1.proj);

    // glm_ortho(-0.5f, +0.5f, -0.5f, +0.5f, 0.1f, 10.0f, ubo1.proj);
    glm_vec2_copy(_G->world.user1, _G->ubo1.user1);
    glm_vec2_copy(_G->world.user2, _G->ubo1.user2);

    // TODO: not sure i make use of one UBO per frame, really
    _G->Vulkan__UpdateUniformBuffer(&_G->s_Vulkan, _G->s_Vulkan.m_currentFrame, &_G->ubo1);
  }

  // clear frame

  for (u64 i = 0; i < local->screen.w * local->screen.h; i++) {
    ((u32*)local->screen.buf)[i] = 0;
  }

  // blit brush to frame
  // u32 xo, yo;
  // for (int i = 0; i < 100; i++) {
  //   xo = (Math__sin((_G->Time__Now() + i * 12) % 2000 / 2000.0 * Math__PI * 2) * 100);
  //   yo = (Math__cos((_G->Time__Now() + i * 12) % 2000 / 2000.0 * Math__PI * 2) * 70);
  //   Bitmap__Draw(
  //       &local->brush,
  //       &local->screen,
  //       (_G->WINDOW_WIDTH - 64) / 2 + xo,
  //       (_G->WINDOW_HEIGHT - 64) / 2 + yo);
  // }

  // try to draw 3d scene
  u32 SCALE = 1024;  // factor for fixed-point arithmetic (avoids f32 decimals)
  s32 H = 320;
  s32 W = 320;
  s32 G = 16;  // grid size
  s32 color = 0;
  s32 y = 0, yd = 0, z = 0, x = 0, xd = 0;
  s32 d = 100 * SCALE;  // distance
  Arena__Reset(local->debugArena);
  String8Node* sn = NULL;
  sn = str8n__allocf(local->debugArena, sn, "%s", 5, "===\n");

  d = Math__map(accumulator3, 0, DEBUG_LOG_TIME_STEP, 0, 10000) * SCALE;

  // tiled gradient horizon
  for (y = 0; y < H; y++) {
    yd = (y - H / 2) * H;   // -51200...51200
    if (yd == 0) continue;  // skip row to avoid divide by zero
    z = d / yd;             // (100*1024) / (-51200)
    if (z == 0) continue;   // skip row to avoid divide by zero (won't happen if SCALE > max(W,H))

    for (x = 0; x < W; x++) {
      xd = ((x - W / 2) * H);
      xd *= z;
      xd /= SCALE;
      yd /= SCALE;
      // if (y == 1) sn = str8n__allocf(local->debugArena, sn, "%+04d ", 6, xx);
      color = (xd * G) | (yd * G);
      color *= 0xff00ff00;  // AGBR
      ((u32*)local->screen.buf)[x + y * W] = color;
    }
    // if (on5sec && y == 1) str8__fputs(sn, stdout);
  }

  _G->Vulkan__UpdateTextureImage(&_G->s_Vulkan, &local->screen);
}