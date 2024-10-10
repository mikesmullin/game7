#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef int32_t s32;
typedef uint8_t u8;
typedef float f32;

typedef struct Arena_t Arena_t;
typedef union SDL_Event SDL_Event;

typedef enum FingerEvent {
  FINGER_NONE = 0,
  FINGER_MOVE = 1,
  FINGER_SCROLL = 2,
  FINGER_UP = 3,
  FINGER_DOWN = 4,
} FingerEvent;

typedef struct FingerState_t {
  FingerEvent event;
  u8 clicks;
  f32 pressure;
  u8 finger;
  s32 x;
  s32 y;
  s32 x_rel;
  s32 y_rel;
  bool button_l;
  bool button_m;
  bool button_r;
  bool button_x1;
  bool button_x2;
  f32 wheel_x;
  f32 wheel_y;
} FingerState_t;

typedef struct PointerInputState_t {
  s32 x, y;
  f32 wheely;
  bool btn1;
} PointerInputState_t;

PointerInputState_t* Finger__Alloc(Arena_t* arena);
void Finger__OnEvent(const SDL_Event* event);
void Finger__Poll(PointerInputState_t* state);
