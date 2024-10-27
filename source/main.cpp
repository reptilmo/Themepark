// main.cpp
// Kostya Leshenko
// CS447P
// Themepark


#include "logging.h"
#include "system.h"
#include "memory.h"
#include "input.h"
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

  Themepark::Input input;

  if (Themepark::system_startup(&context)) {
    Themepark::system_run(&context, &input);
  }

  Themepark::system_shutdown(&context);
  Themepark::memory_report_stats();
  return 0;
}
