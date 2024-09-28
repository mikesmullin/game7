#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
typedef uint64_t u64;

#include "OS.h"
#if OS_WINDOWS == 1
#include <Windows.h>
#define SLEEP(ms) Sleep(ms);
#else
#include <unistd.h>
#define SLEEP(ms) sleep(ms);
#endif

void Time__MeasureCycles();
u64 Time__Now();

#endif  // TIMER_H