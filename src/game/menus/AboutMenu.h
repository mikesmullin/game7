#pragma once

typedef struct Arena_t Arena_t;
typedef struct Menu_t Menu_t;
typedef struct Engine__State_t Engine__State_t;

Menu_t* AboutMenu__alloc(Arena_t* state);
void AboutMenu__init(Menu_t* menu, Engine__State_t* state);
void AboutMenu__render(Menu_t* menu, Engine__State_t* state);
void AboutMenu__gui(struct Menu_t* menu, Engine__State_t* state);
void AboutMenu__tick(Menu_t* menu, Engine__State_t* state);