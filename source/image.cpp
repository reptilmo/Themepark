// image.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "image.h"
#include "logging.h"

namespace Themepark {

// https://en.wikipedia.org/wiki/Truevision_TGA
typedef struct TGAHeader {
  u8 id_field_length;
  u8 color_map_type;
  u8 image_type;
  u8 color_map_spec[5];
  u16 origin_x;
  u16 origin_y;
  u16 image_width;
  u16 image_height;
  u8 image_bits_per_pixel;
  u8 image_descriptor;
} TGAHeader;


Image::Image(DynamicAllocator* alloc) 
  : data(nullptr)
  , width(0)
  , height(0)
  , bytes_per_pixel(0)
  , allocator(alloc)
{}

Image::~Image() {
  ASSERT(allocator != nullptr);
  if (data != nullptr) {
    allocator->free(data, bytes_per_pixel * width * height, MemoryTag::Texture);
  }
}

bool Image::load_tga_file(const char* filename) {

  FILE* file = fopen(filename, "rb");
  if (file == nullptr) {
    LOG_ERROR("Failed to open %s!", filename);
    return false;
  }

  TGAHeader header;
  memset(&header, 0, sizeof(TGAHeader));
  if (fread(&header, sizeof(TGAHeader), 1, file) < 1) {
    LOG_ERROR("Failed to open %s!", filename);
    fclose(file);
    return false;
  }

  if (header.image_type != 2) {
    LOG_ERROR("Unsupported image type %s!", filename);
    fclose(file);
    return false;
  }

  bytes_per_pixel = header.image_bits_per_pixel / 8;
  /*if (bytes_per_pixel != 4) {
    LOG_ERROR("Unsupported image BPP %u!", bytes_per_pixel);
    fclose(file);
    return false;
  }*/

  width = (int)header.image_width;
  height = (int)header.image_height;

  const u32 image_data_len = width * height * bytes_per_pixel;
  data = (u8*)allocator->allocate(sizeof(u8) * image_data_len, MemoryTag::Texture);
  if (data == nullptr) {
    LOG_ERROR("Failed to allocate image buffer for %s!", filename);
    fclose(file);
    return false;
  }

  if (fread(data, sizeof(u8), image_data_len, file) != image_data_len) {
    LOG_ERROR("Failed to read data for %s!", filename);
    fclose(file);
    return false;
  }

  fclose(file);
  return true;
}

} // namespace Themepark
