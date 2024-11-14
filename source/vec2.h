// vec2.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "math.h"

namespace Themepark {

struct vec2 final {
  union {
    struct {
      f32 x, y;
    };
    struct {
      f32 u, v;
    };
  };

  vec2() : x(0.0F), y(0.0F) {}
  ~vec2() = default;

  vec2(f32 a, f32 b) : x(a), y(b) {}
  vec2(const f32* v) : x(v[0]), y(v[1]) {}

  void set(f32 a, f32 b) {
    x = a;
    y = b;
  }

  vec2& operator+=(const vec2& v) {
    x += v.x;
    y += v.y;
    return *this;
  }

  vec2& operator-=(const vec2& v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  vec2& operator*=(f32 f) {
    x *= f;
    y *= f;
    return *this;
  }
};

inline vec2 operator*(const vec2& v, float s) {
  return vec2{
    v.x * s,
    v.y * s,
  }; 
}

inline vec2 operator*(float s, const vec2& v) {
  return vec2{
    v.x * s,
    v.y * s,
  };
}

inline vec2 operator+(const vec2& a, const vec2& b) {
  return vec2{
    a.x + b.x,
    a.y + b.y,
  };
}

inline vec2 operator-(const vec2& a, const vec2& b) {
  return vec2{
    a.x - b.x,
    a.y - b.y,
  };
}

} // namespace Themepark
