#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef int8_t s8;
typedef uint16_t u16;
typedef double f64;

#include "../lib/Bitmap.h"
#include "utils/GLMShim.h"

typedef struct Arena_t Arena_t;
typedef struct Engine__State_t Engine__State_t;
typedef struct List_t List_t;
typedef struct Wavefront_t Wavefront_t;
typedef enum DispatchFnId DispatchFnId;
typedef enum DispatchFn2Id DispatchFn2Id;
typedef struct QuadTreeNode_t QuadTreeNode;
typedef struct BTNode_t BTNode;

typedef void (*logic_oninit_data_t)(Engine__State_t* state);
typedef void (*logic_oninit_compute_t)(Engine__State_t* state);
typedef void (*logic_onreload_t)(Engine__State_t* state);
typedef void (*logic_onfixedupdate_t)(Engine__State_t* state);
typedef void (*logic_onupdate_t)(Engine__State_t* state);

enum INSTANCES {
  INSTANCE_QUAD1 = 0,
};

typedef enum AUDIO_FILES_t {
  AUDIO_TITLE = 0,
  AUDIO_PICKUP_COIN = 1,
  AUDIO_CLICK = 2,
  AUDIO_POWERUP = 3,
  AUDIO_CHOP = 4,
  AUDIO_PUNCH = 5,
  AUDIO_HURT = 6,
  AUDIO_BASH = 7,
  AUDIO_MEOW = 8,
} AUDIO_FILES;

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

typedef struct AudioListenerComponent_t {
} AudioListenerComponent;

typedef struct AudioSourceComponent_t {
} AudioSourceComponent;

#define MAX_LISTENERS 10

typedef enum EventType_t {
  EVENT_NONE,  //
  EVENT_HELLO,
  EVENT_GOODBYE,
  EVENT_DEATH,
  EVENT_ATTACKED,
  EVENT_ANIMOVER,
} EventType;

typedef enum ListenerFnId_t {
  BLOCK__HELLO,  //
  ENTITY__HELLO,
  CAT_ENTITY__GOODBYE,
  LISTENER_FN_COUNT
} ListenerFnId;

typedef struct EventEmitter_t {
  EventType event[MAX_LISTENERS];
  ListenerFnId listeners[MAX_LISTENERS];
  int count;
} EventEmitter;

typedef enum EntityTags1_t : u64 {
  TAG_NONE = 0,
  TAG_WALL = 1 << 1,  //
  TAG_CAT = 1 << 2,  //
  TAG_BRICK = 1 << 3,  //
  TAG_BROKEN = 1 << 4,  //
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
  // TODO: if we don't need to iterate, these can be moved within subclass
  AudioSourceComponent* audio;
  AudioListenerComponent* hear;
  EventEmitter* events;
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

typedef enum ActionType_t {
  ACTION_NONE,  //
  ACTION_USE,
} ActionType;

typedef struct Action_t {
  ActionType type;
  Entity_t* actor;
  Entity_t* target;
} Action;

typedef enum SGStateTags1_t : u64 {
  SGST_NONE = 0,  //
  SGST_BUSY = 1 << 1,
} SGStateTags1;

typedef struct StateGraph_t StateGraph;
typedef struct SGState_t SGState;
typedef void (*SGStateFn)(StateGraph* sg);
typedef void (*SGActionFn)(StateGraph* sg, Action* action);

typedef struct SGStateKeyframe_t {
  u32 id;
  SGStateFn cb;
} SGStateKeyframe;

typedef struct SGState_t {
  SGStateFn onEnter;
  SGStateFn onUpdate;
  SGStateFn onExit;
  u32 frameCount;
  u32 keyframeCount;
  SGStateKeyframe* keyframes;
  // eventListeners[];
} SGState;

typedef enum SGFSM_t {
  SGFSM_NULL,  //
  SGFSM_ENTERING,
  SGFSM_UPDATING,
  SGFSM_EXITING,
} SGFSM;

typedef struct StateGraph_t {
  Entity_t* entity;
  u32 currentState;
  SGFSM fsm;
  u32 frame;
  // SGActionFn actions;
  EventEmitter events;
  SGStateTags1 tags1;
} StateGraph;

typedef struct CatEntity_t {
  Entity_t base;
  f32 xa, ya, za;
  u32 tx;
  StateGraph* sg;
  BTNode* brain;
} CatEntity_t;

typedef struct Sprite_t {
  v3 pos;  // (x, y, z)
  v3 rot;  // (yaw, pitch, roll)
  u32 tx, ty;
  bool useMask;
  u32 mask, color;
} Sprite_t;

typedef struct RubbleSprite_t {
  Sprite_t base;
  f32 xa, ya, za;
  bool removed;
} RubbleSprite;

typedef struct Block_t {
  Entity_t base;
  enum MODELS meshId;
  bool masked;
} Block_t;

typedef struct WallBlock_t {
  Block_t base;
} WallBlock_t;

typedef struct BreakBlock_t {
  Block_t base;
  RubbleSprite* sprites[32];
  StateGraph* sg;
} BreakBlock_t;

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
  u32 maxSpawnCount;
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

  const char* ASSETS_DIR;
  const char* TEXTURES_DIR;
  const char* AUDIO_DIR;
  const char* SFX_DIR;
  const char* MODELS_DIR;
} Logic__State_t;