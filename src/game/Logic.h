#ifndef LOGIC_H
#define LOGIC_H

#include <stdbool.h>
#include <stdint.h>
typedef int8_t s8;
typedef uint16_t u16;
typedef double f64;

#include "../lib/Bitmap.h"
#include "../lib/GLMShim.h"

typedef struct String8Node String8Node;
typedef struct Arena_t Arena_t;
typedef struct Engine__State_t Engine__State_t;

enum INSTANCES {
  INSTANCE_FLOOR_0 = 0,
};

enum AUDIO_FILES {
  AUDIO_TITLE = 0,
  AUDIO_PICKUP_COIN = 1,
  AUDIO_CLICK = 2,
  AUDIO_POWERUP = 3,
};

typedef enum PlayerFacing_t {
  FRONT = 0,
  LEFT = 1,
  RIGHT = 2,
  BACK = 4,
} PlayerFacing_t;

typedef enum PlayerAnimState_t {
  IDLE = 0,
  WALK = 1,
} PlayerAnimState_t;

typedef struct Animation_t {
  f32 duration;
  u8 frameCount;
  u8 frames[10];
} Animation_t;

typedef struct AnimationState_t {
  PlayerFacing_t facing;
  PlayerAnimState_t state;
  u8 frame;
  f64 seek;
  Animation_t* anim;
} AnimationState_t;

typedef struct Camera_t {
  f32 fov;  // field of view
  // TODO: remove all but my game logic .h from the dll, which is delaying compile times
  // TODO: incl. the windows.h > minwindef.h which is overriding `near` keyword
  // f32 near;
  f32 nearZ;        // near plane
  f32 farZ;         // far plane
  mat4 projection;  // projection matrix
  mat4 view;        // view (camera) matrix
} Camera_t;

typedef struct Transform_t {
  vec3 position;  // (x, y, z)
  vec4 rotation;  // (yaw, pitch, roll)
} Transform_t;

typedef struct VirtualJoystick_t {
  f32 xAxis;
  f32 yAxis;
  f32 zAxis;
} VirtualJoystick_t;

typedef struct Menu_t {
  void (*tick)(struct Menu_t* menu, Engine__State_t* state);
  void (*render)(struct Menu_t* menu, Engine__State_t* state);
} Menu_t;

typedef struct TitleMenu_t {
  Menu_t base;
  Bitmap_t bmp;
  String8Node* options;
  u8 optionsLength;
  s8 selection;
  bool playedAudio;
} TitleMenu_t;

typedef struct AboutMenu_t {
  Menu_t base;
} AboutMenu_t;

typedef struct HelpMenu_t {
  Menu_t base;
} HelpMenu_t;

typedef struct Game_t {
  Menu_t* menu;
} Game_t;

typedef struct Entity_t {
  void (*tick)(struct Entity_t* menu, Engine__State_t* state);
  void (*render)(struct Entity_t* menu, Engine__State_t* state);
  Transform_t transform;
  void* level;
  void* sprites;
  bool flying;
  bool removed;
} Entity_t;

typedef struct Player_t {
  Entity_t base;
  Camera_t camera;
  VirtualJoystick_t input;
} Player_t;

typedef struct Sprite_t {
  void (*tick)(struct Sprite_t* menu, Engine__State_t* state);
  void (*render)(struct Sprite_t* menu, Engine__State_t* state);
  Transform_t transform;
  u32 tex;
  u32 color;
  bool removed;
} Sprite_t;

typedef struct Logic__State_t {
  f64 currentTime;

  Bitmap_t screen;
  f32* zbuf;
  f32* zbufWall;
  Bitmap_t atlas;
  Bitmap_t glyphs0;
  Bitmap_t level1;
  u8 currentLevel;
  bool levelLoaded;

  bool isVBODirty;
  bool isUBODirty[2];

  vec3 VEC3_Y_UP;

  u16 CANVAS_WH;
  u16 PIXELS_PER_UNIT;

  char* audioFiles[4];

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
  Entity_t* player;

  f32 WORLD_HEIGHT;
  f32 ATLAS_TILE_SIZE;
  f32 CANVAS_DEBUG_X;
  f32 CANVAS_DEBUG_Y;

  Game_t* game;
} Logic__State_t;

#endif  // LOGIC_H
