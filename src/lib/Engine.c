// the job of this file is to load all persistent systems
// and then forward a fn pointer interface to the ephemeral game (script) logic

#include "Engine.h"

#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>
#include <windows.h>

#include "Arena.h"
#include "Audio.h"
#include "File.h"
#include "Finger.h"
#include "Gamepad.h"
#include "HotReload.h"
#include "Keyboard.h"
#include "SDL.h"
#include "String.h"
#include "Time.h"
#include "Vulkan.h"
#include "Window.h"

static Engine__State_t* state;
static FileMonitor_t* fm;
static Arena_t arena;

static void physicsCallback(const f64 deltaTime);
static void renderCallback(const f64 deltaTime);
static void keyboardCallback();
static void fingerCallback();

static int check_load_logic() {
  char path[32] = "src/game/";
  char file[31];
  if (2 == File__CheckMonitor(fm, file)) {
    LOG_DEBUGF("saw file %s", file);
    strcat(path, file);
    LOG_DEBUGF("path %s", path);
    int r = load_logic(path);
    logic_onload(&arena, state);
    logic_onreload();
    return r;
  }
  return 0;
}

int Engine__Loop() {
  LOG_INFOF("begin engine.");

  arena = Arena__Alloc(1024 * 1024 * 50);  // MB
  state = Arena__Push(&arena, sizeof(Engine__State_t));
  fm = Arena__Push(&arena, sizeof(FileMonitor_t));
  Time__MeasureCycles();
  char* DLL_PATH = "src/game/Logic.c.dll";
  fm->directory = str8_alloc(&arena, "src/game")->str;
  fm->fileName = str8_alloc(&arena, "Logic.c.dll")->str;

  // initialize random seed using current time
  srand(Time__Now());

  state->check_load_logic = &check_load_logic;

  File__StartMonitor(fm);

  if (!load_logic(DLL_PATH)) {
    return 1;
  }
  logic_onload(&arena, state);
  logic_oninit_data();

  Vulkan__InitDriver1(&state->s_Vulkan);

  state->Vulkan__FReadImage = &Vulkan__FReadImage;
  state->Vulkan__FCloseImage = &Vulkan__FCloseImage;
  state->Vulkan__UpdateVertexBuffer = &Vulkan__UpdateVertexBuffer;
  state->Vulkan__UpdateUniformBuffer = &Vulkan__UpdateUniformBuffer;
  state->Vulkan__UpdateTextureImage = &Vulkan__UpdateTextureImage;
  state->Time__Now = &Time__Now;

  Window__New(
      &state->s_Window,
      state->WINDOW_TITLE,
      state->WINDOW_WIDTH,
      state->WINDOW_HEIGHT,
      &state->s_Vulkan);
  SDL__Init();
  Audio__Init();

  state->Audio__LoadAudioFile = &Audio__LoadAudioFile;
  state->Audio__PlayAudio = &Audio__PlayAudio;
  state->Audio__ResumeAudio = &Audio__ResumeAudio;
  state->Audio__StopAudio = &Audio__StopAudio;

  Keyboard__RegisterCallback(keyboardCallback);
  state->g_Keyboard__state = &g_Keyboard__state;
  Finger__RegisterCallback(fingerCallback);
  state->g_Finger__state = &g_Finger__state;

  Gamepad_t gamePad1;
  Gamepad__New(&gamePad1, 0);
  LOG_INFOF("Controller Id: %d, Name: %s", gamePad1.m_index, Gamepad__GetControllerName(&gamePad1));
  Gamepad__Open(&gamePad1);

  Window__Begin(&state->s_Window);

  // vulkan resource setup
  Vulkan__AssertDriverValidationLayersSupported(&state->s_Vulkan);

#if OS_MAC == 1
  ASSERT(s_Vulkan.m_requiredDriverExtensionsCount < VULKAN_REQUIRED_DRIVER_EXTENSIONS_CAP)
  // enable MoltenVK support for MacOS cross-platform support
  s_Vulkan.m_requiredDriverExtensions[s_Vulkan.m_requiredDriverExtensionsCount++] =
      VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
#endif
  Vulkan__AssertDriverExtensionsSupported(&state->s_Vulkan);

  Vulkan__CreateInstance(&state->s_Vulkan, state->WINDOW_TITLE, state->ENGINE_NAME, 1, 0, 0);
  Vulkan__InitDriver2(&state->s_Vulkan);

  Vulkan__UsePhysicalDevice(&state->s_Vulkan, 0);
  Window__Bind(&state->s_Window);

  DrawableArea_t area = {0, 0};
  Window__GetDrawableAreaExtentBounds(&state->s_Window, &area);
  state->world.aspect = ASPECT_SQUARE_1_1;
  state->s_Vulkan.m_aspectRatio = state->world.aspect;
  Window__KeepAspectRatio(&state->s_Window, area.width, area.height);

  // vulkan pipeline setup
  Vulkan__AssertSwapChainSupported(&state->s_Vulkan);
  Vulkan__CreateLogicalDeviceAndQueues(&state->s_Vulkan);
  Vulkan__CreateSwapChain(&state->s_Vulkan, false);
  Vulkan__CreateImageViews(&state->s_Vulkan);
  Vulkan__CreateRenderPass(&state->s_Vulkan);
  Vulkan__CreateDescriptorSetLayout(&state->s_Vulkan);
  Vulkan__CreateGraphicsPipeline(
      &state->s_Vulkan,
      state->shaderFiles[0],
      state->shaderFiles[1],
      sizeof(Mesh_t),
      sizeof(Instance_t),
      5,
      (u32[5]){0, 1, 1, 1, 1},
      (u32[5]){0, 1, 2, 3, 4},
      (u32[5]){
          VK_FORMAT_R32G32_SFLOAT,
          VK_FORMAT_R32G32B32_SFLOAT,
          VK_FORMAT_R32G32B32_SFLOAT,
          VK_FORMAT_R32G32B32_SFLOAT,
          VK_FORMAT_R32_UINT},
      (u32[5]){
          offsetof(Mesh_t, vertex),
          offsetof(Instance_t, pos),
          offsetof(Instance_t, rot),
          offsetof(Instance_t, scale),
          offsetof(Instance_t, texId)});
  Vulkan__CreateFrameBuffers(&state->s_Vulkan);
  Vulkan__CreateCommandPool(&state->s_Vulkan);
  Bitmap_t bmp;
  Vulkan__FReadImage(&bmp, state->textureFiles[0]);
  Vulkan__CreateTextureImage(&state->s_Vulkan, &bmp);
  Vulkan__FCloseImage(&bmp);
  Vulkan__CreateTextureImageView(&state->s_Vulkan);
  Vulkan__CreateTextureSampler(&state->s_Vulkan);
  Vulkan__CreateVertexBuffer(&state->s_Vulkan, 0, sizeof(state->vertices), state->vertices);
  Vulkan__CreateVertexBuffer(&state->s_Vulkan, 1, sizeof(state->instances), state->instances);
  Vulkan__CreateIndexBuffer(&state->s_Vulkan, sizeof(state->indices), state->indices);
  Vulkan__CreateUniformBuffers(&state->s_Vulkan, sizeof(state->ubo1));
  Vulkan__CreateDescriptorPool(&state->s_Vulkan);
  Vulkan__CreateDescriptorSets(&state->s_Vulkan);
  Vulkan__CreateCommandBuffers(&state->s_Vulkan);
  Vulkan__CreateSyncObjects(&state->s_Vulkan);
  state->s_Vulkan.m_drawIndexCount = ARRAY_COUNT(state->indices);

  // setup scene
  logic_oninit_compute();
  logic_onreload();

  // main loop
  Window__RenderLoop(
      &state->s_Window,
      state->PHYSICS_FPS,
      state->RENDER_FPS,
      &physicsCallback,
      &renderCallback);

  // cleanup
  LOG_INFOF("shutdown engine.");
  File__EndMonitor(fm);
  unload_logic();
  Vulkan__DeviceWaitIdle(&state->s_Vulkan);
  Gamepad__Shutdown(&gamePad1);
  Vulkan__Cleanup(&state->s_Vulkan);
  Audio__Shutdown();
  Window__Shutdown(&state->s_Window);
  Arena__Free(&arena);
  LOG_INFOF("end engine.");
  return 0;
}

static void keyboardCallback() {
  logic_onkey();
}

static void fingerCallback() {
  logic_onfinger();
}

static f64 accumulator1 = 0.0f;
static const f32 FILE_CHECK_MONITOR_TIME_STEP = 1.0f / 4;  // 4 checks per second

static void physicsCallback(const f64 deltaTime) {
  accumulator1 += deltaTime;
  if (accumulator1 >= FILE_CHECK_MONITOR_TIME_STEP) {
    check_load_logic();

    while (accumulator1 >= FILE_CHECK_MONITOR_TIME_STEP) {
      accumulator1 -= FILE_CHECK_MONITOR_TIME_STEP;
    }
  }

  // logic_onfixedupdate(deltaTime);
}

static void renderCallback(const f64 deltaTime) {
  logic_onupdate(deltaTime);
}