#pragma once

#include <stdbool.h>
typedef float f32;
typedef double f64;

typedef struct Arena_t Arena_t;
typedef struct Block_t Block_t;
typedef struct Entity_t Entity_t;
typedef struct Engine__State_t Engine__State_t;

Block_t* Block__alloc(Arena_t* arena);
void Block__init(Block_t* block, Engine__State_t* state, f32 x, f32 y);
void Block__render(Block_t* block, Engine__State_t* state);
void Block__gui(Block_t* block, Engine__State_t* state);
void Block__tick(Block_t* block, Engine__State_t* state);
bool Block__collide(Block_t* block, Engine__State_t* state, Entity_t* entity, f64 x, f64 y);
