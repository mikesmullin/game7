#ifndef BLOCK_H
#define BLOCK_H

typedef float f32;
typedef struct Arena_t Arena_t;
typedef struct Block_t Block_t;
typedef struct Engine__State_t Engine__State_t;

Block_t* Block__alloc(Arena_t* arena);
void Block__init(Block_t* block, Engine__State_t* state, f32 x, f32 y);
void Block__render(Block_t* block, Engine__State_t* state);
void Block__gui(Block_t* block, Engine__State_t* state);
void Block__tick(Block_t* block, Engine__State_t* state);

#endif  // BLOCK_H