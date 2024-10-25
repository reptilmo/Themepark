// system.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "system.h"
#include "logging.h"

namespace Themepark {
namespace {

void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

void keyboard_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
  //TODO:
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  //TODO:
}

} // anonymous namespace

bool system_startup(SystemContext* context) {
  ASSERT(context != nullptr);
  if (context == nullptr) {
    return false;
  }

  // TODO:
  if (context->width < 800 || context->height < 600) {
    LOG_INFO("SYS setting resolution to 800x600");
    context->width = 800;
    context->height = 600;
  }

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    LOG_ERROR("SYS failed to initialize GLFW");
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  //glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
  //glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);

  if (context->fullscreen) {
    // TODO:
    LOG_ERROR("SYS fullscreen is not implemented");
    return false;
  
  } else {
    context->window = glfwCreateWindow(
        context->width,
        context->height,
        (char*)context->appname,
        NULL,
        NULL);
  }

  if (context->window == nullptr) {
    return false;
  }

  glfwSetKeyCallback(context->window, keyboard_key_callback);
  glfwSetCursorPosCallback(context->window, mouse_position_callback);
  glfwSetMouseButtonCallback(context->window, mouse_button_callback);
  glfwMakeContextCurrent(context->window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwTerminate();
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

  return true;
}

void system_run(SystemContext* context) {
  ASSERT(context != nullptr);
  ASSERT(context->window != nullptr);
  ASSERT(context->client_startup != nullptr);
  ASSERT(context->client_run != nullptr);
  ASSERT(context->client_shutdown != nullptr);

  if (context->client_startup()) {
    while (!glfwWindowShouldClose(context->window)) {
      context->client_run();
      glfwSwapBuffers(context->window);
      glfwPollEvents();
    }
  }

  context->client_shutdown();
}

void system_shutdown(SystemContext* context) {
  if (context->window != nullptr) {
    glfwDestroyWindow(context->window);
  }
  glfwTerminate();
}

} // namespace Themepark
