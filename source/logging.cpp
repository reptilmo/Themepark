// logging.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "logging.h"

namespace Themepark {

void log(LogLevel level, const u8* message, ...) {

  static const char* level_str[5] = {
    "FATAL",
    "ERROR",
    "INFO",
    "DEBUG",
    "TRACE"
  };

  u8 buffer[2048];
  memset(buffer, 0, sizeof(buffer));

  va_list args;
  va_start(args, message);
  if (vsnprintf((char*)buffer, sizeof(buffer), (char*)message, args) > 0) {
    fprintf(stdout, "%s %s\n", level_str[(i32)level], (char*)buffer);
  }

  va_end(args);
}
} // namespace Themepark
