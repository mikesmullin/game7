#ifndef FINGER_H
#define FINGER_H

#include "Base.h"

typedef enum {
  FINGER_NONE = 0,
  FINGER_MOVE = 1,
  FINGER_SCROLL = 2,
  FINGER_UP = 3,
  FINGER_DOWN = 4,
} FingerEvent;

typedef struct {
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

extern FingerState_t g_Finger__state;

void Finger__OnInput(const void* _event);
void Finger__RegisterCallback(void(*cb));
void Finger__DispatchCallbacks();

#endif  // FINGER_H