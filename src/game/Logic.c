#include "Logic.h"

#include "../lib/Arena.h"
#include "../lib/Bitmap.h"
#include "../lib/Bitmap3D.h"
#include "../lib/Engine.h"
#include "../lib/Finger.h"
#include "../lib/Math.h"
#include "Player.h"

static Engine__State_t* game;

static f32 PixelsToUnits(u32 pixels) {
  return (f32)pixels / game->local->PIXELS_PER_UNIT;
}

static u8 Animate(AnimationState_t* state, f64 deltaTime) {
  state->seek += deltaTime;
  state->seek = Math__mod(state->seek, state->anim->duration);
  state->frame = Math__map(state->seek, 0.0f, state->anim->duration, 0, state->anim->frameCount);
  u8 texId = state->anim->frames[state->frame];
  return texId;
}

void LoadTextures() {
  game->Vulkan__FReadImage(&game->local->atlas, "../assets/textures/atlas.png");
}

// on process start
__declspec(dllexport) void logic_onload(Engine__State_t* _state) {
  game = _state;
  LOG_DEBUGF("onload projection %p", game->local->player.camera.projection);
}

// on init (data only)
__declspec(dllexport) void logic_oninit_data() {
  game->local = Arena__Push(game->arena, sizeof(Logic__State_t));
  game->local->arena = game->arena;
  Player__Init(game->local);
  LOG_DEBUGF("oninit projection %p", game->local->player.camera.projection);

  game->WINDOW_TITLE = "Retro";
  game->ENGINE_NAME = "MS2024";
  u32 dim = 180;
  game->CANVAS_WIDTH = dim;
  game->CANVAS_HEIGHT = dim;
  game->WINDOW_WIDTH = dim * 4;
  game->WINDOW_HEIGHT = dim * 4;

  game->PHYSICS_FPS = 50;
  game->RENDER_FPS = 60;
  game->local->PLAYER_WALK_SPEED = 3.0f;                                // per-second
  game->local->PLAYER_TURN_SPEED = 1.0f;                                // per-second
  game->local->PLAYER_ZOOM_SPEED = 2 * game->local->PLAYER_WALK_SPEED;  // per-second
  game->local->PLAYER_LOOK_SPEED = 0.3f;                                // per-second

  game->local->isVBODirty = true;
  game->local->isUBODirty[0] = true;
  game->local->isUBODirty[1] = true;
  game->local->VEC3_Y_UP[0] = 0;
  game->local->VEC3_Y_UP[1] = 1;
  game->local->VEC3_Y_UP[2] = 0;
  game->local->CANVAS_WH = 100;
  game->local->PIXELS_PER_UNIT = game->local->CANVAS_WH;
  game->instanceCount = 1;

  game->indices[0] = 0;
  game->indices[1] = 1;
  game->indices[2] = 2;
  game->indices[3] = 2;
  game->indices[4] = 3;
  game->indices[5] = 0;

  game->vertices[0].vertex[0] = -0.5f;
  game->vertices[0].vertex[1] = -0.5f;
  game->vertices[1].vertex[0] = 0.5f;
  game->vertices[1].vertex[1] = -0.5f;
  game->vertices[2].vertex[0] = 0.5f;
  game->vertices[2].vertex[1] = 0.5f;
  game->vertices[3].vertex[0] = -0.5f;
  game->vertices[3].vertex[1] = 0.5f;

  game->shaderFiles[0] = "../assets/shaders/simple_shader.frag.spv";
  game->shaderFiles[1] = "../assets/shaders/simple_shader.vert.spv";

  game->local->audioFiles[0] = "../assets/audio/sfx/pickupCoin.wav";

  game->local->newTexId = 0;
}

__declspec(dllexport) void logic_oninit_compute() {
  Bitmap__Alloc(
      game->arena,
      &game->local->screen,
      game->CANVAS_WIDTH,
      game->CANVAS_HEIGHT,
      4 /*RGBA*/);
  game->local->zbuf = Arena__Push(game->arena, game->CANVAS_WIDTH * game->CANVAS_HEIGHT);

  game->local->debugArena = Arena__SubAlloc(game->arena, 1024 * 50);  // MB

  LoadTextures();

  game->Audio__LoadAudioFile(game->local->audioFiles[AUDIO_PICKUP_COIN]);

  // setup scene
  glm_vec3_copy((vec3){0, 0, 1.5}, game->world.cam);
  glm_vec3_copy((vec3){0, 0, 0}, game->world.look);

  glm_vec3_copy((vec3){0, 0, 0}, game->instances[INSTANCE_FLOOR_0].pos);
  glm_vec3_copy((vec3){0, 0, 0}, game->instances[INSTANCE_FLOOR_0].rot);
  glm_vec3_copy(
      (vec3){PixelsToUnits(100), PixelsToUnits(100), 1},  // ABGR
      game->instances[INSTANCE_FLOOR_0].scale);
  game->instances[INSTANCE_FLOOR_0].texId = 0;
  game->instanceCount = 1;

  game->local->isUBODirty[0] = true;
  game->local->isUBODirty[1] = true;
}

