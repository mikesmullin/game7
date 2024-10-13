#include "AudioSource.h"

#include "../../lib/Engine.h"
#include "../Logic.h"

extern Engine__State_t* g_engine;

void AudioSource__play(AudioSourceComponent* audio, AUDIO_FILES sound) {
  // TODO: calc distance/dropoff between source and listener, set volume/pan
  // TODO: only play if listener is near

  g_engine->Audio__ResumeAudio(g_engine->audio, sound, false, 1.0f);
}