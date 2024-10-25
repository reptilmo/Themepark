// defined.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Unsigned integers
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

// Signed integers
using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

// Floating point
using f32 = float;
using f64 = double;

static_assert(sizeof(u8) == 1, "Unexpected u8 size, must be 1 byte!");
static_assert(sizeof(u16) == 2, "Unexpected u16 size, must be 2 bytes!");
static_assert(sizeof(u32) == 4, "Unexpected u32 size, must be 4 bytes!");
static_assert(sizeof(u64) == 8, "Unexpected u64 size, must be 8 bytes!");

static_assert(sizeof(i8) == 1, "Unexpected i8 size, must be 1 byte!");
static_assert(sizeof(i16) == 2, "Unexpected i16 size, must be 2 bytes!");
static_assert(sizeof(i32) == 4, "Unexpected i32 size, must be 4 bytes!");
static_assert(sizeof(i64) == 8, "Unexpected i64 size, must be 8 bytes!");

static_assert(sizeof(f32) == 4, "Unexpected f32 size, must be 4 bytes!");
static_assert(sizeof(f64) == 8, "Unexpected f64 size, must be 8 bytes!");

static_assert(sizeof(bool) == 1, "Unexpected bool size, must be 1 byte!");

constexpr u32 U32_MAX = UINT_MAX;


#ifdef DEBUG_BUILD
#define ASSERT(expr) \
  do { \
    if (!(expr)) { \
      raise(SIGTRAP); \
    } \
  } while (0)
#else
#define ASSERT(expr)
#endif


#define DISABLE_COPY_AND_MOVE(ClassName) \
  ClassName(const ClassName&) = delete; \
  ClassName& operator= (const ClassName&) = delete; \
  ClassName(ClassName&&) noexcept = delete; \
  ClassName& operator= (ClassName&&) noexcept = delete;


void* platform_memory_allocate(u64 size);
void platform_memory_free(void* memory);
void platform_memory_fill(void* memory, i32, u64 size);

struct platform_mutex{};
void platform_mutex_init(platform_mutex* mutex);
void platform_mutex_destroy(platform_mutex* mutex);
void platform_mutex_lock(platform_mutex* mutex);
void platform_mutex_unlock(platform_mutex* mutex); 
