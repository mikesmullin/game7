#include "Keyboard.h"

#include "Arena.h"

static KeyboardState_t lastState;

InputState_t* Keyboard__Alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(InputState_t));
}

void Keyboard__Poll(InputState_t* state) {
  // Get the state of the keyboard
  const Uint8* keystate = SDL_GetKeyboardState(NULL);

  // #metacode
  // #macro KEYBOARD_UPDATE_LATCH(T1)
  //   {{~#for T1~}}
  //   if (false == lastState.{{this.s1}} && true == keystate[{{this.e1}}]) {
  //     state->{{this.s2}} = true;
  //   }
  //   else if (false == keystate[{{this.e1}}]) {
  //     state->{{this.s2}} = false;
  //   }
  //   lastState.{{this.s1}} = keystate[{{this.e1}}];
  //   {{~/for~}}
  //
  // #table T_KEYS
  //   s1     | e1                  | s2
  //   wKey   | SDL_SCANCODE_W      | fwd
  //   sKey   | SDL_SCANCODE_S      | back
  //   aKey   | SDL_SCANCODE_A      | left
  //   dKey   | SDL_SCANCODE_D      | right
  //   spKey  | SDL_SCANCODE_SPACE  | use
  //   qKey   | SDL_SCANCODE_Q      | up
  //   eKey   | SDL_SCANCODE_E      | down
  //   rKey   | SDL_SCANCODE_R      | reload
  //   escKey | SDL_SCANCODE_ESCAPE | escape
  //
  // KEYBOARD_UPDATE_LATCH(T_KEYS)
  // #metagen
  if (false == lastState.wKey && true == keystate[SDL_SCANCODE_W]) {
    state->fwd = true;
  } else if (false == keystate[SDL_SCANCODE_W]) {
    state->fwd = false;
  }
  lastState.wKey = keystate[SDL_SCANCODE_W];
  if (false == lastState.sKey && true == keystate[SDL_SCANCODE_S]) {
    state->back = true;
  } else if (false == keystate[SDL_SCANCODE_S]) {
    state->back = false;
  }
  lastState.sKey = keystate[SDL_SCANCODE_S];
  if (false == lastState.aKey && true == keystate[SDL_SCANCODE_A]) {
    state->left = true;
  } else if (false == keystate[SDL_SCANCODE_A]) {
    state->left = false;
  }
  lastState.aKey = keystate[SDL_SCANCODE_A];
  if (false == lastState.dKey && true == keystate[SDL_SCANCODE_D]) {
    state->right = true;
  } else if (false == keystate[SDL_SCANCODE_D]) {
    state->right = false;
  }
  lastState.dKey = keystate[SDL_SCANCODE_D];
  if (false == lastState.spKey && true == keystate[SDL_SCANCODE_SPACE]) {
    state->use = true;
  } else if (false == keystate[SDL_SCANCODE_SPACE]) {
    state->use = false;
  }
  lastState.spKey = keystate[SDL_SCANCODE_SPACE];
  if (false == lastState.qKey && true == keystate[SDL_SCANCODE_Q]) {
    state->up = true;
  } else if (false == keystate[SDL_SCANCODE_Q]) {
    state->up = false;
  }
  lastState.qKey = keystate[SDL_SCANCODE_Q];
  if (false == lastState.eKey && true == keystate[SDL_SCANCODE_E]) {
    state->down = true;
  } else if (false == keystate[SDL_SCANCODE_E]) {
    state->down = false;
  }
  lastState.eKey = keystate[SDL_SCANCODE_E];
  if (false == lastState.rKey && true == keystate[SDL_SCANCODE_R]) {
    state->reload = true;
  } else if (false == keystate[SDL_SCANCODE_R]) {
    state->reload = false;
  }
  lastState.rKey = keystate[SDL_SCANCODE_R];
  if (false == lastState.escKey && true == keystate[SDL_SCANCODE_ESCAPE]) {
    state->escape = true;
  } else if (false == keystate[SDL_SCANCODE_ESCAPE]) {
    state->escape = false;
  }
  lastState.escKey = keystate[SDL_SCANCODE_ESCAPE];

  // #metaend
}
