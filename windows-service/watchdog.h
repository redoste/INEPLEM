#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <Windows.h>
#include <cstdint>

HANDLE italcWatchdogThread();
uint32_t WINAPI italcWatchdog(LPVOID lpParameter);
#endif
