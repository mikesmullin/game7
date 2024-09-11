#ifndef HOT_RELOAD_H
#define HOT_RELOAD_H

#include "../game/Logic.h"

extern logic_boot_t logic_boot;

int load_logic(void);
int unload_logic(void);

#endif  // HOT_RELOAD_H
