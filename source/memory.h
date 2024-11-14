// memory.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"

namespace Themepark {

constexpr u64 KiB(u64 value) { return value * 1024ULL; }
constexpr u64 MiB(u64 value) { return value * 1024ULL * 1024ULL; }
constexpr u64 GiB(u64 value) { return value * 1024ULL * 1024ULL * 1024ULL; }

enum class MemoryTag {
  Unknown,
  Mesh,
  Texture,
  Shader,
  Renderer,

  Count
};

class LinearAllocator final {
  DISABLE_COPY_AND_MOVE(LinearAllocator)
public:
  LinearAllocator(u64 max_size);
  ~LinearAllocator();

  void* allocate(u64 size);
  void free(void* memory);
};

class DynamicAllocator final {
  DISABLE_COPY_AND_MOVE(DynamicAllocator)
public:
  DynamicAllocator() = default;
  ~DynamicAllocator() = default;

  bool startup(u64 size);
  void shutdown();

  void* allocate(u64 size, MemoryTag tag);
  void free(void* memory, u64 size, MemoryTag tag);
  
private:
  u8* memory_{};
  u64 memory_size_{};
  u64 memory_used_{};

  struct AllocNode {
    AllocNode* next;
    u8* chunk;
    u64 chunk_size;
    u8 freed;
    MemoryTag tag;
  };

  AllocNode* find_freed(u64 size);
  AllocNode* find_previous(u64 size);

  AllocNode* alloc_list_head_{};
};

void memory_report_stats();

} // namespace Themepark
