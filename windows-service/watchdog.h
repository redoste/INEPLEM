#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <Windows.h>

HANDLE italcWatchdogThread();
uint32_t WINAPI italcWatchdog(LPVOID lpParameter);
#endif
