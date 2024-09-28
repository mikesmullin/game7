#include "Logic.h"

#include "../lib/Arena.h"
#include "../lib/Array.h"
#include "../lib/Bitmap.h"
#include "../lib/Engine.h"
#include "../lib/Finger.h"
#include "../lib/Keyboard.h"
#include "../lib/Log.h"
#include "../lib/Math.h"
#include "Game.h"
#include "entities/Player.h"
#include "levels/Level.h"

static Engine__State_t* game;

static f32 PixelsToUnits(u32 pixels) {
  Logic__State_t* logic = game->local;
  return (f32)pixels / logic->PIXELS_PER_UNIT;
}

static u8 Animate(AnimationState_t* state, f64 deltaTime) {
  state->seek += deltaTime;
  state->seek = Math__mod(state->seek, state->anim->duration);
  state->frame = Math__map(state->seek, 0.0f, state->anim->duration, 0, state->anim->frameCount);
  u8 texId = state->anim->frames[state->frame];
  return texId;
}

void LoadTextures() {
  Logic__State_t* logic = game->local;

  game->Vulkan__FReadImage(&logic->atlas, "../assets/textures/atlas.png");
  game->Vulkan__FReadImage(&logic->glyphs0, "../assets/textures/glyphs0.png");
}

// on process start
__declspec(dllexport) void logic_onload(Engine__State_t* _state) {
  game = _state;
  Logic__State_t* logic = game->local;
}

// on init (data only)
__declspec(dllexport) void logic_oninit_data() {
  Logic__State_t* logic = Arena__Push(game->arena, sizeof(Logic__State_t));
  game->local = logic;
  logic->player = Player__alloc(game->arena);
  Player__init(logic->player, game);

  game->WINDOW_TITLE = "Retro";
  game->ENGINE_NAME = "MS2024";
  u32 dim = 180;
  game->CANVAS_WIDTH = dim;
  game->CANVAS_HEIGHT = dim;
  game->WINDOW_WIDTH = dim * 4;
  game->WINDOW_HEIGHT = dim * 4;

  logic->WORLD_HEIGHT = 4.0f;  // world height
  logic->ATLAS_TILE_SIZE = 8.0f;

  logic->CANVAS_DEBUG_X = game->CANVAS_WIDTH / 2.0f;
  logic->CANVAS_DEBUG_Y = game->CANVAS_HEIGHT / 2.0f;

  game->PHYSICS_FPS = 50;
  game->RENDER_FPS = 60;
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
  logic->CANVAS_WH = 100;
  logic->PIXELS_PER_UNIT = logic->CANVAS_WH;
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

  logic->audioFiles[AUDIO_TITLE] = "../assets/audio/sfx/title.wav";
  logic->audioFiles[AUDIO_PICKUP_COIN] = "../assets/audio/sfx/pickupCoin.wav";
  logic->audioFiles[AUDIO_CLICK] = "../assets/audio/sfx/click.wav";
  logic->audioFiles[AUDIO_POWERUP] = "../assets/audio/sfx/powerUp.wav";

  logic->newTexId = 0;
}

__declspec(dllexport) void logic_oninit_compute() {
  Logic__State_t* logic = game->local;

  Bitmap__Alloc(game->arena, &logic->screen, game->CANVAS_WIDTH, game->CANVAS_HEIGHT, 4 /*RGBA*/);
  logic->zbuf = Arena__Push(game->arena, game->CANVAS_WIDTH * game->CANVAS_HEIGHT * sizeof(f32));
  logic->zbufWall = Arena__Push(game->arena, game->CANVAS_WIDTH * sizeof(f32));
  logic->debugArena = Arena__SubAlloc(game->arena, 1024 * 50);  // MB

  LoadTextures();
  for (u32 i = 0; i < ARRAY_COUNT(logic->audioFiles); i++) {
    game->Audio__LoadAudioFile(logic->audioFiles[i]);
  }

  // setup scene
  glms_vec3_copy((vec3){0, 0, 1.5}, game->world.cam);
  glms_vec3_copy((vec3){0, 0, 0}, game->world.look);

  glms_vec3_copy((vec3){0, 0, 0}, game->instances[INSTANCE_FLOOR_0].pos);
  glms_vec3_copy((vec3){0, 0, 0}, game->instances[INSTANCE_FLOOR_0].rot);
  glms_vec3_copy(
      (vec3){PixelsToUnits(100), PixelsToUnits(100), 1},  // ABGR
      game->instances[INSTANCE_FLOOR_0].scale);
  game->instances[INSTANCE_FLOOR_0].texId = 0;
  game->instanceCount = 1;

  logic->isUBODirty[0] = true;
  logic->isUBODirty[1] = true;

  logic->game = Game__alloc(game->arena);
  Game__init(logic->game, game);
}

