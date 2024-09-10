#ifndef ENGINE_H
#define ENGINE_H

#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>

#include "Base.h"
#include "Vulkan.h"
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
  INSTANCE_PLAYER_1 = 1,
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

  int (*pFreeClass)(int inc);
} Engine__State_t;

f32 PixelsToUnits(u32 pixels);

void Engine__OnLoad(Engine__State_t* state);

static int freestate = 0;
int FreeClass(int inc);

#endif  // ENGINE_H