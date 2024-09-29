#define SDL_MAIN_HANDLED

#include "Audio.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <cmixer.h>
#include <string.h>

#include "Arena.h"
#include "List.h"
#include "Log.h"

static SDL_mutex* mutex;

static void _lock_handler(cm_Event* e) {
  if (e->type == CM_EVENT_LOCK) {
    SDL_LockMutex(mutex);
  }
  if (e->type == CM_EVENT_UNLOCK) {
    SDL_UnlockMutex(mutex);
  }
}

static void _audio_callback(void* udata, Uint8* stream, int size) {
  cm_process((cm_Int16*)stream, size / 2);
}

Audio_t* Audio__Alloc(Arena_t* arena) {
  Audio_t* self = Arena__Push(arena, sizeof(Audio_t));
  self->sources = List__alloc(arena);

  SDL_AudioSpec fmt, got;
  cm_Source* src;

  mutex = SDL_CreateMutex();

  // init SDL audio
  memset(&fmt, 0, sizeof(fmt));
  fmt.freq = 44100;
  fmt.format = AUDIO_S16;
  fmt.channels = 2;
  fmt.samples = 1024;
  fmt.callback = _audio_callback;

  ASSERT_CONTEXT(
      !!(self->device = SDL_OpenAudioDevice(NULL, 0, &fmt, &got, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE)),
      "SDL Error: %s",
      SDL_GetError());

  // init library
  cm_init(got.freq);
  cm_set_lock(_lock_handler);
  cm_set_master_gain(0.5);

  // start audio
  SDL_PauseAudioDevice(self->device, 0);

  return self;
}

void Audio__LoadAudioFile(Arena_t* arena, Audio_t* self, const char* path) {
  cm_Source* src = cm_new_source_from_file(path);
  ASSERT_CONTEXT(src, "Failed to load audio file %s", cm_get_error())

  List__append(arena, self->sources, src);
  LOG_INFOF("Audio file loaded. idx: %u, path: %s", self->sources->len, path);
}

void Audio__PlayAudio(Audio_t* self, const int id, const bool loop, const double gain) {
  cm_Source* src = List__get(self->sources, id);
  if (cm_get_state(src) == CM_STATE_PLAYING) {
    cm_stop(src);
  }
  cm_set_gain(src, gain);
  cm_set_loop(src, loop ? 1 : 0);
  cm_play(src);
}

void Audio__ResumeAudio(Audio_t* self, const int id, const bool loop, const double gain) {
  cm_Source* src = List__get(self->sources, id);
  if (cm_get_state(src) == CM_STATE_PLAYING) {
    return;
  }
  cm_set_gain(src, gain);
  cm_set_loop(src, loop ? 1 : 0);
  cm_play(src);
}

void Audio__StopAudio(Audio_t* self, const int id) {
  cm_Source* src = List__get(self->sources, id);
  if (cm_get_state(src) == CM_STATE_PLAYING) {
    cm_stop(src);
  }
}

void Audio__Shutdown(Audio_t* self) {
  List__Node_t* node = self->sources->head;
  for (u32 i = 0; i < self->sources->len; i++) {
    cm_destroy_source(node->data);
    node = node->next;
  }
  SDL_CloseAudioDevice(self->device);
}