// camera.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "camera.h"
#include "input.h"

#define VIEW_FACTOR 0.5F
#define MOVE_FACTOR 0.5F

namespace Themepark {

using namespace Math;

void Camera::startup(const vec3& pos, const vec3& up, f32 yaw, f32 pitch) {
  position = pos;
  world_up = up;
  horizontal_angle = yaw;
  vertical_angle = pitch;
}


void Camera::shutdown() {

}


mat4 Camera::view_matrix(Input* input, f32 delta_time) {
  
  const f32 pitch_delta = VIEW_FACTOR * input->mouse_delta_y() * delta_time;
  const f32 yaw_delta = VIEW_FACTOR * input->mouse_delta_x() * delta_time;
  const f32 move_factor = MOVE_FACTOR * delta_time;

  vertical_angle -= pitch_delta;
  horizontal_angle += yaw_delta;

  const f32 cos_pitch = cos(RADIANS(vertical_angle));
  const f32 sin_pitch = sin(RADIANS(vertical_angle));
  const f32 cos_yaw = cos(RADIANS(horizontal_angle));
  const f32 sin_yaw = sin(RADIANS(horizontal_angle));
  
  vec3 front{cos_yaw * cos_pitch, sin_pitch, sin_yaw * cos_pitch};
  front.normalize(); 
  vec3 right = cross(front, world_up);
  vec3 up = cross(right, front);

  const mat4 rotation{{
    right.x, up.x, -front.x, 0,
    right.y, up.y, -front.y, 0,
    right.z, up.z, -front.z, 0,
    0,       0,    0,        1,
  }};

  if (input->move_forward()) {
    position += (front * move_factor);
  }

  if (input->move_back()) {
    position -= (front * move_factor);
  }

  if (input->move_right()) {
    position += (right * move_factor);
  }

  if (input->move_left()) {
    position -= (right * move_factor);
  }

  return mat4_translate(-position.x, -position.y, -position.z) * rotation;
}

} // namespace Themepark
