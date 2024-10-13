#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef uint8_t u8;

typedef struct Arena_t Arena_t;
typedef struct List_t List_t;
typedef struct SDL_mutex SDL_mutex;

typedef struct Audio_t {
  u8 device;
  List_t* sources;
} Audio_t;

Audio_t* Audio__Alloc(Arena_t* arena);
void Audio__LoadAudioFile(Arena_t* arena, Audio_t* self, const char* path);
void Audio__PlayAudio(Audio_t* self, const int id, const bool loop, const double gain);
void Audio__ResumeAudio(
    Audio_t* self, const int id, const bool loop, const double gain, const double pan);
void Audio__StopAudio(Audio_t* self, const int id);
void Audio__Shutdown(Audio_t* self);