#ifndef PLAYER_H
#define PLAYER_H

typedef struct Arena_t Arena_t;
typedef struct Entity_t Entity_t;
typedef struct Engine__State_t Engine__State_t;

Entity_t* Player__alloc(Arena_t* arena);
void Player__init(Entity_t* entity, Engine__State_t* state);
void Player__tick(Entity_t* entity, Engine__State_t* state);
void Player__render(Entity_t* entity, Engine__State_t* state);

#endif  // PLAYER_H