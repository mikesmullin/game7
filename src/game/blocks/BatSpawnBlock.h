#ifndef BAT_SPAWN_BLOCK_H
#define BAT_SPAWN_BLOCK_H

typedef float f32;

typedef struct Arena_t Arena_t;
typedef struct Block_t Block_t;
typedef struct Engine__State_t Engine__State_t;

Block_t* BatSpawnBlock__alloc(Arena_t* arena);
void BatSpawnBlock__init(Block_t* block, Engine__State_t* state, f32 x, f32 y);
void BatSpawnBlock__render(Block_t* block, Engine__State_t* state);
void BatSpawnBlock__tick(Block_t* block, Engine__State_t* state);

#endif  // BAT_SPAWN_BLOCK_H