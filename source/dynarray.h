// dynarray.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "memory.h"

namespace Themepark {

template <typename T>
class DynArray final {
  DISABLE_COPY_AND_MOVE(DynArray);
public:
  DynArray() = default;
  ~DynArray() = default;

  void init(DynamicAllocator* alloc, MemoryTag t) {
    ASSERT(alloc != nullptr);
    ASSERT(t != MemoryTag::Unknown);
    allocator = alloc;
    tag = t;
  }

  void reset() {
    used = 0;
  }

  void clear() {
    ASSERT(allocator != nullptr);
    allocator->free(buffer, sizeof(T) * capacity, tag);
    capacity = 0;
    used = 0;
  }

  const T* data() const {
    return buffer;
  }

  u64 size() const {
    return used;
  }

  const T& operator[](u64 i) const {
    ASSERT(i >= 0 && i < used);
    return buffer[i];
  }

 T& operator[](u64 i) {
    ASSERT(i >= 0 && i < used);
    return buffer[i];
  }


  void push_back(T& value) {
    if (used >= capacity) {
      realloc(10);
    }

    buffer[used] = value;
    used++;
  }

  void push_back(T* data, u64 size) {
    if (used + size >= capacity) {
      realloc(size);
    }

    memcpy(&buffer[used], data, size);
    used += size;
  }

private:
  void realloc(u64 more) {
    u64 new_capacity = capacity;
    if (new_capacity == 0) {
      new_capacity = more;
    } else {
      new_capacity = more + (new_capacity * 2);
    }

    ASSERT(new_capacity < (u64)U32_MAX);
    T* new_buffer = (T*)allocator->allocate(sizeof(T) * new_capacity, tag);
    memcpy(new_buffer, buffer, sizeof(T) * used);
    allocator->free(buffer, sizeof(T) * capacity, tag);
    capacity = new_capacity;
    buffer = new_buffer;
  }

  T* buffer{};
  u64 capacity{};
  u64 used{};
  DynamicAllocator* allocator{};
  MemoryTag tag{};
};

bool read_file(DynArray<i8>* data, const char* filename);

} // namespace Themepark
