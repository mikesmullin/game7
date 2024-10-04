#ifndef LEVEL_H
#define LEVEL_H

typedef struct Arena_t Arena_t;
typedef struct Level_t Level_t;
typedef struct Engine__State_t Engine__State_t;

Level_t* Level__alloc(Arena_t* arena);
void Level__init(Arena_t* arena, Level_t* level, Engine__State_t* state);
void Level__load(Level_t* level, Engine__State_t* state, char* file);
void Level__render(Level_t* level, Engine__State_t* state);
void Level__gui(Level_t* level, Engine__State_t* state);
void Level__tick(Level_t* level, Engine__State_t* state);

#endif  // LEVEL_H