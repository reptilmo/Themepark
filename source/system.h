// system.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Themepark {

typedef bool (*ClientStartupCallback)();
typedef void (*ClientRunCallback)();
typedef void (*ClientShutdownCallback)();

typedef struct SystemContext {
  u32 width;
  u32 height;
  u8* appname;
  GLFWwindow* window;
  bool fullscreen;
  ClientStartupCallback client_startup;
  ClientRunCallback client_run;
  ClientShutdownCallback client_shutdown;
} SystemContext;

bool system_valid_context(SystemContext* context);
bool system_startup(SystemContext* context);
void system_run(SystemContext* context);
void system_shutdown(SystemContext* context);


} // namespace Themepark
