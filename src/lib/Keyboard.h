#pragma once

#include <stdbool.h>

typedef struct Arena_t Arena_t;

typedef struct KeyboardState_t {
  bool wKey;
  bool aKey;
  bool sKey;
  bool dKey;
  bool qKey;
  bool eKey;
  bool rKey;
  bool spKey;
  bool ctlKey;
  // bool altKey;
  // bool shiftKey;
  // bool metaKey;
  bool escKey;
} KeyboardState_t;

typedef struct KbInputState_t {
  bool fwd;
  bool back;
  bool left;
  bool right;
  bool use;
  bool up;
  bool down;
  bool reload;
  bool escape;
} KbInputState_t;

KbInputState_t* Keyboard__Alloc(Arena_t* arena);
void Keyboard__Poll(KbInputState_t* kbState);
