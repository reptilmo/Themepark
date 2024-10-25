// memory.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "memory.h"
#include "logging.h"

namespace Themepark {
namespace {

constexpr f64 cKiB = 1024.0;
constexpr f64 cMiB = 1024.0 * 1024.0;
constexpr f64 cGiB = 1024.0 * 1024.0 * 1024.0;

constexpr const char* XiB[4] = {"KiB", "MiB", "GiB", "bytes"};

constexpr u32 mtag_int(MemoryTag tag) {
  return static_cast<u32>(tag);
}

constexpr u32 calc_units(f64* total) {
  u32 units_index = 3;

  if (*total >= cGiB) {
    *total = *total / cGiB;
    units_index = 2;
  } else if (*total >= cMiB) {
    *total = *total / cMiB;
    units_index = 1;
  } else if (*total >= cKiB) {
    *total = *total / cKiB;
    units_index = 0;
  }

  return units_index;
}

constexpr const char* memory_tag_str[mtag_int(MemoryTag::Count)] = {
  "UNKNOWN   :",
  "MESH      :",
  "TEXTURE   :",
  "SHADER    :",
};

class HeapAllocationTracker final {
  DISABLE_COPY_AND_MOVE(HeapAllocationTracker)
public:
  static HeapAllocationTracker& get() {
    static HeapAllocationTracker instance{};
    return instance;
  }

  ~HeapAllocationTracker() {
    platform_mutex_destroy(&mux_);
  }

  inline void update_total_up(u64 bytes) {
    platform_mutex_lock(&mux_);
    total_allocated_bytes += bytes;
    platform_mutex_unlock(&mux_);
  }

  inline void update_total_down(u64 bytes) {
    platform_mutex_lock(&mux_);
    total_allocated_bytes -= bytes;
    platform_mutex_unlock(&mux_);
  }

  inline void update_tag_up(MemoryTag tag, u64 bytes) {
    platform_mutex_lock(&mux_);
    tag_allocated_bytes[mtag_int(tag)] += bytes;
    tag_allocation_count[mtag_int(tag)]++;
    platform_mutex_unlock(&mux_);
  }

  inline void update_tag_down(MemoryTag tag, u64 bytes) {
    platform_mutex_lock(&mux_);
    tag_allocated_bytes[mtag_int(tag)] -= bytes;
    tag_allocation_count[mtag_int(tag)]--;
    platform_mutex_unlock(&mux_);
  }

  const char* memory_stats_string() {
    static thread_local char buffer[8192]{};
    platform_mutex_lock(&mux_);

    i32 used_len = 0;
    for (u32 i = 0; i < mtag_int(MemoryTag::Count); i++) {

      u64 count = tag_allocation_count[i];
      f64 total = f64(tag_allocated_bytes[i]);

      u32 u = calc_units(&total);

      i32 len = snprintf(buffer + used_len, sizeof(buffer) - used_len,
        "  %s %llu allocations of %0.2f%s\n", memory_tag_str[i], count, total, XiB[u]);

      used_len += len;
    }

    f64 total = f64(total_allocated_bytes);
    u32 u = calc_units(&total);
    snprintf(buffer + used_len, sizeof(buffer) - used_len, "Total %0.2f%s\n", total, XiB[u]);

    platform_mutex_unlock(&mux_);
    return buffer;
  }

private:
  HeapAllocationTracker() {
    platform_mutex_init(&mux_);
  }

