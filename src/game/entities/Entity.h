#ifndef ENTITY_H
#define ENTITY_H

typedef struct Arena_t Arena_t;
typedef struct Entity_t Entity_t;
typedef struct Engine__State_t Engine__State_t;

Entity_t* Entity__alloc(Arena_t* arena);
void Entity__init(Entity_t* entity, Engine__State_t* state);
void Entity__render(Entity_t* entity, Engine__State_t* state);
void Entity__gui(struct Entity_t* entity, Engine__State_t* state);
void Entity__tick(Entity_t* entity, Engine__State_t* state);

#endif  // ENTITY_H