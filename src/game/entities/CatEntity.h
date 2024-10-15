#pragma once

#include <stdint.h>
typedef uint32_t u32;

typedef struct Arena_t Arena_t;
typedef struct Entity_t Entity_t;
typedef struct Engine__State_t Engine__State_t;
typedef struct OnCollideClosure_t OnCollideClosure;
typedef struct SGState_t SGState;
typedef struct StateGraph_t StateGraph;
typedef struct Action_t Action;

Entity_t* CatEntity__alloc(Arena_t* arena);
void CatEntity__init(Entity_t* entity, Engine__State_t* state);
void CatEntity__render(Entity_t* entity, Engine__State_t* state);
void CatEntity__gui(Entity_t* entity, Engine__State_t* state);
void CatEntity__tick(Entity_t* entity, Engine__State_t* state);
void CatEntity__collide(Entity_t* entity, Engine__State_t* state, OnCollideClosure* params);
SGState* CatEntity__getSGState(u32 id);
void CatEntity__callSGAction(StateGraph* sg, Action* action);