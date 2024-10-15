#pragma once

typedef struct RubbleSprite_t RubbleSprite;
typedef struct Engine__State_t Engine__State_t;

void RubbleSprite__init(RubbleSprite* sprite, Engine__State_t* state);
void RubbleSprite__tick(RubbleSprite* sprite, Engine__State_t* state);
void RubbleSprite__render(RubbleSprite* sprite, Engine__State_t* state);