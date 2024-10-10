#pragma once

#include <stdbool.h>
typedef float f32;

typedef struct Entity_t Entity_t;
typedef struct Engine__State_t Engine__State_t;

bool Collider__check(Entity_t* entity, Engine__State_t* state, f32 x, f32 y);
bool CircleCollider2D__check(f32 x0, f32 y0, f32 r0, f32 x1, f32 y1, f32 r1);
bool BoxCollider2D__check(f32 x0, f32 y0, f32 r0, f32 x1, f32 y1, f32 r1);