__declspec(dllexport) void logic_onreload() {
  LOG_DEBUGF("Logic dll loaded.");
  game->Audio__ResumeAudio(AUDIO_PICKUP_COIN, false, 1.0f);

  // compose brush
  // Bitmap__Alloc(&local->brush, 64, 64, 4 /*RGBA*/);
  // srand(_G->Time__Now());
  // for (u64 i = 0; i < local->brush.w * local->brush.h; i++) {
  //   ((u32*)local->brush.buf)[i] = Math__urandom() * (Math__urandom2(0, 5) / 4);
  // }

  // update rgba image texture
  // Bitmap_t atlas;
  // state->Vulkan__FReadImage(&atlas, state->textureFiles[0]);
  // state->Vulkan__UpdateTextureImage(&state->s_Vulkan, &atlas);
}

__declspec(dllexport) void logic_onkey() {
  // LOG_DEBUGF(
  //     "SDL_KEY{UP,DOWN} state "
  //     "code %u location %u pressed %u alt %u "
  //     "ctrl %u shift %u meta %u",
  //     game->g_Keyboard__state->code,
  //     game->g_Keyboard__state->location,
  //     game->g_Keyboard__state->pressed,
  //     game->g_Keyboard__state->altKey,
  //     game->g_Keyboard__state->ctrlKey,
  //     game->g_Keyboard__state->shiftKey,
  //     game->g_Keyboard__state->metaKey);

  if (game->mouseCaptured && 41 == game->g_Keyboard__state->code) {  // ESC
    game->Window__CaptureMouse(false);
    game->mouseCaptured = false;
    // game->s_Window.quit = true;
  }

  if (21 == game->g_Keyboard__state->code) {  // R
    LoadTextures();
  }
}

__declspec(dllexport) void logic_onfinger() {
  // LOG_DEBUGF(
  //     "SDL_FINGER state "
  //     "event %s "
  //     "clicks %u pressure %2.5f finger %u "
  //     "x %u y %u x_rel %d y_rel %d wheel_x %2.5f wheel_y %2.5f "
  //     "button_l %d button_m %d button_r %d button_x1 %d button_x2 %d ",
  //     (game->g_Finger__state->event == FINGER_UP       ? "UP"
  //      : game->g_Finger__state->event == FINGER_DOWN   ? "DOWN"
  //      : game->g_Finger__state->event == FINGER_MOVE   ? "MOVE"
  //      : game->g_Finger__state->event == FINGER_SCROLL ? "SCROLL"
  //                                                      : ""),
  //     game->g_Finger__state->clicks,
  //     game->g_Finger__state->pressure,
  //     game->g_Finger__state->finger,
  //     game->g_Finger__state->x,
  //     game->g_Finger__state->y,
  //     game->g_Finger__state->x_rel,
  //     game->g_Finger__state->y_rel,
  //     game->g_Finger__state->wheel_x,
  //     game->g_Finger__state->wheel_y,
  //     game->g_Finger__state->button_l,
  //     game->g_Finger__state->button_m,
  //     game->g_Finger__state->button_r,
  //     game->g_Finger__state->button_x1,
  //     game->g_Finger__state->button_x2);

  if (FINGER_SCROLL == game->g_Finger__state->event) {
    // TODO: how to animate camera zoom with spring damping/smoothing?
    // TODO: how to move this into physics callback? or is it better not to?
    game->world.cam[2] +=
        -game->g_Finger__state->wheel_y * game->local->PLAYER_ZOOM_SPEED /* deltaTime*/;
    game->local->isUBODirty[0] = true;
    game->local->isUBODirty[1] = true;
  }

  if (!game->mouseCaptured && FINGER_DOWN == game->g_Finger__state->event &&
      game->g_Finger__state->button_l) {
    game->Window__CaptureMouse(true);
    game->mouseCaptured = true;
  }

  if (game->mouseCaptured) {
    game->local->player.transform.rotation[0] =
        game->local->player.transform.rotation[0] +
        (game->local->PLAYER_LOOK_SPEED * game->g_Finger__state->x_rel);
    game->local->player.transform.rotation[1] =
        game->local->player.transform.rotation[1] +
        (-game->local->PLAYER_LOOK_SPEED * game->g_Finger__state->y_rel);
  }
}

