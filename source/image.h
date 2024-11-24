// image.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "memory.h"

namespace Themepark {

struct Image final {
  DISABLE_COPY_AND_MOVE(Image);
public:
  Image(DynamicAllocator* allocator);
  ~Image();

  bool load_tga_file(const char* filename);

  u8* data;
  u32 width;
  u32 height;
  u32 bytes_per_pixel;
  DynamicAllocator* allocator;
};

} // namespace Themepark
