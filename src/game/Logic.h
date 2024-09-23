#ifndef LOGIC_H
#define LOGIC_H

#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>

#include "../lib/Arena.h"
#include "../lib/Base.h"
#include "../lib/Bitmap.h"
#include "Player.h"

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
  f32 fov;  // field of view
  // TODO: remove all but my game logic .h from the dll, which is delaying compile times
  // TODO: incl. the windows.h > minwindef.h which is overriding `near` keyword
  // f32 near;
  f32 nearZ;        // near plane
  f32 farZ;         // far plane
  mat4 projection;  // projection matrix
  mat4 view;        // view (camera) matrix
} Camera_t;

typedef struct {
  vec3 position;  // (x, y, z)
  vec4 rotation;  // (yaw, pitch, roll)
} Transform_t;

typedef struct {
  f32 xAxis;
  f32 yAxis;
  f32 zAxis;
} VirtualJoystick_t;

typedef struct {
  Transform_t transform;
  Camera_t camera;
  VirtualJoystick_t input;
} Player_t;

typedef struct {
  PlayerFacing_t facing;
  PlayerAnimState_t state;
  u8 frame;
  f64 seek;
  Animation_t* anim;
} AnimationState_t;

typedef struct {
  Arena_t* arena;
  f64 currentTime;

  Bitmap_t screen;
  f32* zbuf;
  f32* zbufWall;
  Bitmap_t brush;
  Bitmap_t atlas;

  bool isVBODirty;
  bool isUBODirty[2];

  vec3 VEC3_Y_UP;

  u16 CANVAS_WH;
  u16 PIXELS_PER_UNIT;

  char* audioFiles[1];

  Animation_t ANIM_VIKING_IDLE_FRONT;
  Animation_t ANIM_VIKING_IDLE_LEFT;
  Animation_t ANIM_VIKING_WALK_LEFT;
  Animation_t ANIM_VIKING_WALK_FRONT;
  AnimationState_t playerAnimationState;

  f32 PLAYER_WALK_SPEED;
  f32 PLAYER_FLY_SPEED;
  f32 PLAYER_ZOOM_SPEED;
  f32 PLAYER_LOOK_SPEED;

  u8 newTexId;

  Arena_t* debugArena;
  Player_t player;

  u8 WORLD_HEIGHT;
  u8 WORLD_TILE_SCALE;
  u8 ATLAS_TILE_SIZE;
} Logic__State_t;

void Player__Init(Logic__State_t* game);

#endif  // LOGIC_H
