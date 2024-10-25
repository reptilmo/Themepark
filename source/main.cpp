// main.cpp

#include "logging.h"
#include "system.h"
#include "themepark.h"

int main(int argc, char* argv[]) {
  Themepark::SystemContext context = {0};
  context.appname = (u8*)"Themepark";
  context.width = 640;
  context.height = 480;
  context.fullscreen = false;
  context.client_startup = Themepark::themepark_startup;
  context.client_run = Themepark::themepark_run;
  context.client_shutdown = Themepark::themepark_shutdown;

  if (Themepark::system_startup(&context)) {
    Themepark::system_run(&context);
  }

  Themepark::system_shutdown(&context);
  return 0;
}
