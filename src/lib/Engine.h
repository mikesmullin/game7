#ifndef ENGINE_H
#define ENGINE_H

#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>
#include <windows.h>

#include "Arena.h"
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

typedef struct {
  void* localState;
  Vulkan_t s_Vulkan;
  Window_t s_Window;
  char* WINDOW_TITLE;
  char* ENGINE_NAME;
  u16 WINDOW_WIDTH;
  u16 WINDOW_HEIGHT;
  u16 CANVAS_WIDTH;
  u16 CANVAS_HEIGHT;
  u8 PHYSICS_FPS;
  u8 RENDER_FPS;
  char* shaderFiles[2];
  World_t world;
  u16 instanceCount;
  Instance_t instances[MAX_INSTANCES];
  u16 indices[6];
  Mesh_t vertices[4];
  ubo_ProjView_t ubo1;  // projection x view matrices

  int (*check_load_logic)();

  void (*Vulkan__FReadImage)(Bitmap_t* bmp, const char* filePath);
  void (*Vulkan__UpdateTextureImage)(Vulkan_t* self, const Bitmap_t* bmp);
  void (*Vulkan__UpdateVertexBuffer)(Vulkan_t* self, u8 idx, u64 size, const void* indata);
  void (*Vulkan__UpdateUniformBuffer)(Vulkan_t* self, u8 frame, void* ubo);

  void (*Audio__LoadAudioFile)(const char* path);
  void (*Audio__PlayAudio)(const int id, const bool loop, const double gain);
  void (*Audio__ResumeAudio)(const int id, const bool loop, const double gain);
  void (*Audio__StopAudio)(const int id);

  KeyboardState_t* g_Keyboard__state;
  FingerState_t* g_Finger__state;
} Engine__State_t;

typedef void (*logic_onload_t)(Arena_t*, Engine__State_t*);
typedef void (*logic_oninit_data_t)();
typedef void (*logic_oninit_compute_t)();
typedef void (*logic_onreload_t)();
typedef void (*logic_onkey_t)();
typedef void (*logic_onfinger_t)();
typedef void (*logic_onfixedupdate_t)(const f64 currentTime, const f64 deltaTime);
typedef void (*logic_onupdate_t)(const f64 currentTime, const f64 deltaTime);

int Engine__Loop();

#endif  // ENGINE_H