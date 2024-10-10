#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef int8_t s8;
typedef uint16_t u16;
typedef double f64;

#include "../lib/Bitmap.h"
#include "../lib/GLMShim.h"

typedef struct Arena_t Arena_t;
typedef struct Engine__State_t Engine__State_t;
typedef struct List_t List_t;
typedef struct Wavefront_t Wavefront_t;
typedef enum DispatchFnId DispatchFnId;
typedef enum DispatchFn2Id DispatchFn2Id;
typedef struct QuadTreeNode_t QuadTreeNode;

typedef void (*logic_oninit_data_t)(Engine__State_t* state);
typedef void (*logic_oninit_compute_t)(Engine__State_t* state);
typedef void (*logic_onreload_t)(Engine__State_t* state);
typedef void (*logic_onfixedupdate_t)(Engine__State_t* state);
typedef void (*logic_onupdate_t)(Engine__State_t* state);

static const f32 PLAYER_WALK_SPEED = 5.0f;  // per-second
static const f32 PLAYER_STRAFE_MOD = 0.5f;  // percent of walk
static const f32 PLAYER_FLY_SPEED = 3.0f;  // per-second
static const f32 PLAYER_LOOK_SPEED = 0.3f;  // deg/sec
static const f32 PLAYER_BOB = 0.05f;
static const f32 PLAYER_HURT_ANIM_TIME = 0.33;

static const char* ASSETS_DIR;
static const char* TEXTURES_DIR;
static const char* AUDIO_DIR;
static const char* SFX_DIR;
static const char* MODELS_DIR;

enum INSTANCES {
  INSTANCE_QUAD1 = 0,
};

enum AUDIO_FILES {
  AUDIO_TITLE = 0,
  AUDIO_PICKUP_COIN = 1,
  AUDIO_CLICK = 2,
  AUDIO_POWERUP = 3,
  AUDIO_CHOP = 4,
  AUDIO_PUNCH = 5,
  AUDIO_HURT = 6,
  AUDIO_BASH = 7,
};

enum MODELS {
  MODEL_BOX = 0,  // models/box.obj
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
  f32 nearZ;  // near plane
  f32 farZ;  // far plane
  mat4 projection;  // projection matrix
  mat4 view;  // view (camera) matrix
} Camera_t;

typedef struct VirtualJoystick_t {
  f32 xAxis;
  f32 yAxis;
  f32 zAxis;
} VirtualJoystick_t;

typedef struct Menu_t {
  DispatchFnId tick;
  DispatchFnId render;
  DispatchFnId gui;
} Menu_t;

typedef struct TitleMenu_t {
  Menu_t base;
  Bitmap_t bmp;
  List_t* options;
  s8 selection;
  bool playedAudio;
  bool skip;
} TitleMenu_t;

typedef struct AboutMenu_t {
  Menu_t base;
} AboutMenu_t;

typedef struct HelpMenu_t {
  Menu_t base;
} HelpMenu_t;

typedef struct TransformComponent_t {
  v3 pos;  // (x, y, z)
  v3 rot;  // (yaw, pitch, roll)
} TransformComponent;

typedef struct EventEmitterComponent_t {
} EventEmitterComponent;

void EventEmitter__listen();
void EventEmitter__remove();
void EventEmitter__push();

typedef enum ColliderType_t {
  BOX_COLLIDER_2D,
  CIRCLE_COLLIDER_2D,
} ColliderType;

typedef struct ColliderComponent_t {
  ColliderType type;
  DispatchFnId collide;
} ColliderComponent;

typedef struct BoxCollider2DComponent_t {
  ColliderComponent base;
  //f32 x, y; // origin
  f32 hw, hh;  // half width/height (radius)
} BoxCollider2DComponent;

typedef struct CircleCollider2DComponent_t {
  ColliderComponent base;
  //f32 x, y; // origin
  f32 r;  // radius
} CircleCollider2DComponent;

