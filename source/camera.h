// camera.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "vec3.h"
#include "vec2.h"
#include "mat4.h"

namespace Themepark {

class Input;

class Camera final {
  DISABLE_COPY_AND_MOVE(Camera);
public:
  Camera() = default;
  ~Camera() = default;

  void startup(const vec3& pos, const vec3& up, f32 yaw, f32 pitch);
  void shutdown();

  mat4 view_matrix(Input* input, f32 delta_time);

private:
  vec3 position{};
  vec3 world_up{};
  f32 vertical_angle{};
  f32 horizontal_angle{};
};

} // namespace Themepark