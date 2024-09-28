#include "Finger.h"

#include "Arena.h"
// #include "Log.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

static FingerState_t lastState;
static PointerInputState_t* mState;

#define FINGER_CALLBACKS_CAP 10
static const void (*CALLBACKS[FINGER_CALLBACKS_CAP])();
static u8 callbackCount = 0;

PointerInputState_t* Finger__Alloc(Arena_t* arena) {
  mState = Arena__Push(arena, sizeof(PointerInputState_t));
  return mState;
}

void Finger__ProcessLastEvent();  // fwd decl

void Finger__OnEvent(const SDL_Event* event) {
  lastState.event = FINGER_NONE;
  lastState.clicks = 0;
  lastState.pressure = 0.0f;
  lastState.finger = 0;
  lastState.x = 0;
  lastState.y = 0;
  lastState.x_rel = 0;
  lastState.y_rel = 0;
  lastState.button_l = false;
  lastState.button_m = false;
  lastState.button_r = false;
  lastState.button_x1 = false;
  lastState.button_x2 = false;
  lastState.wheel_x = 0.0f;
  lastState.wheel_y = 0.0f;

  switch (event->type) {
    case SDL_MOUSEMOTION:
      lastState.event = FINGER_MOVE;
      lastState.x = event->motion.x;
      lastState.y = event->motion.y;
      lastState.x_rel = event->motion.xrel;
      lastState.y_rel = event->motion.yrel;

      lastState.button_l = event->motion.state & SDL_BUTTON_LMASK;
      lastState.button_m = event->motion.state & SDL_BUTTON_MMASK;
      lastState.button_r = event->motion.state & SDL_BUTTON_RMASK;
      lastState.button_x1 = event->motion.state & SDL_BUTTON_X1MASK;
      lastState.button_x2 = event->motion.state & SDL_BUTTON_X2MASK;
      Finger__ProcessLastEvent();
      break;

    case SDL_FINGERMOTION:
      lastState.event = FINGER_MOVE;
      lastState.x = event->tfinger.x;
      lastState.y = event->tfinger.y;
      lastState.x_rel = event->tfinger.dx;
      lastState.y_rel = event->tfinger.dy;
      lastState.finger = event->tfinger.fingerId;
      Finger__ProcessLastEvent();
      break;

    case SDL_MOUSEWHEEL:
      lastState.event = FINGER_SCROLL;
      // lastState.wheel_x = event->wheel.x;
      // lastState.wheel_y = event->wheel.y;
      lastState.wheel_x = event->wheel.preciseX;
      lastState.wheel_y = event->wheel.preciseY;

      Finger__ProcessLastEvent();
      break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      lastState.event = SDL_MOUSEBUTTONDOWN == event->type ? FINGER_DOWN : FINGER_UP;
      lastState.x = event->button.x;
      lastState.y = event->button.y;

      lastState.clicks = event->button.clicks;
      lastState.button_l = event->button.button == SDL_BUTTON_LEFT;
      lastState.button_m = event->button.button == SDL_BUTTON_MIDDLE;
      lastState.button_r = event->button.button == SDL_BUTTON_RIGHT;
      lastState.button_x1 = event->button.button == SDL_BUTTON_X1;
      lastState.button_x2 = event->button.button == SDL_BUTTON_X2;

      Finger__ProcessLastEvent();
      break;

    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
      lastState.event = SDL_FINGERDOWN == event->type ? FINGER_DOWN : FINGER_UP;
      lastState.x = event->tfinger.x;
      lastState.y = event->tfinger.y;
      lastState.pressure = event->tfinger.pressure;
      lastState.finger = event->tfinger.fingerId;
      lastState.button_l = SDL_FINGERDOWN == event->type;

      Finger__ProcessLastEvent();
      break;
  }
}

void Finger__ProcessLastEvent() {
  // LOG_DEBUGF(
  //     "SDL_FINGER state "
  //     "event %s "
  //     "clicks %u pressure %2.5f finger %u "
  //     "x %u y %u x_rel %d y_rel %d wheel_x %2.5f wheel_y %2.5f "
  //     "button_l %d button_m %d button_r %d button_x1 %d button_x2 %d ",
  //     (lastState.event == FINGER_UP       ? "UP"
  //      : lastState.event == FINGER_DOWN   ? "DOWN"
  //      : lastState.event == FINGER_MOVE   ? "MOVE"
  //      : lastState.event == FINGER_SCROLL ? "SCROLL"
  //                                         : ""),
  //     lastState.clicks,
  //     lastState.pressure,
  //     lastState.finger,
  //     lastState.x,
  //     lastState.y,
  //     lastState.x_rel,
  //     lastState.y_rel,
  //     lastState.wheel_x,
  //     lastState.wheel_y,
  //     lastState.button_l,
  //     lastState.button_m,
  //     lastState.button_r,
  //     lastState.button_x1,
  //     lastState.button_x2);

  if (FINGER_SCROLL == lastState.event) {
    mState->wheely += lastState.wheel_y;
  }

  if (FINGER_MOVE == lastState.event) {
    mState->x += lastState.x_rel;
    mState->y += lastState.y_rel;
  }

  if (FINGER_DOWN == lastState.event && lastState.button_l) {
    mState->btn1 = true;
  }
  if (FINGER_UP == lastState.event && lastState.button_l) {
    mState->btn1 = false;
  }

  // LOG_DEBUGF(
  //     "mState "
  //     "x %d y %d wheely %3.3f btn1 %u",
  //     mState->x,
  //     mState->y,
  //     mState->wheely,
  //     mState->btn1);
}