typedef struct Rigidbody2DComponent_t {
  f32 xa, za;  // movement deltas (pre-collision)
} Rigidbody2DComponent;

typedef struct HealthComponent_t {
  f32 hp;
  f32 hurtTime;
} HealthComponent;

typedef enum RendererType_t {
  SPRITE_RENDERER,
  MESH_RENDERER,
} RendererType;

typedef enum TextureAsset_t {
  ATLAS_TEXTURE,
  GLYPH0_TEXTURE,
} TextureAsset;

typedef enum MeshAsset_t {
  BOX_MESH,
} MeshAsset;

typedef struct RendererComponent_t {
  RendererType type;
  TextureAsset atlas;
  u32 tx, ty;
  bool useMask;
  u32 mask, color;
  MeshAsset mesh;
} RendererComponent;

typedef struct EngineComponent_t {
  DispatchFnId tick;
  DispatchFnId render;
  DispatchFnId gui;
} EngineComponent;

typedef enum EntityTags1_t : u64 {
  TAG_NONE = 0,
  TAG_WALL = 1 << 1,  //
  TAG_CAT = 1 << 2,  //
  TAG_FLYING = 1 << 3,  //
  TAG_DEAD = 1 << 4,  //
  TAG_REMOVED = 1 << 5,  //
} EntityTags1;

typedef struct Entity_t {
  u32 id;
  u64 tags1;
  EngineComponent* engine;
  TransformComponent* tform;
  EventEmitterComponent* event;
  ColliderComponent* collider;
  Rigidbody2DComponent* rb;
  RendererComponent* render;
  HealthComponent* health;
} Entity_t;

typedef struct OnCollideClosure_t {
  Entity_t *source, *target;
  f32 x, y;
  bool before, after;
  bool noclip;
} OnCollideClosure;

typedef struct Player_t {
  Entity_t base;
  Camera_t camera;
  VirtualJoystick_t input;
  f32 bobPhase;
} Player_t;

typedef struct CatEntity_t {
  Entity_t base;
  f32 xa, ya, za;
  u32 tx;
} CatEntity_t;

typedef struct Sprite_t {
  v3 pos;  // (x, y, z)
  v3 rot;  // (yaw, pitch, roll)
  u32 tex;
  u32 color;
} Sprite_t;

typedef struct Block_t {
  Entity_t base;
  enum MODELS meshId;
  bool masked;
} Block_t;

typedef struct WallBlock_t {
  Block_t base;
} WallBlock_t;

typedef struct SpawnBlock_t {
  Block_t base;
  bool firstTick;
} SpawnBlock_t;

typedef struct CatSpawnBlock_t {
  Block_t base;
  bool firstTick;
  u32 spawnCount;
  f32 spawnInterval;
  f32 animTime;
  u32 spawnedCount;
} CatSpawnBlock_t;

typedef struct Level_t {
  Bitmap_t* bmp;
  List_t* entities;
  bool skybox;
  Bitmap_t* world;
  u32 wallTex;
  u32 ceilTex;
  u32 floorTex;
  u32 wallCol;
  u32 ceilCol;
  u32 floorCol;
  u32 width;
  u32 depth;
  u32 height;
  SpawnBlock_t* spawner;
  Arena_t* qtArena;
  QuadTreeNode* qt;
} Level_t;

typedef struct Game_t {
  Menu_t* menu;
  Level_t* curLvl;
  Entity_t* curPlyr;
  List_t* meshes;
  u32 lastUid;
} Game_t;

typedef struct Logic__State_t {
  Game_t* game;
  Bitmap_t screen;
  f32* zbuf;
  f32* zbufWall;
  Bitmap_t atlas;
  Bitmap_t glyphs0;

  bool isVBODirty;
  bool isUBODirty[2];

  u16 PIXELS_PER_UNIT;
  f32 PLAYER_ZOOM_SPEED;

  f32 CANVAS_DEBUG_X;
  f32 CANVAS_DEBUG_Y;
} Logic__State_t;