#pragma once

#include "../Logic.h"
#include "StateGraph.h"

static SGState idle, tail;
static void idleOnEnter(StateGraph* sg) {
  StateGraph__gotoState(sg, &tail);
}
static SGState idle = {
    .onEnter = idleOnEnter,
    .frame = 0,
};

static void tailOnEnter(StateGraph* sg) {
  // StateGraph__addTag(sg, SGST_BUSY);
  sg->entity->render->ty = 2;
}
static void tailKF1(StateGraph* sg) {
  CatEntity_t* self = (CatEntity_t*)sg->entity;
  sg->entity->render->tx = 2;
  // sg->entity->audio->play("meow");
}
static void tailKF2(StateGraph* sg) {
  sg->entity->render->tx = 3;
}
static void tailKF3(StateGraph* sg) {
  sg->entity->render->tx = 4;
}
static void tailKF4(StateGraph* sg) {
  sg->entity->render->tx = 5;
}
static void tailKF5(StateGraph* sg) {
  sg->entity->render->tx = 6;
}
static SGState tail = {
    .onEnter = tailOnEnter,
    .frameCount = 32,
    .keyframeCount = 6,
    .keyframes =
        (SGStateKeyframe[]){
            {0, tailKF1},  //
            {6, tailKF2},
            {12, tailKF3},
            {18, tailKF4},
            {24, tailKF5},
        },
};
// static SGState* states[] = {
//     &idle,  //
//     &tail,
// };