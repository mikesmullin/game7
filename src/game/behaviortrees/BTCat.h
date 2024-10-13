#pragma once

#include "../../lib/Log.h"
#include "../Logic.h"
#include "BehaviorTree.h"

typedef struct IdleNode_t {
  BTNode base;
} IdleNode;

static BTStatus idleTick(IdleNode* node) {
  CatEntity_t* self = (CatEntity_t*)node->base.entity;
  // LOG_DEBUGF("BTCat idle");
  // node->base.entity->tags1 |= TAG_FLYING;
  return BT_RUNNING;  // Fail and switch to another state if a threat is detected
  // return BT_SUCCESS;  // We're the last node, so don't try to do anything else
}

typedef struct PanicNode_t {
  BTNode base;
} PanicNode;

static BTStatus panicTick(PanicNode* node) {
  CatEntity_t* self = (CatEntity_t*)node->base.entity;

  if (true /*cat->isThreatNear*/) {
    // LOG_DEBUGF("BTCat panic");
    return BT_RUNNING;  // Keep running in panic mode if the threat is still nearby
  }
  return BT_FAILURE;  // Do other things if the threat is gone
}

typedef struct FollowLeaderNode_t {
  BTNode base;
} FollowLeaderNode;

static BTStatus followLeaderTick(FollowLeaderNode* node) {
  if (true /*cat->isLeaderNear*/) {
    // LOG_DEBUGF("BTCat follow");
    return BT_RUNNING;  // Continue following the leader
  }
  return BT_FAILURE;  // Pick a new action, while leader is gone
}

static IdleNode BTidle = {.base.tick = (BTStatus(*)(BTNode*))(&idleTick)};
static PanicNode BTpanic = {.base.tick = (BTStatus(*)(BTNode*))(&panicTick)};
static FollowLeaderNode BTfollowLeader = {.base.tick = (BTStatus(*)(BTNode*))(&followLeaderTick)};
static SelectorNode BTroot = {
    .childCount = 3,
    .children =
        (BTNode*[]){
            (BTNode*)&BTpanic,  //
            (BTNode*)&BTfollowLeader,
            (BTNode*)&BTidle,
        },
    .base.tick = (BTStatus(*)(BTNode*))(&SelectorTick),
};
