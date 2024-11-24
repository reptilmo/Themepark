// memory.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "memory.h"
#include "system.h"
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
  "RENDERER  :",
};

class HeapAllocationTracker final {
  DISABLE_COPY_AND_MOVE(HeapAllocationTracker)
public:
  static HeapAllocationTracker& get() {
    static HeapAllocationTracker instance{};
    return instance;
  }

  ~HeapAllocationTracker() {
    system_mutex_destroy(&mux_);
  }

  inline void update_total_up(u64 bytes) {
    system_mutex_lock(&mux_);
    total_allocated_bytes += bytes;
    system_mutex_unlock(&mux_);
  }

  inline void update_total_down(u64 bytes) {
    system_mutex_lock(&mux_);
    total_allocated_bytes -= bytes;
    system_mutex_unlock(&mux_);
  }

  inline void update_tag_up(MemoryTag tag, u64 bytes) {
    system_mutex_lock(&mux_);
    tag_allocated_bytes[mtag_int(tag)] += bytes;
    tag_allocation_count[mtag_int(tag)]++;
    system_mutex_unlock(&mux_);
  }

  inline void update_tag_down(MemoryTag tag, u64 bytes) {
    system_mutex_lock(&mux_);
    tag_allocated_bytes[mtag_int(tag)] -= bytes;
    tag_allocation_count[mtag_int(tag)]--;
    system_mutex_unlock(&mux_);
  }

  const char* memory_stats_string() {
    static thread_local char buffer[8192]{};
    system_mutex_lock(&mux_);

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

    system_mutex_unlock(&mux_);
    return buffer;
  }

private:
  HeapAllocationTracker() {
    system_mutex_create(&mux_);
  }

  u64 total_allocated_bytes{};
  u64 tag_allocated_bytes[mtag_int(MemoryTag::Count)]{};
  u64 tag_allocation_count[mtag_int(MemoryTag::Count)]{};
  SystemMutex mux_{};
};

}

#ifdef TRACK_HEAP
#define UPDATE_TOTAL_UP(size) HeapAllocationTracker::get().update_total_up((size))
#define UPDATE_TOTAL_DOWN(size) HeapAllocationTracker::get().update_total_down((size))
#define UPDATE_TAG_UP(tag, size) HeapAllocationTracker::get().update_tag_up((tag), (size))
#define UPDATE_TAG_DOWN(tag, size) HeapAllocationTracker::get().update_tag_down((tag), (size))
#define FILL_FREED(addr, size) memset((addr), 'D', size);
#else
#define UPDATE_TOTAL_UP(size)
#define UPDATE_TOTAL_DOWN(size)
#define UPDATE_TAG_UP(tag, size)
#define UPDATE_TAG_DOWN(tag, size)
#define FILL_FREED(addr, size)
#endif


bool DynamicAllocator::startup(u64 size) {
  memory_ = (u8*)::calloc(size, sizeof(u8));
  if (!memory_) {
    LOG_FATAL("DynamicAllocator failed to aquire a block of %d bytes!", size);
    return false;
  }

  memory_size_ = size;
  UPDATE_TOTAL_UP(size);
  return true;
}

void DynamicAllocator::shutdown() {
  if (memory_ && memory_size_ > 0) {
    ::free(memory_);
    UPDATE_TOTAL_DOWN(memory_size_);
  }
}

void* DynamicAllocator::allocate(u64 size, MemoryTag tag) {
  AllocNode* alloc = find_freed(size);
  
  if (alloc != nullptr) {
    
    alloc->chunk_size = size;
    alloc->tag = tag;
    alloc->freed = false;

  } else {
    const u64 total_size = sizeof(AllocNode) + size;
    
    if (total_size <= memory_size_ - memory_used_) {
      alloc = (AllocNode*)(memory_ + memory_used_);
      memory_used_ += total_size;

      alloc->chunk = (u8*)alloc + sizeof(AllocNode);
      alloc->chunk_size = size;
      alloc->tag = tag;
      alloc->freed = false;

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

void DynamicAllocator::free(void* memory, u64 size, MemoryTag tag) {
  if (memory == nullptr && size == 0) {
    return;
  }
  
  AllocNode* alloc = alloc_list_head_;
  for (; alloc != nullptr; alloc = alloc->next) {
    if (alloc->chunk == memory) {
      ASSERT(alloc->chunk_size == size);
      ASSERT(alloc->tag == tag);
      alloc->freed = true;
      FILL_FREED(alloc->chunk, alloc->chunk_size);
      UPDATE_TAG_DOWN(alloc->tag, alloc->chunk_size);
    }
  }
}

DynamicAllocator::AllocNode* DynamicAllocator::find_freed(u64 size) {
  AllocNode* alloc = alloc_list_head_;
  for (; alloc != nullptr; alloc = alloc->next) {
    if (size <= alloc->chunk_size && alloc->freed) {
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

void memory_report_stats() {
  LOG_INFO("~~~~~~~~~~ HEAP ALLOCATIONS ~~~~~~~~~~\n%s",
      HeapAllocationTracker::get().memory_stats_string());
}

} // namespace Themepark
