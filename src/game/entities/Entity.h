#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef int32_t s32;
typedef float f32;
typedef double f64;

typedef struct Arena_t Arena_t;
typedef struct Entity_t Entity_t;
typedef struct Engine__State_t Engine__State_t;

Entity_t* Entity__alloc(Arena_t* arena);
void Entity__init(Entity_t* entity, Engine__State_t* state);