__declspec(dllexport) void logic_onreload() {
  LOG_DEBUGF("Logic dll loaded.");

  if (!game->dllLoadedOnce) {
    game->dllLoadedOnce = true;
    // doesn't play audio the first time proc runs
  } else {
    game->Audio__ResumeAudio(AUDIO_PICKUP_COIN, false, 1.0f);
  }

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

__declspec(dllexport) void logic_onfinger() {
  Logic__State_t* logic = game->local;

  // LOG_DEBUGF(`
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
    game->world.cam[2] += -game->g_Finger__state->wheel_y * logic->PLAYER_ZOOM_SPEED /* deltaTime*/;
    logic->isUBODirty[0] = true;
    logic->isUBODirty[1] = true;
  }

  if (!game->mouseCaptured && FINGER_DOWN == game->g_Finger__state->event &&
      game->g_Finger__state->button_l) {
    game->Window__CaptureMouse(true);
    game->mouseCaptured = true;
  }

  if (game->mouseCaptured) {
    logic->player->transform.rotation[0] =
        logic->player->transform.rotation[0] +
        (logic->PLAYER_LOOK_SPEED * game->g_Finger__state->x_rel);
    while (logic->player->transform.rotation[0] < 0.0f) {
      logic->player->transform.rotation[0] += 360.0f;
    }
    while (logic->player->transform.rotation[0] >= 360.0f) {
      logic->player->transform.rotation[0] -= 360.0f;
    }
    logic->player->transform.rotation[1] =
        logic->player->transform.rotation[1] +
        (-logic->PLAYER_LOOK_SPEED * game->g_Finger__state->y_rel);
    while (logic->player->transform.rotation[1] < 0.0f) {
      logic->player->transform.rotation[1] += 360.0f;
    }
    while (logic->player->transform.rotation[1] >= 360.0f) {
      logic->player->transform.rotation[1] -= 360.0f;
    }
  }
}

