// math.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"

namespace Themepark {
namespace Math {

constexpr f32 EPSILON = FLT_EPSILON;
constexpr f32 PI = static_cast<f32>(M_PI);
constexpr f32 HALF_PI = PI * 0.5F;

constexpr f32 RADIANS(f32 degrees) { 
  return degrees * PI / 180.0F;
}

inline f32 sin(f32 radians) {
  return sinf(radians);
}

inline f32 cos(f32 radians) {
  return cosf(radians);
}

inline f32 tan(f32 radians) {
  return tanf(radians);
}

inline f32 rsqrt(f32 f) {
  __m128 reg = _mm_set1_ps(f);
  return _mm_cvtss_f32(_mm_rsqrt_ss(reg));
}

inline f32 abs(f32 f) {
  return fabs(f);
}

inline f32 max(f32 a, f32 b) {
  return fmaxf(a, b);
}

} // namespace Math
} // namespace Themepark
