// image.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "memory.h"

namespace Themepark {

struct Image {
  u8* data;
  u32 width;
  u32 height;
  u32 bytes_per_pixel;
};

bool load_tga_file(Image* image, DynamicAllocator* allocator, const char* filename);
void free_image(Image* image);

} // namespace Themepark
