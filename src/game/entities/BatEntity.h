#ifndef BAT_ENTITY_H
#define BAT_ENTITY_H

typedef struct Arena_t Arena_t;
typedef struct Entity_t Entity_t;
typedef struct Engine__State_t Engine__State_t;

Entity_t* BatEntity__alloc(Arena_t* arena);
void BatEntity__init(Entity_t* entity, Engine__State_t* state);
void BatEntity__tick(Entity_t* entity, Engine__State_t* state);
void BatEntity__render(Entity_t* entity, Engine__State_t* state);

#endif  // BAT_ENTITY_H