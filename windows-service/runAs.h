#ifndef RUNAS_H
#define RUNAS_H

#include <cstdint>
#include <string>

std::string runAsService(uint32_t sessionId, std::string cmdLine);

#endif