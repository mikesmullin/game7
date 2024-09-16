#include "Timer.h"

#include <stdio.h>

#include "Base.h"

static u64 CYCLES_PER_SECOND;
static u64 CYCLES_PER_MILLISECOND;
static FILE* cache;

void Timer__MeasureCycles() {
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

f32 Timer__NowSeconds() {
  return (f32)Now() / CYCLES_PER_SECOND;
}

u32 Timer__NowMilliseconds() {
  return Now() / CYCLES_PER_MILLISECOND;
}