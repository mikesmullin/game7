#pragma once

typedef float f32;
typedef struct Arena_t Arena_t;
typedef struct Block_t Block_t;
typedef struct Engine__State_t Engine__State_t;

Block_t* WallBlock__alloc(Arena_t* arena);
void WallBlock__init(Block_t* block, Engine__State_t* state, f32 x, f32 y);
void WallBlock__render(Block_t* block, Engine__State_t* state);