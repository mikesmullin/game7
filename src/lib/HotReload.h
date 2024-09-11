#ifndef HOT_RELOAD_H
#define HOT_RELOAD_H

#include "Engine.h"

extern logic_boot_t logic_boot;
extern logic_oninit_data_t logic_oninit_data;
extern logic_oninit_compute_t logic_oninit_compute;
extern logic_onreload_t logic_onreload;
extern logic_onkey_t logic_onkey;
extern logic_onfinger_t logic_onfinger;
extern logic_onfixedupdate_t logic_onfixedupdate;
extern logic_onupdate_t logic_onupdate;

int load_logic(void);
int unload_logic(void);

#endif  // HOT_RELOAD_H
