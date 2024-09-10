#ifndef HOT_RELOAD_H
#define HOT_RELOAD_H

#include "../game/Logic.h"

#define LOGIC_FILENAME "src\\game\\Logic.c.dll"

extern logic_init_t logic_init;
extern logic_init2_t logic_init2;
extern logic_init3_t logic_init3;
extern logic_update_t logic_update;
extern logic_draw_t logic_draw;

int load_logic(void);
int unload_logic(void);

#endif  // HOT_RELOAD_H
