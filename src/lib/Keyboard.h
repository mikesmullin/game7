#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>

typedef struct Arena_t Arena_t;

typedef struct KeyboardState_t {
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

typedef struct InputState_t {
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