#ifndef PLAYER_H
#define PLAYER_H

typedef struct Arena_t Arena_t;
typedef struct Entity_t Entity_t;
typedef struct Engine__State_t Engine__State_t;
typedef float f32;

Entity_t* Player__alloc(Arena_t* arena);
void Player__init(Entity_t* entity, Engine__State_t* state);
void Player__render(Entity_t* entity, Engine__State_t* state);
void Player__gui(struct Entity_t* entity, Engine__State_t* state);
void Player__tick(Entity_t* entity, Engine__State_t* state);

static const f32 PLAYER_WALK_SPEED = 4.0f;  // per-second
static const f32 PLAYER_STRAFE_MOD = 0.5f;  // percent of walk
static const f32 PLAYER_FLY_SPEED = 1.0f;   // per-second
static const f32 PLAYER_LOOK_SPEED = 0.1f;  // deg/sec
static const f32 PLAYER_BOB = 0.05f;

#endif  // PLAYER_H