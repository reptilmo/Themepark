// mat4.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "math.h"

namespace Themepark {

struct mat4 {
  f32 m[16];
};

inline mat4 mat4_identity() {
  return mat4{{
    1.0F, 0.0F, 0.0F, 0.0F,
    0.0F, 1.0F, 0.0F, 0.0F,
    0.0F, 0.0F, 1.0F, 0.0F,
    0.0F, 0.0F, 0.0F, 1.0F,
  }};
}

inline mat4 mat4_perspective(f32 fov, f32 front, f32 back, f32 aspect_ratio) {
  const f32 top = front * Math::tan(Math::RADIANS(fov * 0.5F));
  const f32 bottom = -top;
  const f32 right = top * aspect_ratio;
  const f32 left = -right;

  return mat4{{
    2 * front / (right - left),
    0,
    0,
    0,

    0,
    2 * front / (top - bottom),
    0,
    0,

    (right + left) / (right - left),
    (top + bottom) / (top - bottom),
    -(back + front) / (back - front),
    -1,

    0,
    0,
    -2 * front * back / (back - front),
    0,
  }};
}

inline mat4 mat4_translate(f32 x, f32 y, f32 z) {
  return mat4{{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    x, y, z, 1,
  }};
}


} // namespace Themepark