// on physics
__declspec(dllexport) void logic_onfixedupdate(const f64 currentTime, const f64 deltaTime) {
  // LOG_DEBUGF("Logic dll onfixedupdate.");
  game->local->currentTime = currentTime;

  // Direction vectors for movement
  vec3 forward, right, front;

  // Convert yaw to radians for direction calculation
  float yaw_radians = glm_rad(game->local->player.transform.rotation[0]);

  // Calculate the front vector based on yaw only (for movement along the XZ plane)
  front[0] = cosf(yaw_radians);
  front[1] = 0.0f;
  front[2] = sinf(yaw_radians);
  glm_vec3_normalize(front);

  // Calculate the right vector (perpendicular to the front vector)
  glm_vec3_cross(front, (vec3){0.0f, 1.0f, 0.0f}, right);
  glm_vec3_normalize(right);

  // Move forward (W)
  if (26 == game->g_Keyboard__state->code && game->g_Keyboard__state->pressed) {  // W
    glm_vec3_scale(front, game->local->PLAYER_WALK_SPEED * deltaTime, forward);
    glm_vec3_add(
        game->local->player.transform.position,
        forward,
        game->local->player.transform.position);

    LOG_DEBUGF(
        "player pos %3.3f %3.3f %3.3f",
        game->local->player.transform.position[0],
        game->local->player.transform.position[1],
        game->local->player.transform.position[2]);
    LOG_DEBUGF(
        "player rot %3.3f %3.3f %3.3f",
        game->local->player.transform.rotation[0],
        game->local->player.transform.rotation[1],
        game->local->player.transform.rotation[2]);
  }

  // Move backward (S)
  if (22 == game->g_Keyboard__state->code && game->g_Keyboard__state->pressed) {  // S
    glm_vec3_scale(front, -game->local->PLAYER_WALK_SPEED * deltaTime, forward);
    glm_vec3_add(
        game->local->player.transform.position,
        forward,
        game->local->player.transform.position);
  }

  // Move right (D)
  if (7 == game->g_Keyboard__state->code && game->g_Keyboard__state->pressed) {  // D
    glm_vec3_scale(right, game->local->PLAYER_WALK_SPEED * deltaTime, forward);
    glm_vec3_add(
        game->local->player.transform.position,
        forward,
        game->local->player.transform.position);
  }

  // Move left (A)
  if (4 == game->g_Keyboard__state->code && game->g_Keyboard__state->pressed) {  // A
    glm_vec3_scale(right, -game->local->PLAYER_WALK_SPEED * deltaTime, forward);
    glm_vec3_add(
        game->local->player.transform.position,
        forward,
        game->local->player.transform.position);
  }

  if (20 == game->g_Keyboard__state->code && game->g_Keyboard__state->pressed) {  // Q
    game->local->player.transform.position[1] += game->local->PLAYER_WALK_SPEED * deltaTime;
  }
  if (8 == game->g_Keyboard__state->code && game->g_Keyboard__state->pressed) {  // E
    game->local->player.transform.position[1] -= game->local->PLAYER_WALK_SPEED * deltaTime;
  }
  if (224 == game->g_Keyboard__state->code && game->g_Keyboard__state->pressed) {  // Ctl
  }
  if (226 == game->g_Keyboard__state->code && game->g_Keyboard__state->pressed) {  // Alt
  }

  // state->isVBODirty = true;
  // state->isUBODirty[0] = true;
  // state->isUBODirty[1] = true;
}

static f64 accumulator2 = 0.0f;
static const f32 FPS_LOG_TIME_STEP = 1.0f;  // every second
static f64 accumulator3 = 0.0f;
static const f32 DEBUG_LOG_TIME_STEP = 1 * 60.0f;  // every 5sec
static u16 frames = 0;

s32 invert_endianness(s32 value) {
  return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000) |
         ((value << 24) & 0xFF000000);
}

// on draw
__declspec(dllexport) void logic_onupdate(const f64 currentTime, const f64 deltaTime) {
  // LOG_DEBUGF("Logic dll onupdate.");
  game->local->currentTime = currentTime;

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

  if (game->local->isVBODirty) {
    game->local->isVBODirty = false;

    game->s_Vulkan.m_instanceCount = game->instanceCount;
    game->Vulkan__UpdateVertexBuffer(&game->s_Vulkan, 1, sizeof(game->instances), game->instances);
  }

  if (game->local->isUBODirty[game->s_Vulkan.m_currentFrame]) {
    game->local->isUBODirty[game->s_Vulkan.m_currentFrame] = false;

    // 3d cam
    glm_lookat(
        game->world.cam,
        game->world.look,
        game->local->VEC3_Y_UP,  // Y-axis points upwards (GLM default)
        game->ubo1.view);

    game->s_Vulkan.m_aspectRatio = game->world.aspect;  // sync viewport

    glm_perspective(
        glm_rad(45.0f),  // half the actual 90deg fov
        game->world.aspect,
        0.1f,  // TODO: adjust clipping range for z depth?
        10.0f,
        game->ubo1.proj);

    // glm_ortho(-0.5f, +0.5f, -0.5f, +0.5f, 0.1f, 10.0f, ubo1.proj);
    glm_vec2_copy(game->world.user1, game->ubo1.user1);
    glm_vec2_copy(game->world.user2, game->ubo1.user2);

    // TODO: not sure i make use of one UBO per frame, really
    game->Vulkan__UpdateUniformBuffer(&game->s_Vulkan, game->s_Vulkan.m_currentFrame, &game->ubo1);
  }

  Bitmap3D__RenderHorizon(game);
  Bitmap3D__PostProcessing(game);

  game->Vulkan__UpdateTextureImage(&game->s_Vulkan, &game->local->screen);
}
