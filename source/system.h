// system.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "input.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>

namespace Themepark {

typedef bool (*ClientStartupCallback)();
typedef void (*ClientRunCallback)(void*);
typedef void (*ClientShutdownCallback)();

typedef struct SystemContext {
  u32 width;
  u32 height;
  u8* appname;
  SDL_Window* window;
  SDL_GLContext glcontext;
  bool fullscreen;
  bool running;
  ClientStartupCallback client_startup;
  ClientRunCallback client_run;
  ClientShutdownCallback client_shutdown;
} SystemContext;

bool system_valid_context(SystemContext* context);
bool system_startup(SystemContext* context);
void system_run(SystemContext* context, Input* input);
void system_shutdown(SystemContext* context);

typedef struct SystemMutex {
  SDL_Mutex* mutex;
} SystemMutex;

bool system_mutex_create(SystemMutex* m);
void system_mutex_lock(SystemMutex* m);
void system_mutex_unlock(SystemMutex* m);
void system_mutex_destroy(SystemMutex* m);

} // namespace Themepark
