// dynamic_allocator.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "platform.h"
#include "memory.h"

namespace Themepark {

template <typename T>
class DynamicBuffer final {
  DISABLE_COPY_AND_MOVE(DynamicBuffer)
public:
  DynamicBuffer();
  DynamicBuffer(DynamicAllocator& allocator);
  ~DynamicBuffer();

private:





} // namespace Themepark