// on physics
__declspec(dllexport) void logic_onfixedupdate(const f64 currentTime, const f64 deltaTime) {
  Logic__State_t* logic = game->local;
  Player_t* player = (Player_t*)logic->player;

  // LOG_DEBUGF("Logic dll onfixedupdate.");
  logic->currentTime = currentTime;

  // LOG_DEBUGF(
  //     "SDL_KEY{UP,DOWN} state "
  //     "w %u a %u s %u d %u q %u e %u sp %u"
  //     "r %u esc %u",
  //     game->inputState->fwd,
  //     game->inputState->left,
  //     game->inputState->back,
  //     game->inputState->right,
  //     game->inputState->use,
  //     game->inputState->up,
  //     game->inputState->down,
  //     game->inputState->reload,
  //     game->inputState->escape);

  if (game->mouseCaptured) {
    if (game->inputState->escape) {  // ESC
      game->inputState->escape = false;
      game->Window__CaptureMouse(false);
      game->mouseCaptured = false;
    }

    // TODO: do this on key up only, to avoid multiple calls at once!
    if (true == game->inputState->reload) {  // R
      game->inputState->reload = false;
      LoadTextures();
      logic->game->currentLevel->spawner->firstTick = true;  // tp to spawn
      // Player__Init(logic);
    }

    // W-S Forward/Backward axis
    if (game->inputState->fwd && game->inputState->back) {
      player->input.zAxis = 0.0f;
    } else if (game->inputState->fwd) {
      player->input.zAxis = 1.0f;
    } else if (game->inputState->back) {
      player->input.zAxis = -1.0f;
    } else {
      player->input.zAxis = 0.0f;
    }

    // A-D Left/Right axis
    if (game->inputState->left && game->inputState->right) {
      player->input.xAxis = 0.0f;
    } else if (game->inputState->left) {
      player->input.xAxis = 1.0f;
    } else if (game->inputState->right) {
      player->input.xAxis = -1.0f;
    } else {
      player->input.xAxis = 0.0f;
    }

    // Q-E Up/Down axis
    if (game->inputState->up && game->inputState->down) {
      player->input.yAxis = 0.0f;
    } else if (game->inputState->up) {
      player->input.yAxis = 1.0f;
    } else if (game->inputState->down) {
      player->input.yAxis = -1.0f;
    } else {
      player->input.yAxis = 0.0f;
    }

    // Direction vectors for movement
    vec3 forward, right, front;

    // Convert yaw to radians for direction calculation
    float yaw_radians = glms_rad(logic->player->transform.rotation[0]);

    // Calculate the front vector based on yaw only (for movement along the XZ plane)
    front[0] = Math__cos(yaw_radians);
    front[1] = 0.0f;
    front[2] = Math__sin(yaw_radians);
    glms_vec3_normalize(front);

    // Calculate the right vector (perpendicular to the front vector)
    glms_vec3_cross(front, (vec3){0.0f, 1.0f, 0.0f}, right);
    glms_vec3_normalize(right);

    // apply forward/backward motion
    if (0 != player->input.zAxis) {
      glms_vec3_scale(front, player->input.zAxis * logic->PLAYER_WALK_SPEED * deltaTime, forward);
      glms_vec3_add(logic->player->transform.position, forward, logic->player->transform.position);
    }

    // apply left/right motion
    if (0 != player->input.xAxis) {
      glms_vec3_scale(right, -player->input.xAxis * logic->PLAYER_WALK_SPEED * deltaTime, forward);
      glms_vec3_add(logic->player->transform.position, forward, logic->player->transform.position);
    }

    // apply up/down motion
    if (0 != player->input.yAxis) {
      logic->player->transform.position[1] +=
          player->input.yAxis * logic->PLAYER_FLY_SPEED * deltaTime;

      logic->player->transform.position[1] =
          MATH_CLAMP(0, logic->player->transform.position[1], 1.0f /*logic->WORLD_HEIGHT*/);
    }
  }

  // state->isVBODirty = true;
  // state->isUBODirty[0] = true;
  // state->isUBODirty[1] = true;

  Game__tick(logic->game, game);
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
  Logic__State_t* logic = game->local;
  // LOG_DEBUGF("Logic dll onupdate.");
  logic->currentTime = currentTime;

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

  if (logic->isVBODirty) {
    logic->isVBODirty = false;

    game->VulkanWrapper__SetInstanceCount(game->instanceCount);
    game->VulkanWrapper__UpdateVertexBuffer(1, sizeof(game->instances), game->instances);
  }

  if (logic->isUBODirty[game->VulkanWrapper__GetCurrentFrame()]) {
    logic->isUBODirty[game->VulkanWrapper__GetCurrentFrame()] = false;

    // 3d cam
    glms_lookat(
        game->world.cam,
        game->world.look,
        logic->VEC3_Y_UP,  // Y-axis points upwards (GLM default)
        game->ubo1.view);

    game->VulkanWrapper__SetAspectRatio(game->world.aspect);  // sync viewport

    glms_perspective(
        glms_rad(45.0f),  // half the actual 90deg fov
        game->world.aspect,
        0.1f,  // TODO: adjust clipping range for z depth?
        10.0f,
        game->ubo1.proj);

    // glms_ortho(-0.5f, +0.5f, -0.5f, +0.5f, 0.1f, 10.0f, ubo1.proj);
    glms_vec2_copy(game->world.user1, game->ubo1.user1);
    glms_vec2_copy(game->world.user2, game->ubo1.user2);

    // TODO: not sure i make use of one UBO per frame, really
    game->VulkanWrapper__UpdateUniformBuffer(game->VulkanWrapper__GetCurrentFrame(), &game->ubo1);
  }

  Game__render(logic->game, game);

  game->VulkanWrapper__UpdateTextureImage(&logic->screen);
}
