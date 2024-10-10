#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef uint32_t u32;
typedef float f32;

typedef struct Entity_t Entity_t;
typedef struct Engine__State_t Engine__State_t;

void Rigidbody2D__move(Entity_t* entity, Engine__State_t* state);
