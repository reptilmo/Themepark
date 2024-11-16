// dynarray.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "dynarray.h"
#include "logging.h"

namespace Themepark {

bool read_file(DynArray<i8>* data, const char* filename) {
  ASSERT(data != nullptr);

  FILE* file = fopen(filename, "rb");
  if (file == nullptr) {
    LOG_ERROR("Failed to open %s!", filename);
    return false;
  }

  i8 buf[MAX_READ_LEN];
  u64 len = 0;
  u64 total = 0;

  while ((len = fread(buf, sizeof(i8), MAX_READ_LEN, file)) > 0) {
    data->push_back(buf, len);
    total += len;
  }

  fclose(file);
  return total > 0;
}

} // namespace Themepark
