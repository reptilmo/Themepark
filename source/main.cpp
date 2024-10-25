// main.cpp

#include "logging.h"
#include "system.h"

bool visuals_startup() {
  glClearColor(0.5f, 0.3f, 0.3f, 1.0f);
  return true;
}

void visuals_run() {
  glClear(GL_COLOR_BUFFER_BIT);
}

void visuals_shutdown() {

}

int main(int argc, char* argv[]) {
  Themepark::SystemContext context = {0};
  context.appname = (u8*)"Themepark";
  context.width = 640;
  context.height = 480;
  context.fullscreen = false;
  context.client_startup = visuals_startup;
  context.client_run = visuals_run;
  context.client_shutdown = visuals_shutdown;

  if (Themepark::system_startup(&context)) {
    Themepark::system_run(&context);
  }

  Themepark::system_shutdown(&context);
  return 0;
}
