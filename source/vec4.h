// vec4.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "math.h"

namespace Themepark {

struct vec4 final {
  f32 x, y, z, w;

  vec4() : x(0.0F), y(0.0F), z(0.0F), w(0.0F) {}
  ~vec4() = default;

  vec4(f32 a, f32 b, f32 c, f32 d) : x(a), y(b), z(c), w(d) {}
  vec4(const f32* v) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}
};

} // namespace Themepark
