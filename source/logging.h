// assert.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"

namespace Themepark {

enum class LogLevel {
  Fatal,
  Error,
  Info,
};

void log(LogLevel level, const u8* message, ...);

} // namespace Themepark


#define LOG_FATAL(message, ...) \
  Themepark::log(Themepark::LogLevel::Fatal, (const u8*)message, ##__VA_ARGS__)

#define LOG_ERROR(message, ...) \
  Themepark::log(Themepark::LogLevel::Error, (const u8*)message, ##__VA_ARGS__)

#define LOG_INFO(message, ...) \
  Themepark::log(Themepark::LogLevel::Info, (const u8*)message, ##__VA_ARGS__)

