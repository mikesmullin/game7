#ifndef KEYBOARD_H
#define KEYBOARD_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "Base.h"

typedef struct {
  bool wKey;
  bool aKey;
  bool sKey;
  bool dKey;
  bool qKey;
  bool eKey;
  bool rKey;
  bool altKey;
  bool ctrlKey;
  bool shiftKey;
  bool metaKey;
  bool escKey;
} KeyboardState_t;

extern KeyboardState_t g_Keyboard__state;

void Keyboard__Poll();

#endif