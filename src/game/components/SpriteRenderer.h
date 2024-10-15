#pragma once

typedef struct Entity_t Entity_t;
typedef struct Sprite_t Sprite_t;
typedef struct Engine__State_t Engine__State_t;

void SpriteRenderer__render(Sprite_t* sprite, Engine__State_t* state);
void SpriteRenderer__render2(Entity_t* entity, Engine__State_t* state);