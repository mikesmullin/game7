#include "Logic.h"

#include <stdio.h>
#include <windows.h>

__declspec(dllexport) void logic_init(State* state) {
  state->example = 0;
  printf("Logic dll loaded!\n");
}

__declspec(dllexport) void logic_draw(const State* state) {
}

__declspec(dllexport) void logic_update(State* state) {
}
