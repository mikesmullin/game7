#include "Engine.h"

#include "Audio.h"

void Engine__OnLoad(Engine__State_t* state) {
  state->isVBODirty = true;
  state->isUBODirty[0] = true;
  state->isUBODirty[1] = true;

  state->instanceCount = 1;

  state->VEC3_Y_UP[0] = 0;
  state->VEC3_Y_UP[1] = 1;
  state->VEC3_Y_UP[2] = 0;

  state->CANVAS_WH = 800;
  state->PIXELS_PER_UNIT = state->CANVAS_WH;

  state->Audio__LoadAudioFile = &Audio__LoadAudioFile;
  state->Audio__PlayAudio = &Audio__PlayAudio;
  state->Audio__ResumeAudio = &Audio__ResumeAudio;
}
