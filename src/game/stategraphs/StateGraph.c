#include "StateGraph.h"

#include "../Logic.h"

void StateGraph__gotoState(StateGraph* sg, SGState* state) {
  sg->fsm = SGFSM_EXITING;
  if (NULL != sg->currentState->onExit) sg->currentState->onExit(sg);
  sg->currentState = state;
  state->frame = 0;
}

void StateGraph__tick(StateGraph* sg) {
  while (sg->fsm == SGFSM_EXITING || sg->fsm == SGFSM_NULL) {
    sg->fsm = SGFSM_ENTERING;
    if (NULL != sg->currentState->onEnter) sg->currentState->onEnter(sg);

    if (sg->fsm == SGFSM_UPDATING || sg->fsm == SGFSM_ENTERING) {
      sg->fsm = SGFSM_UPDATING;
      if (NULL != sg->currentState->onUpdate) sg->currentState->onUpdate(sg);
    }
  }

  for (u32 i = 0; i < sg->currentState->keyframeCount; i++) {
    SGStateKeyframe* frame = &sg->currentState->keyframes[i];
    if (sg->currentState->frame == frame->id) {
      frame->cb(sg);
      break;
    }
  }
  if (sg->currentState->frameCount > 0) {
    sg->currentState->frame = (sg->currentState->frame + 1) % sg->currentState->frameCount;
  }
}

void StateGraph__addTag(StateGraph* sg, SGStateTags1 tag) {
  sg->tags1 |= tag;
}

void StateGraph__removeTag(StateGraph* sg, SGStateTags1 tag) {
  sg->tags1 &= ~(tag);
}

bool StateGraph__hasTag(StateGraph* sg, SGStateTags1 tag) {
  return sg->tags1 & tag;  // NOTE: doesn't compare tag=0
}
