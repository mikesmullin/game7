#include "HotReload.h"

#include <stdio.h>

static void* logic = NULL;

logic_init_t logic_init;
logic_update_t logic_update;
logic_draw_t logic_draw;

#if OS_NIX == 1
#include <dlfcn.h>

#define LOAD_SYMBOL(name)                                                      \
  do {                                                                         \
    name = (name##_t)dlsym(logic, #name);                                      \
    if (name == NULL) {                                                        \
      fprintf(stderr, "ERROR could not find symbol %s: %s", #name, dlerror()); \
      return 1;                                                                \
    }                                                                          \
  } while (0)

int load_logic(void) {
  if (logic != NULL) {
    if (unload_logic()) {
      return 1;
    }
  }

  logic = dlopen(LOGIC_FILENAME, RTLD_NOW);
  if (logic == NULL) {
    fprintf(stderr, "ERROR loading %s: %s\n", LOGIC_FILENAME, dlerror());
    return 1;
  }

  LOAD_SYMBOL(logic_init);
  LOAD_SYMBOL(logic_update);
  LOAD_SYMBOL(logic_draw);

  return 0;
}

int unload_logic(void) {
  if (dlclose(logic)) {
    fprintf(stderr, "ERROR unloading %s: %s\n", LOGIC_FILENAME, dlerror());
    return 1;
  }

  return 0;
}
#endif

// #if OS_WINDOWS == 1
#include <windows.h>

int load_logic(void) {
  if (logic != NULL) {
    if (unload_logic()) {
      return 1;
    }
  }

  logic = LoadLibrary(LOGIC_FILENAME);
  if (!logic) {
    // Retrieve the error code
    DWORD errorCode = GetLastError();

    // Print the error code
    printf("LoadLibrary failed with error code: %lu\n", errorCode);

    // Optionally, format the error message into a readable string
    LPVOID errorMessage;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&errorMessage,
        0,
        NULL);

    // Print the formatted error message
    printf("ERROR loading %s: %s\n", LOGIC_FILENAME, (char*)errorMessage);

    // Free the buffer allocated by FormatMessage
    LocalFree(errorMessage);

    return 1;
  }

  logic_init = (void (*)())GetProcAddress(logic, "logic_init");
  logic_update = (void (*)())GetProcAddress(logic, "logic_update");
  logic_draw = (void (*)())GetProcAddress(logic, "logic_draw");

  return 0;
}

int unload_logic(void) {
  int r = FreeLibrary(logic);
  if (r == 0) {
    // Retrieve the error code
    DWORD errorCode = GetLastError();

    // Print the error code
    printf("FreeLibrary failed with error code: %lu\n", errorCode);

    // Optionally, format the error message into a readable string
    LPVOID errorMessage;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&errorMessage,
        0,
        NULL);

    // Print the formatted error message
    printf("ERROR freeing %s: %s\n", LOGIC_FILENAME, (char*)errorMessage);

    // Free the buffer allocated by FormatMessage
    LocalFree(errorMessage);

    return 1;
  }

  logic = NULL;

  return 0;
}

// #endif