  u64 total_allocated_bytes{};
  u64 tag_allocated_bytes[mtag_int(MemoryTag::Count)]{};
  u64 tag_allocation_count[mtag_int(MemoryTag::Count)]{};
  platform_mutex mux_{};
};

#ifdef TRACK_HEAP
#define UPDATE_TOTAL_UP(size) HeapAllocationTracker::get().update_total_up((size))
#define UPDATE_TOTAL_DOWN(size) HeapAllocationTracker::get().update_total_down((size))
#define UPDATE_TAG_UP(tag, size) HeapAllocationTracker::get().update_tag_up((tag), (size))
#define UPDATE_TAG_DOWN(tag, size) HeapAllocationTracker::get().update_tag_down((tag), (size))
#define FILL_FREED(addr, size) platform_memory_fill((addr), 'D', size);
#else
#define UPDATE_TOTAL_UP(size)
#define UPDATE_TOTAL_DOWN(size)
#define UPDATE_TAG_UP(tag, size)
#define UPDATE_TAG_DOWN(tag, size)
#define FILL_FREED(addr, size)
#endif

class DynamicAllocator final {
  DISABLE_COPY_AND_MOVE(DynamicAllocator)
public:
  static DynamicAllocator& get();
  DynamicAllocator() = default;
  ~DynamicAllocator() = default;

  b8 startup(u64 size);
  void shutdown();

  void* allocate(u64 size, MemoryTag tag);
  b8 free(void* memory, u64 size, MemoryTag tag);
  
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
  AllocNode* alloc_list_tail_{};
};

DynamicAllocator& DynamicAllocator::get() {
  static DynamicAllocator instance;
  return instance;
}

b8 DynamicAllocator::startup(u64 size) {
  memory_ = (u8*)platform_memory_allocate(size);
  if (!memory_) {
    LOG_FATAL("DynamicAllocator failed to aquire a block of %d bytes!", size);
    return FALSE;
  }

  memory_size_ = size;
  UPDATE_TOTAL_UP(size);
  return TRUE;
}

void DynamicAllocator::shutdown() {
  if (memory_ && memory_size_ > 0) {
    platform_memory_free(memory_);
    UPDATE_TOTAL_DOWN(memory_size_);
  }
}

void* DynamicAllocator::allocate(u64 size, MemoryTag tag) {
  AllocNode* alloc = find_freed(size);
  
  if (alloc != nullptr) {
    
    alloc->chunk_size = size;
    alloc->tag = tag;
    alloc->freed = FALSE;

  } else {
    const u64 total_size = sizeof(AllocNode) + size;
    
    if (total_size <= memory_size_ - memory_used_) {
      alloc = (AllocNode*)memory_ + memory_used_;
      memory_used_ += total_size;

      alloc->chunk = (u8*)alloc + sizeof(AllocNode);
      alloc->chunk_size = size;
      alloc->tag = tag;
      alloc->freed = FALSE;

      if (!alloc_list_head_ || alloc_list_head_->chunk_size <= size) {
        alloc->next = alloc_list_head_;
        alloc_list_head_ = alloc;
      } else {
        AllocNode* previous = find_previous(size);
        ASSERT(previous);
        alloc->next = previous->next;
        previous->next = alloc;
      }
    }
  }

  if (alloc) {
    UPDATE_TAG_UP(tag, size);
    return alloc->chunk;
  }

  return nullptr;
}



b8 DynamicAllocator::free(void* memory, u64 size, MemoryTag tag) {
  AllocNode* alloc = alloc_list_head_;

  for (; alloc != nullptr; alloc = alloc->next) {
    if (alloc->chunk == memory) {
      ASSERT(alloc->chunk_size == size);
      ASSERT(alloc->tag == tag);
      alloc->freed = TRUE;
      FILL_FREED(alloc->chunk, alloc->chunk_size);
      UPDATE_TAG_DOWN(alloc->tag, alloc->chunk_size);
      return TRUE;
    }
  }

  return FALSE;
}


DynamicAllocator::AllocNode* DynamicAllocator::find_freed(u64 size) {
  AllocNode* alloc = alloc_list_head_;
  for (; alloc != nullptr; alloc = alloc->next) {
    if (alloc->chunk_size <= size && alloc->freed) {
      return alloc;
    }
  }
  return nullptr;
}

DynamicAllocator::AllocNode* DynamicAllocator::find_previous(u64 size) {
  AllocNode* alloc = alloc_list_head_;
  while (alloc->next != nullptr && alloc->next->chunk_size < size) {
    alloc = alloc->next;
  }
  return alloc;
}

} // internal namespace



} // namespace Themepark
