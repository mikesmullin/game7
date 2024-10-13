#pragma once

typedef struct Entity_t Entity_t;
typedef enum AUDIO_FILES_t AUDIO_FILES;
typedef struct AudioSourceComponent_t AudioSourceComponent;

void AudioSource__play(AudioSourceComponent* audio, AUDIO_FILES sound);