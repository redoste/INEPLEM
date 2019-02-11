#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <Windows.h>
#include <cstdint>

#include "serviceCore.h"

HANDLE italcWatchdogThread(ServiceCore *service);
uint32_t WINAPI italcWatchdog(LPVOID lpParameter);
#endif
