// mat4.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "math.h"
#include "vec3.h"
#include "vec4.h"

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

inline mat4 operator* (const mat4& l, const mat4& r) {
  return mat4{{
    l.m[0] * r.m[0] + l.m[1] * r.m[4] + l.m[2] * r.m[8] + l.m[3] * r.m[12],
    l.m[0] * r.m[1] + l.m[1] * r.m[5] + l.m[2] * r.m[9] + l.m[3] * r.m[13],
    l.m[0] * r.m[2] + l.m[1] * r.m[6] + l.m[2] * r.m[10] + l.m[3] * r.m[14],
    l.m[0] * r.m[3] + l.m[1] * r.m[7] + l.m[2] * r.m[11] + l.m[3] * r.m[15],

    l.m[4] * r.m[0] + l.m[5] * r.m[4] + l.m[6] * r.m[8] + l.m[7] * r.m[12],
    l.m[4] * r.m[1] + l.m[5] * r.m[5] + l.m[6] * r.m[9] + l.m[7] * r.m[13],
    l.m[4] * r.m[2] + l.m[5] * r.m[6] + l.m[6] * r.m[10] + l.m[7] * r.m[14],
    l.m[4] * r.m[3] + l.m[5] * r.m[7] + l.m[6] * r.m[11] + l.m[7] * r.m[15],

    l.m[8] * r.m[0] + l.m[9] * r.m[4] + l.m[10] * r.m[8] + l.m[11] * r.m[12],
    l.m[8] * r.m[1] + l.m[9] * r.m[5] + l.m[10] * r.m[9] + l.m[11] * r.m[13],
    l.m[8] * r.m[2] + l.m[9] * r.m[6] + l.m[10] * r.m[10] + l.m[11] * r.m[14],
    l.m[8] * r.m[3] + l.m[9] * r.m[7] + l.m[10] * r.m[11] + l.m[11] * r.m[15],

    l.m[12] * r.m[0] + l.m[13] * r.m[4] + l.m[14] * r.m[8] + l.m[15] * r.m[12],
    l.m[12] * r.m[1] + l.m[13] * r.m[5] + l.m[14] * r.m[9] + l.m[15] * r.m[13],
    l.m[12] * r.m[2] + l.m[13] * r.m[6] + l.m[14] * r.m[10] + l.m[15] * r.m[14],
    l.m[12] * r.m[3] + l.m[13] * r.m[7] + l.m[14] * r.m[11] + l.m[15] * r.m[15],
  }};
}

inline mat4 mat4_rotate_z(f32 radians) {
  const f32 c = Math::cos(radians);
  const f32 s = Math::sin(radians);
  return mat4{{
    c, -s, 0, 0,
    s,  c, 0, 0,
    0,  0, 1, 0,
    0,  0, 0, 1,
  }};
}

inline mat4 mat4_rotate_y(f32 radians) {
  const f32 c = Math::cos(radians);
  const f32 s = Math::sin(radians);
  return mat4{{
    c, 0, s, 0,
    0, 1, 0, 0,
   -s, 0, c, 0,
    0, 0, 0, 1,
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

inline mat4 mat4_scale(f32 x, f32 y, f32 z) {
  return mat4{{
    x, 0, 0, 0,
    0, y, 0, 0,
    0, 0, z, 0,
    0, 0, 0, 1,
  }};
}

inline mat4 mat4_perspective(f32 fov, f32 front, f32 back, f32 aspect) {
  const f32 top = front * Math::tan(Math::RADIANS(fov * 0.5F));
  const f32 right = top * aspect;
  const f32 left = -right;
  const f32 bottom = -top;

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

} // namespace Themepark
