#pragma once

typedef struct Arena_t Arena_t;
typedef struct Entity_t Entity_t;
typedef struct Engine__State_t Engine__State_t;
typedef float f32;

static const f32 PLAYER_BOB = 0.05f;
static const f32 PLAYER_HURT_ANIM_TIME = 0.33;

Entity_t* Player__alloc(Arena_t* arena);
void Player__init(Entity_t* entity, Engine__State_t* state);
void Player__render(struct Entity_t* entity, Engine__State_t* state);
void Player__tick(Entity_t* entity, Engine__State_t* state);