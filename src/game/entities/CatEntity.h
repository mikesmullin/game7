#ifndef CAT_ENTITY_H
#define CAT_ENTITY_H

typedef struct Arena_t Arena_t;
typedef struct Entity_t Entity_t;
typedef struct Engine__State_t Engine__State_t;

Entity_t* CatEntity__alloc(Arena_t* arena);
void CatEntity__init(Entity_t* entity, Engine__State_t* state);
void CatEntity__tick(Entity_t* entity, Engine__State_t* state);
void CatEntity__render(Entity_t* entity, Engine__State_t* state);

#endif  // CAT_ENTITY_H