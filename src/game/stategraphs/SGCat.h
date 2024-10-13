#pragma once

#include "../../lib/Math.h"
#include "../Logic.h"
#include "../components/AudioSource.h"
#include "StateGraph.h"

static SGState SGmeow, SGtail, SGblink;
static void idleOnEnter(StateGraph* sg) {
  StateGraph__gotoState(sg, &SGmeow);
}
static SGState SGidle = {
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
static void tailKF6(StateGraph* sg) {
  StateGraph__gotoState(sg, Math__urandom2(0, 10) < 1 ? &SGmeow : &SGblink);
}
static SGState SGtail = {
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
            {31, tailKF6},
        },
};

static void blinkKF1(StateGraph* sg) {  // b eyes open
  sg->entity->render->tx = 3;
  sg->entity->render->ty = 2;
}
// static void blinkKF2(StateGraph* sg) {  // l eye closed
//   sg->entity->render->tx = 1;
//   sg->entity->render->ty = 3;
// }
// static void blinkKF3(StateGraph* sg) {  // r eye closed
//   sg->entity->render->tx = 2;
//   sg->entity->render->ty = 3;
// }
static void blinkKF4(StateGraph* sg) {  // b eyes closed
  sg->entity->render->tx = 3;
  sg->entity->render->ty = 3;
}
static void blinkKF5(StateGraph* sg) {
  StateGraph__gotoState(sg, Math__urandom2(0, 10) < 1 ? &SGmeow : &SGtail);
}

static SGState SGblink = {
    .frameCount = 12 * 4,
    .keyframeCount = 5,
    .keyframes =
        (SGStateKeyframe[]){
            {12 * 0, blinkKF1},  //
            // {12 * 1, blinkKF2},
            // {12 * 2, blinkKF3},
            {12 * 3, blinkKF4},
            {12 * 4 - 1, blinkKF5},
        },
};

static void meowKF1(StateGraph* sg) {  // eyes open, mouth closed
  sg->entity->render->tx = 3;
  sg->entity->render->ty = 2;
}
static void meowKF2(StateGraph* sg) {  // eyes open, mouth open
  sg->entity->render->tx = 0;
  sg->entity->render->ty = 4;
  AudioSource__play(sg->entity, AUDIO_MEOW);
}
static void meowKF3(StateGraph* sg) {  // eyes closed, mouth open
  sg->entity->render->tx = 3;
  sg->entity->render->ty = 4;
}
static void meowKF4(StateGraph* sg) {
  StateGraph__gotoState(sg, Math__urandom2(0, 10) < 1 ? &SGblink : &SGtail);
}

static SGState SGmeow = {
    .frameCount = 108,
    .keyframeCount = 4,
    .keyframes =
        (SGStateKeyframe[]){
            {0, meowKF1},  // eyes open, mouth closed
            {3, meowKF2},  // eyes open, mouth open (sound)
            // {9, meowKF3},  // eyes closed, mouth open
            {15, meowKF1},  // eyes open, mouth closed
            {107, meowKF4},
        },
};

// static SGState* states[] = {
//     &SGidle,  //
//     &SGtail,
// };