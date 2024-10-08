#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef uint16_t u16;
typedef uint32_t u32;
typedef double f64;

typedef struct SDL_Window SDL_Window;
typedef struct Vulkan_t Vulkan_t;

typedef struct DrawableArea_t {
  u32 width;
  u32 height;
} DrawableArea_t;

typedef struct Window_t {
  bool quit;
  SDL_Window* window;
  char* title;
  u16 width;
  u16 height;
  Vulkan_t* vulkan;
} Window_t;

void Window__New(Window_t* self, char* title, u16 width, u16 height, Vulkan_t* vulkan);
void Window__Begin(Window_t* self);
void Window__Bind(Window_t* self);
void Window__CaptureMouse(const bool state);
void Window__Shutdown(Window_t* self);
void Window__GetDrawableAreaExtentBounds(Window_t* self, DrawableArea_t* area);
void Window__KeepAspectRatio(Window_t* self, const u32 width, const u32 height);
void Window__RenderLoop(
    Window_t* self,
    const int physicsFps,
    const int renderFps,
    void (*physicsCallback)(const f64, const f64),
    void (*renderCallback)(const f64, const f64, const u32, const u32));
void Window__SetTitle(Window_t* window, const char* title);
