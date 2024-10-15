#pragma once

typedef float f32;
typedef struct Arena_t Arena_t;
typedef struct Block_t Block_t;
typedef struct Engine__State_t Engine__State_t;
typedef struct StateGraph_t StateGraph;
typedef struct Action_t Action;

Block_t* BreakBlock__alloc(Arena_t* arena);
void BreakBlock__init(Block_t* block, Engine__State_t* state, f32 x, f32 y);
void BreakBlock__render(Block_t* block, Engine__State_t* state);
void BreakBlock__tick(Block_t* block, Engine__State_t* state);
void BreakBlock__callSGAction(StateGraph* sg, Action* action);