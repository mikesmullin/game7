#ifndef KEYBOARD_H
#define KEYBOARD_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "Arena.h"
#include "Base.h"

typedef struct {
  bool wKey;
  bool aKey;
  bool sKey;
  bool dKey;
  bool spKey;
  bool qKey;
  bool eKey;
  bool rKey;
  // bool altKey;
  // bool ctrlKey;
  // bool shiftKey;
  // bool metaKey;
  bool escKey;
} KeyboardState_t;

typedef struct {
  bool fwd;
  bool back;
  bool left;
  bool right;
  bool use;
  bool up;
  bool down;
  bool reload;
  bool escape;
} InputState_t;

InputState_t* Keyboard__Alloc(Arena_t* arena);
void Keyboard__Poll(InputState_t* state);

#endif