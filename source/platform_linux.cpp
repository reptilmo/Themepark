// platform.h
// Kostya Leshenko
// CS447P
// Themepark

#include "platform.h"

void* platform_memory_allocate(u64 size) {
  return malloc(size);
}

void platform_memory_free(void* memory) {
  free(memory);
}

void platform_memory_fill(void* memory, i32 value, u64 size) {
  memset(memory, value, size);
}

void platform_mutex_init(platform_mutex* mutex) {
}

void platform_mutex_destroy(platform_mutex* mutex) {
}

void platform_mutex_lock(platform_mutex* mutex) {
}

void platform_mutex_unlock(platform_mutex* mutex) {
}
