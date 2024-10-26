// vec.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "math.h"

namespace Themepark {

struct vec3 final {
  f32 x, y, z;

  vec3() : x(0.0F), y(0.0F), z(0.0F) {}
  ~vec3() = default;

  vec3(f32 a, f32 b, f32 c) : x(a), y(b), z(c) {}
  vec3(const f32* v) : x(v[0]), y(v[1]), z(v[2]) {}

  vec3& operator+=(const vec3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  vec3& operator-=(const vec3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  vec3& operator*=(f32 f) {
    x *= f;
    y *= f;
    z *= f;
    return *this;
  }
};

inline vec3 operator*(const vec3& v, float s) {
  return vec3{
    v.x * s,
    v.y * s,
    v.z * s,
  }; 
}

inline vec3 operator*(float s, const vec3& v) {
  return vec3{
    v.x * s,
    v.y * s,
    v.z * s,
  };
}

inline vec3 operator+(const vec3& a, const vec3& b) {
  return vec3{
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
  };
}

inline vec3 operator-(const vec3& a, const vec3& b) {
  return vec3{
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
  };
}

inline float dot(const vec3& a, const vec3& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline void normalize(vec3& v) {
  const float r = Math::rsqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  v.x *= r;
  v.y *= r;
  v.z *= r;
}

inline vec3 normalized(const vec3& v) {
  const float r = Math::rsqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  return vec3{
    v.x * r,
    v.y * r,
    v.z * r,
  };
}

inline vec3 cross(const vec3& a, const vec3& b) {
  return vec3{
    a.y * b.z - a.z * b.y,
    a.z * b.x - a.x * b.z,
    a.x * b.y - a.y * b.x,
  };
}

} // namespace Themepark
