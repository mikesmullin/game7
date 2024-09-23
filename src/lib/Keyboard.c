#include "Keyboard.h"

KeyboardState_t g_Keyboard__state;

void Keyboard__Poll() {
  // Get the state of the keyboard
  const Uint8* keystate = SDL_GetKeyboardState(NULL);

  g_Keyboard__state.wKey = keystate[SDL_SCANCODE_W];
  g_Keyboard__state.aKey = keystate[SDL_SCANCODE_A];
  g_Keyboard__state.sKey = keystate[SDL_SCANCODE_S];
  g_Keyboard__state.dKey = keystate[SDL_SCANCODE_D];
  g_Keyboard__state.qKey = keystate[SDL_SCANCODE_Q];
  g_Keyboard__state.eKey = keystate[SDL_SCANCODE_E];
  g_Keyboard__state.rKey = keystate[SDL_SCANCODE_R];
  g_Keyboard__state.escKey = keystate[SDL_SCANCODE_ESCAPE];
}