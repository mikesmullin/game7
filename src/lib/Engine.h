#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef f32 vec2[2];
typedef f32 vec3[3];
typedef f32 vec4[4];
typedef vec4 mat4[4];

typedef struct Arena_t Arena_t;
typedef struct Audio_t Audio_t;
typedef struct Bitmap_t Bitmap_t;
typedef struct FingerState_t FingerState_t;
typedef struct KbInputState_t KbInputState_t;
typedef struct List_t List_t;
typedef struct Logic__State_t Logic__State_t;
typedef struct PointerInputState_t PointerInputState_t;
typedef struct String_t String_t;
typedef struct Window_t Window_t;

#define MAX_INSTANCES 255  // TODO: find out how to exceed this limit

typedef struct Instance_t {
  vec3 pos;
  vec3 rot;
  vec3 scale;
  u32 texId;
} Instance_t;

typedef struct World_t {
  vec3 cam;
  vec3 look;
  vec2 user1;
  vec2 user2;
  f32 aspect;
} World_t;

typedef struct ubo_ProjView_t {
  mat4 proj;
  mat4 view;
  vec2 user1;
  vec2 user2;
} ubo_ProjView_t;

typedef struct Engine__State_t {
  Arena_t* arena;
  Logic__State_t* local;
  Window_t* window;
  Audio_t* audio;
  f64 currentTime;
  f64 deltaTime;
  u32 costPhysics;
  u32 costRender;
  String_t* WINDOW_TITLE;
  char* ENGINE_NAME;
  u16 DIMS;
  u16 WINDOW_WIDTH;
  u16 WINDOW_HEIGHT;
  u16 CANVAS_WIDTH;
  u16 CANVAS_HEIGHT;
  u8 PHYSICS_FPS;
  u8 RENDER_FPS;
  List_t* shaderFiles;
  World_t world;
  u16 instanceCount;
  Instance_t instances[MAX_INSTANCES];
  u16 indices[6];
  vec2 vertices[4];
  ubo_ProjView_t ubo1;  // projection x view matrices

  int (*check_load_logic)();

  void (*Vulkan__FReadImage)(Bitmap_t* bmp, const char* filePath);
  void (*VulkanWrapper__UpdateTextureImage)(const Bitmap_t* bmp);
  void (*VulkanWrapper__UpdateVertexBuffer)(u8 idx, u64 size, const void* indata);
  void (*VulkanWrapper__UpdateUniformBuffer)(u8 frame, void* ubo);
  void (*VulkanWrapper__SetInstanceCount)(u32 instanceCount);
  u8 (*VulkanWrapper__GetCurrentFrame)();
  void (*VulkanWrapper__SetAspectRatio)(f32 aspectRatio);

  void (*Audio__LoadAudioFile)(Arena_t* arena, Audio_t* self, const char* path);
  void (*Audio__PlayAudio)(Audio_t* self, const int id, const bool loop, const double gain);
  void (*Audio__ResumeAudio)(
      Audio_t* self, const int id, const bool loop, const double gain, double pan);
  void (*Audio__StopAudio)(Audio_t* self, const int id);

  bool mouseCaptured;
  void (*Window__CaptureMouse)(const bool state);
  void (*Window__SetTitle)(Window_t* window, const char* title);

  KbInputState_t* kbState;
  PointerInputState_t* mState;
  bool dllLoadedOnce;
} Engine__State_t;

int Engine__Loop();
