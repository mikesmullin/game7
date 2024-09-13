#ifndef ENGINE_H
#define ENGINE_H

#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>
#include <windows.h>

#include "Base.h"
#include "Bitmap.h"
#include "Finger.h"
#include "Keyboard.h"
#include "Window.h"

#define MAX_INSTANCES 255  // TODO: find out how to exceed this limit

typedef struct {
  vec2 vertex;
} Mesh_t;

typedef struct {
  vec3 pos;
  vec3 rot;
  vec3 scale;
  u32 texId;
} Instance_t;

typedef struct {
  vec3 cam;
  vec3 look;
  vec2 user1;
  vec2 user2;
  f32 aspect;
} World_t;

typedef struct {
  mat4 proj;
  mat4 view;
  vec2 user1;
  vec2 user2;
} ubo_ProjView_t;

enum INSTANCES {
  INSTANCE_FLOOR_0 = 0,
};

enum AUDIO_FILES {
  AUDIO_PICKUP_COIN = 0,
};

typedef enum {
  FRONT = 0,
  LEFT = 1,
  RIGHT = 2,
  BACK = 4,
} PlayerFacing_t;

typedef enum {
  IDLE = 0,
  WALK = 1,
} PlayerAnimState_t;

typedef struct {
  f32 duration;
  u8 frameCount;
  u8 frames[10];
} Animation_t;

typedef struct {
  PlayerFacing_t facing;
  PlayerAnimState_t state;
  u8 frame;
  f64 seek;
  Animation_t* anim;
} AnimationState_t;

typedef struct {
  World_t world;

  bool isVBODirty;
  bool isUBODirty[2];

  Vulkan_t s_Vulkan;
  Window_t s_Window;

  u16 instanceCount;
  Instance_t instances[MAX_INSTANCES];

  vec3 VEC3_Y_UP;

  ubo_ProjView_t ubo1;  // projection x view matrices

  u16 CANVAS_WH;
  u16 PIXELS_PER_UNIT;

  u16 indices[6];

  Mesh_t vertices[4];

  char* shaderFiles[2];

  char* textureFiles[1];

  char* audioFiles[1];

  Animation_t ANIM_VIKING_IDLE_FRONT;
  Animation_t ANIM_VIKING_IDLE_LEFT;
  Animation_t ANIM_VIKING_WALK_LEFT;
  Animation_t ANIM_VIKING_WALK_FRONT;
  AnimationState_t playerAnimationState;

  char* WINDOW_TITLE;
  char* ENGINE_NAME;
  u16 WINDOW_WIDTH;
  u16 WINDOW_HEIGHT;
  u8 PHYSICS_FPS;
  u8 RENDER_FPS;
  f32 PLAYER_WALK_SPEED;
  f32 PLAYER_ZOOM_SPEED;

  int (*check_load_logic)();

  void (*Vulkan__FReadImage)(Bitmap_t* bmp, const char* filePath);
  void (*Vulkan__FCloseImage)(const Bitmap_t* bmp);
  void (*Vulkan__UpdateTextureImage)(Vulkan_t* self, const Bitmap_t* bmp);
  void (*Vulkan__UpdateVertexBuffer)(Vulkan_t* self, u8 idx, u64 size, const void* indata);
  void (*Vulkan__UpdateUniformBuffer)(Vulkan_t* self, u8 frame, void* ubo);
  f32 (*Timer__NowSeconds)();
  u32 (*Timer__NowMilliseconds)();
  u32 (*Timer__Now)();
  u32 (*M__urandom)();
  u32 (*M__urandom2)(u32 a, u32 b);

  void (*Audio__LoadAudioFile)(const char* path);
  void (*Audio__PlayAudio)(const int id, const bool loop, const double gain);
  void (*Audio__ResumeAudio)(const int id, const bool loop, const double gain);
  void (*Audio__StopAudio)(const int id);

  KeyboardState_t* g_Keyboard__state;
  FingerState_t* g_Finger__state;
  u8 newTexId;
  Bitmap_t screen;
  u8 screenBuf[320 * 320 * 4];

} Engine__State_t;

typedef void (*logic_onload_t)(Engine__State_t*);
typedef void (*logic_oninit_data_t)();
typedef void (*logic_oninit_compute_t)();
typedef void (*logic_onreload_t)();
typedef void (*logic_onkey_t)();
typedef void (*logic_onfinger_t)();
typedef void (*logic_onfixedupdate_t)(const f64 deltaTime);
typedef void (*logic_onupdate_t)(const f64 deltaTime);

int Engine__Loop();

#endif  // ENGINE_H