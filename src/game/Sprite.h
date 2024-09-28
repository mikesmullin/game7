#ifndef SPRITE_H
#define SPRITE_H

typedef struct Arena_t Arena_t;
typedef struct Sprite_t Sprite_t;
typedef struct Engine__State_t Engine__State_t;

Sprite_t* Sprite__alloc(Arena_t* arena);
void Sprite__init(Sprite_t* sprite, Engine__State_t* state);
void Sprite__tick(Sprite_t* sprite, Engine__State_t* state);
void Sprite__render(Sprite_t* sprite, Engine__State_t* state);

#endif  // SPRITE_H