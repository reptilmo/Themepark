// themepark.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "system.h"

namespace Themepark {

bool themepark_startup(u32 view_width, u32 view_height);
void themepark_run(RunContext* run_context);
void themepark_shutdown();

} // namespace Themepark
