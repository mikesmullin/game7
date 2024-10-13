#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef uint64_t u64;

typedef struct StateGraph_t StateGraph;
typedef struct SGState_t SGState;
typedef enum SGStateTags1_t : u64 SGStateTags1;

void StateGraph__gotoState(StateGraph* sg, SGState* state);
void StateGraph__tick(StateGraph* sg);
void StateGraph__addTag(StateGraph* sg, SGStateTags1 tag);
void StateGraph__removeTag(StateGraph* sg, SGStateTags1 tag);
bool StateGraph__hasTag(StateGraph* sg, SGStateTags1 tag);