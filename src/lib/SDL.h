#pragma once

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <stdbool.h>
#include <stdint.h>
typedef uint32_t u32;

extern bool SDL__enableAudio;
extern bool SDL__enableGamepad;
extern bool SDL__enableVideo;

void SDL__Init();
void SDL__Shutdown();
