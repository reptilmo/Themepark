// system.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "system.h"
#include "logging.h"

namespace Themepark {
namespace {

} // anonymous namespace

bool system_startup(SystemContext* context) {
  context->running = false;
  ASSERT(context != nullptr);
  if (context == nullptr) {
    return false;
  }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    LOG_FATAL("SDL failed to initialize!");
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // TODO:
  if (context->width < 800 || context->height < 600) {
    LOG_INFO("SYS setting resolution to 800x600");
    context->width = 800;
    context->height = 600;
  }

  if (context->fullscreen) {
    // TODO:
    LOG_FATAL("SYS fullscreen is not implemented");
    return false;
  
  } else {
    context->window = SDL_CreateWindow(
        (char*)context->appname,
        context->width,
        context->height,
        SDL_WINDOW_OPENGL);
  }

  if (context->window == nullptr) {
    LOG_FATAL("SYS %s", SDL_GetError());
    return false;
  }

  context->glcontext = SDL_GL_CreateContext(context->window);
  if (context->glcontext == nullptr) {
    LOG_FATAL("SYS %s", SDL_GetError());
    return false;
  }

  if (!gladLoadGL()) {
    LOG_FATAL("SYS gladLoadGL failed!");
    return false;
  }

  const auto renderer = glGetString(GL_RENDERER);
  if (renderer != nullptr) {
    LOG_INFO("Renderer: %s", renderer);
  }

  const auto version = glGetString(GL_VERSION);
  if (version != nullptr) {
    LOG_INFO("OpenGL version: %s", version);
  }

  context->running = true;
  return context->running;
}

void system_run(SystemContext* context) {
  ASSERT(context != nullptr);
  ASSERT(context->window != nullptr);
  ASSERT(context->running == true);
  ASSERT(context->client_startup != nullptr);
  ASSERT(context->client_run != nullptr);
  ASSERT(context->client_shutdown != nullptr);

  if (context->client_startup()) {
    SDL_Event close_event;
    while (context->running) {
      SDL_PumpEvents();
      if (SDL_PeepEvents(nullptr, 1,
            SDL_PEEKEVENT,
            SDL_EVENT_WINDOW_CLOSE_REQUESTED,
            SDL_EVENT_WINDOW_CLOSE_REQUESTED) > 0) {
        context->running = false;
      }
      context->client_run();
      SDL_GL_SwapWindow(context->window);
    }
  }

  context->client_shutdown();
}

void system_shutdown(SystemContext* context) {
  if (context->glcontext != nullptr) {
    SDL_GL_DestroyContext(context->glcontext);
  }
  if (context->window != nullptr) {
    SDL_DestroyWindow(context->window);
  }
  SDL_Quit();
}

} // namespace Themepark
