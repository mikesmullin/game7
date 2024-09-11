#include "game/Logic.h"
#include "lib/Base.h"
#include "lib/File.h"
#include "lib/HotReload.h"

Engine__State_t* state = &(Engine__State_t){};
FileMonitor_t* fm = &(FileMonitor_t){.directory = "src/game", .fileName = "Logic.c.dll"};

static int check_load_logic() {
  // if (2 == File__CheckMonitor(fm)) {
  return load_logic();
  // }
  // return 0;
}

int main() {
  LOG_INFOF("begin main.");
  state->check_load_logic = &check_load_logic;

  File__StartMonitor(fm);

  if (!load_logic()) {
    return 1;
  }
  logic_boot(state);

  LOG_INFOF("shutdown main.");
  File__EndMonitor(fm);
  unload_logic();
  LOG_INFOF("end main.");
  return 0;
}