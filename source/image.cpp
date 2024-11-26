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

bool load_tga_file(Image* image,
    DynamicAllocator* allocator,
    const char* filename) {

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

  image->bytes_per_pixel = header.image_bits_per_pixel / 8;
  /*if (bytes_per_pixel != 4) {
    LOG_ERROR("Unsupported image BPP %u!", bytes_per_pixel);
    fclose(file);
    return false;
  }*/

  image->width = (int)header.image_width;
  image->height = (int)header.image_height;

  const u32 image_data_len = image->width * image->height * image->bytes_per_pixel;

  image->data = (u8*)allocator->allocate(sizeof(u8) * image_data_len, MemoryTag::Texture);
  if (image->data == nullptr) {
    LOG_ERROR("Failed to allocate image buffer for %s!", filename);
    fclose(file);
    return false;
  }

  if (fread(image->data, sizeof(u8), image_data_len, file) != image_data_len) {
    LOG_ERROR("Failed to read data for %s!", filename);
    fclose(file);
    return false;
  }

  fclose(file);
  return true;
}

void free_image(Image* image, DynamicAllocator* allocator) {
  ASSERT(image != nullptr && allocator != nullptr);
  allocator->free(image->data,
      image->width * image->height * image->bytes_per_pixel,
      MemoryTag::Texture);
}

} // namespace Themepark
