#include "Engine.h"

#include <windows.h>

#include "Arena.h"
#include "Audio.h"
#include "Bitmap.h"
#include "File.h"
#include "Finger.h"
#include "Gamepad.h"
#include "HotReload.h"
#include "Keyboard.h"
#include "List.h"
#include "Log.h"
#include "SDL.h"
#include "String.h"
#include "Time.h"
#include "Vulkan.h"
#include "VulkanWrapper.h"
#include "Window.h"

static Arena_t arena;
static Engine__State_t* state;
static FileMonitor_t* fm;
static Vulkan_t vulkan;
static Window_t window;

static void physicsCallback(const f64 currentTime, const f64 deltaTime);
static void renderCallback(const f64, const f64, const u32, const u32);

static int check_load_logic() {
  char path[32] = "src/game/";
  char file[31];
  if (2 == File__CheckMonitor(fm, file)) {
    LOG_DEBUGF("saw file %s", file);
    strcat_s(path, 32, file);
    LOG_DEBUGF("path %s", path);
    int r = load_logic(path);
    if (r) {
      logic_onreload(state);
    }
    return r;
  }
  return 0;
}

int Engine__Loop() {
  LOG_INFOF("begin engine.");

  // SLEEP(1000 * 5);  // wait for debugger attach

  Arena__Alloc(&arena, 1024 * 1024 * 50);  // MB
  state = Arena__Push(&arena, sizeof(Engine__State_t));
  state->arena = &arena;
  fm = Arena__Push(&arena, sizeof(FileMonitor_t));
  Time__MeasureCycles();
  char* DLL_PATH = "src/game/Logic.c.dll";
  fm->directory = String__alloc(&arena, "src/game")->str;
  fm->fileName = String__alloc(&arena, "Logic.c.dll")->str;

  // initialize random seed using current time
  srand(Time__Now());

  state->check_load_logic = &check_load_logic;
  state->Vulkan__FReadImage = &Vulkan__FReadImage;
  state->VulkanWrapper__UpdateTextureImage = &VulkanWrapper__UpdateTextureImage;
  state->VulkanWrapper__UpdateVertexBuffer = &VulkanWrapper__UpdateVertexBuffer;
  state->VulkanWrapper__UpdateUniformBuffer = &VulkanWrapper__UpdateUniformBuffer;
  state->VulkanWrapper__SetInstanceCount = &VulkanWrapper__SetInstanceCount;
  state->VulkanWrapper__GetCurrentFrame = &VulkanWrapper__GetCurrentFrame;
  state->VulkanWrapper__SetAspectRatio = &VulkanWrapper__SetAspectRatio;

  SDL__Init();
  state->audio = Audio__Alloc(state->arena);
  state->Audio__LoadAudioFile = &Audio__LoadAudioFile;
  state->Audio__PlayAudio = &Audio__PlayAudio;
  state->Audio__ResumeAudio = &Audio__ResumeAudio;
  state->Audio__StopAudio = &Audio__StopAudio;

  File__StartMonitor(fm);
  if (!load_logic(DLL_PATH)) {
    return 1;
  }
  logic_oninit_data(state);

  VulkanWrapper__Init(&vulkan);
  Vulkan__InitDriver1(&vulkan);

  Window__New(
      &window,
      state->WINDOW_TITLE->str,
      state->WINDOW_WIDTH,
      state->WINDOW_HEIGHT,
      &vulkan);
  state->window = &window;
  state->Window__CaptureMouse = &Window__CaptureMouse;
  state->Window__SetTitle = &Window__SetTitle;
  state->kbState = Keyboard__Alloc(state->arena);
  state->mState = Finger__Alloc(state->arena);

  Gamepad_t gamePad1;
  Gamepad__New(&gamePad1, 0);
  LOG_INFOF("Controller Id: %d, Name: %s", gamePad1.m_index, Gamepad__GetControllerName(&gamePad1));
  Gamepad__Open(&gamePad1);

  Window__Begin(&window);

  // vulkan resource setup
  Vulkan__AssertDriverValidationLayersSupported(&vulkan);

#if OS_MAC == 1
  ASSERT(s_Vulkan.m_requiredDriverExtensionsCount < VULKAN_REQUIRED_DRIVER_EXTENSIONS_CAP)
  // enable MoltenVK support for MacOS cross-platform support
  s_Vulkan.m_requiredDriverExtensions[s_Vulkan.m_requiredDriverExtensionsCount++] =
      VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
#endif
  Vulkan__AssertDriverExtensionsSupported(&vulkan);

  Vulkan__CreateInstance(&vulkan, state->WINDOW_TITLE->str, state->ENGINE_NAME, 1, 0, 0);
  Vulkan__InitDriver2(&vulkan);

  Vulkan__UsePhysicalDevice(&vulkan, 0);
  Window__Bind(&window);

  DrawableArea_t area = {0, 0};
  Window__GetDrawableAreaExtentBounds(&window, &area);
  state->world.aspect = ASPECT_SQUARE_1_1;
  vulkan.m_aspectRatio = state->world.aspect;
  Window__KeepAspectRatio(&window, area.width, area.height);

  // vulkan pipeline setup
  Vulkan__AssertSwapChainSupported(&vulkan);
  Vulkan__CreateLogicalDeviceAndQueues(&vulkan);
  Vulkan__CreateSwapChain(&vulkan, false);
  Vulkan__CreateImageViews(&vulkan);
  Vulkan__CreateRenderPass(&vulkan);
  Vulkan__CreateDescriptorSetLayout(&vulkan);
  Vulkan__CreateGraphicsPipeline(
      &vulkan,
      List__get(state->shaderFiles, 0),
      List__get(state->shaderFiles, 1),
      sizeof(vec2),
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
          0,
          offsetof(Instance_t, pos),
          offsetof(Instance_t, rot),
          offsetof(Instance_t, scale),
          offsetof(Instance_t, texId)});
  Vulkan__CreateFrameBuffers(&vulkan);
  Vulkan__CreateCommandPool(&vulkan);
  // create temporary base texture matching canvas dimensions
  Bitmap_t bmp;
  Bitmap__Init(&bmp, state->CANVAS_WIDTH, state->CANVAS_WIDTH, 4);
  u8 bmpbuf[bmp.len];
  // fill with black
  memset(bmpbuf, 0, bmp.len);
  bmp.buf = bmpbuf;
  Vulkan__CreateTextureImage(&vulkan, &bmp);
  Vulkan__CreateTextureImageView(&vulkan);
  Vulkan__CreateTextureSampler(&vulkan);
  Vulkan__CreateVertexBuffer(&vulkan, 0, sizeof(state->vertices), state->vertices);
  u8 indicesCount = 6;
  Vulkan__CreateVertexBuffer(&vulkan, 1, sizeof(state->instances), state->instances);
  Vulkan__CreateIndexBuffer(&vulkan, sizeof(u16) * indicesCount, state->indices);
  Vulkan__CreateUniformBuffers(&vulkan, sizeof(state->ubo1));
  Vulkan__CreateDescriptorPool(&vulkan);
  Vulkan__CreateDescriptorSets(&vulkan);
  Vulkan__CreateCommandBuffers(&vulkan);
  Vulkan__CreateSyncObjects(&vulkan);
  vulkan.m_drawIndexCount = indicesCount;

  // setup scene

  logic_oninit_compute(state);
  logic_onreload(state);

  // main loop
  Window__RenderLoop(
      &window,
      state->PHYSICS_FPS,
      state->RENDER_FPS,
      &physicsCallback,
      &renderCallback);

  // cleanup
  LOG_INFOF("shutdown engine.");
  File__EndMonitor(fm);
  unload_logic();
  Vulkan__DeviceWaitIdle(&vulkan);
  Gamepad__Shutdown(&gamePad1);
  Vulkan__Cleanup(&vulkan);
  Audio__Shutdown(state->audio);
  Window__Shutdown(&window);
  Arena__Free(&arena);
  LOG_INFOF("end engine.");
  return 0;
}

static f64 accumulator1 = 0.0f;
static const f32 FILE_CHECK_MONITOR_TIME_STEP = 1.0f / 4;  // 4 checks per second

static void physicsCallback(const f64 currentTime, const f64 deltaTime) {
  Keyboard__Poll(state->kbState);

  accumulator1 += deltaTime;
  if (accumulator1 >= FILE_CHECK_MONITOR_TIME_STEP) {
    check_load_logic();

    while (accumulator1 >= FILE_CHECK_MONITOR_TIME_STEP) {
      accumulator1 -= FILE_CHECK_MONITOR_TIME_STEP;
    }
  }

  state->currentTime = currentTime;
  state->deltaTime = deltaTime;
  logic_onfixedupdate(state);
}

static void renderCallback(
    const f64 currentTime, const f64 deltaTime, const u32 costPhysics, const u32 costRender) {
  state->currentTime = currentTime;
  state->deltaTime = deltaTime;
  state->costPhysics = costPhysics;
  state->costRender = costRender;
  logic_onupdate(state);
}