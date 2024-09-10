#ifndef LOGIC_H
#define LOGIC_H

#include "../lib/Engine.h"

typedef void (*logic_init_t)(Engine__State_t*);
typedef void (*logic_init2_t)(Engine__State_t*);
typedef void (*logic_init3_t)(Engine__State_t*);
typedef void (*logic_update_t)(Engine__State_t*);
typedef void (*logic_draw_t)(const Engine__State_t*);

#endif  // LOGIC_H
