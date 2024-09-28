#ifndef SPAWN_BLOCK_H
#define SPAWN_BLOCK_H

typedef float f32;

typedef struct Arena_t Arena_t;
typedef struct Block_t Block_t;
typedef struct Engine__State_t Engine__State_t;

Block_t* SpawnBlock__alloc(Arena_t* arena);
void SpawnBlock__init(Block_t* block, Engine__State_t* state, f32 x, f32 y);
void SpawnBlock__render(Block_t* block, Engine__State_t* state);
void SpawnBlock__tick(Block_t* block, Engine__State_t* state);

#endif  // SPAWN_BLOCK_H