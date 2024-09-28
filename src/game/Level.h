#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>

typedef uint8_t u8;
typedef struct Engine__State_t Engine__State_t;

void Level__Load(Engine__State_t* game, u8 id);
void Level__Render(Engine__State_t* game);
void Level__RNG(Engine__State_t* game);

#endif  // LEVEL_H