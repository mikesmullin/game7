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
void Entity__render(Entity_t* entity, Engine__State_t* state);
void Entity__gui(Entity_t* entity, Engine__State_t* state);
void Entity__tick(Entity_t* entity, Engine__State_t* state);
void Entity__move(Entity_t* entity, Engine__State_t* state);
bool Entity__collide(Entity_t* self, Engine__State_t* state, Entity_t* entity, f64 x, f64 y);
