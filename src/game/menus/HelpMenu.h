#pragma once

typedef struct Arena_t Arena_t;
typedef struct Menu_t Menu_t;
typedef struct Engine__State_t Engine__State_t;

Menu_t* HelpMenu__alloc(Arena_t* state);
void HelpMenu__init(Menu_t* menu, Engine__State_t* state);
void HelpMenu__render(struct Menu_t* menu, Engine__State_t* state);
void HelpMenu__gui(struct Menu_t* menu, Engine__State_t* state);
void HelpMenu__tick(struct Menu_t* menu, Engine__State_t* state);
