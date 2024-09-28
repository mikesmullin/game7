#include "Keyboard.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "Arena.h"
// #include "Log.h"

static KeyboardState_t lastState;

KbInputState_t* Keyboard__Alloc(Arena_t* arena) {
  return Arena__Push(arena, sizeof(KbInputState_t));
}

void Keyboard__Poll(KbInputState_t* kbState) {
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
    kbState->fwd = true;
  } else if (false == keystate[SDL_SCANCODE_W]) {
    kbState->fwd = false;
  }
  lastState.wKey = keystate[SDL_SCANCODE_W];
  if (false == lastState.sKey && true == keystate[SDL_SCANCODE_S]) {
    kbState->back = true;
  } else if (false == keystate[SDL_SCANCODE_S]) {
    kbState->back = false;
  }
  lastState.sKey = keystate[SDL_SCANCODE_S];
  if (false == lastState.aKey && true == keystate[SDL_SCANCODE_A]) {
    kbState->left = true;
  } else if (false == keystate[SDL_SCANCODE_A]) {
    kbState->left = false;
  }
  lastState.aKey = keystate[SDL_SCANCODE_A];
  if (false == lastState.dKey && true == keystate[SDL_SCANCODE_D]) {
    kbState->right = true;
  } else if (false == keystate[SDL_SCANCODE_D]) {
    kbState->right = false;
  }
  lastState.dKey = keystate[SDL_SCANCODE_D];
  if (false == lastState.spKey && true == keystate[SDL_SCANCODE_SPACE]) {
    kbState->use = true;
  } else if (false == keystate[SDL_SCANCODE_SPACE]) {
    kbState->use = false;
  }
  lastState.spKey = keystate[SDL_SCANCODE_SPACE];
  if (false == lastState.qKey && true == keystate[SDL_SCANCODE_Q]) {
    kbState->up = true;
  } else if (false == keystate[SDL_SCANCODE_Q]) {
    kbState->up = false;
  }
  lastState.qKey = keystate[SDL_SCANCODE_Q];
  if (false == lastState.eKey && true == keystate[SDL_SCANCODE_E]) {
    kbState->down = true;
  } else if (false == keystate[SDL_SCANCODE_E]) {
    kbState->down = false;
  }
  lastState.eKey = keystate[SDL_SCANCODE_E];
  if (false == lastState.rKey && true == keystate[SDL_SCANCODE_R]) {
    kbState->reload = true;
  } else if (false == keystate[SDL_SCANCODE_R]) {
    kbState->reload = false;
  }
  lastState.rKey = keystate[SDL_SCANCODE_R];
  if (false == lastState.escKey && true == keystate[SDL_SCANCODE_ESCAPE]) {
    kbState->escape = true;
  } else if (false == keystate[SDL_SCANCODE_ESCAPE]) {
    kbState->escape = false;
  }
  lastState.escKey = keystate[SDL_SCANCODE_ESCAPE];
  // #metaend

  // LOG_DEBUGF(
  //     "kbState "
  //     "w %u a %u s %u d %u q %u e %u sp %u "
  //     "r %u esc %u",
  //     kbState->fwd,
  //     kbState->left,
  //     kbState->back,
  //     kbState->right,
  //     kbState->use,
  //     kbState->up,
  //     kbState->down,
  //     kbState->reload,
  //     kbState->escape);
}
