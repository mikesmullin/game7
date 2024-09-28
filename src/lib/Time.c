#include "Time.h"

#include <stdio.h>

#include "Log.h"
#include "OS.h"

#if OS_MAC == 1
#include <mach/mach_time.h>
#endif

// #if OS_WINDOWS == 1
// #include <time.h>
// #endif

#if OS_EMSCRIPTEN == 1
#include <emscripten.h>
#endif

inline __forceinline u64 Now() {
#if OS_MAC == 1
  // NOTICE: this counter pauses while Mac sleeps
  return (u64(mach_absolute_time();

#elif OS_EMSCRIPTEN == 1
  return (u64)(emscripten_get_now() * 1e+6);

#elif defined(__i386__)
  u64 ret;
  __asm__ volatile("rdtsc" : "=A"(ret));
  return ret;

#elif defined(__x86_64__) || defined(__amd64__)
  u64 low, high;
  __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
  return (u64)((high << 32) | low);

#else
#error OS not supported by cycle timer implementation

#endif
}

static u64 CYCLES_PER_SECOND;
static u64 CYCLES_PER_MILLISECOND;
static FILE* cache;

// A cycle timer is a kind of monotonic clock
// this is a cross-platform implementation
// it provides a single function `u64 Now()` which returns time since PC power on
// it uses the RDTSC (Read Time-Stamp Counter) instruction to count CPU cycles
// it is fast because it avoids a system call
// it has some limitations
// - performance will vary by cpu
// - consistency is not guaranteed across threads
// - you can measure milliseconds, but not exactly cycles-per-opcode
void Time__MeasureCycles() {
  fopen_s(&cache, "../cpu_cycles_per_sec", "r");
  if (cache) {
    fread_s(&CYCLES_PER_SECOND, sizeof(u64), sizeof(u64), 1, cache);
    fclose(cache);
  } else {
    u64 start = Now();
    SLEEP(1000)
    u64 end = Now();
    CYCLES_PER_SECOND = end - start;

    fopen_s(&cache, "../cpu_cycles_per_sec", "w");
    fwrite(&CYCLES_PER_SECOND, sizeof(u64), 1, cache);
    fclose(cache);
  }
  LOG_DEBUGF("CYCLES_PER_SECOND = %llu", CYCLES_PER_SECOND);
  CYCLES_PER_MILLISECOND = CYCLES_PER_SECOND / 1000;
}

u64 Time__Now() {
  return Now() / CYCLES_PER_MILLISECOND;
}