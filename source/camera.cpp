// camera.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "camera.h"
#include "input.h"

#define VIEW_FACTOR 10.0F
#define MOVE_FACTOR 10.0F

namespace Themepark {

using namespace Math;

void Camera::startup(const vec3& pos, const vec3& up, f32 yaw, f32 pitch) {
  position = pos;
  world_up = up;
  horizontal_angle = yaw;
  vertical_angle = pitch;
}

void Camera::shutdown() {}

void Camera::update_view_matrices(CameraMatrixBlock* block, Input* input, f32 delta_time) {
  
  const f32 pitch_delta = VIEW_FACTOR * input->mouse_delta_y() * delta_time;
  const f32 yaw_delta = VIEW_FACTOR * input->mouse_delta_x() * delta_time;
  const f32 move_factor = MOVE_FACTOR * delta_time;

  horizontal_angle += yaw_delta;
  vertical_angle -= pitch_delta;
  if (vertical_angle < -89.0F) {
    vertical_angle = -89.0F;
  } else if (vertical_angle > 89.0F) {
    vertical_angle = 89.0F;
  }

  const f32 cos_pitch = cos(RADIANS(vertical_angle));
  const f32 sin_pitch = sin(RADIANS(vertical_angle));
  const f32 cos_yaw = cos(RADIANS(horizontal_angle));
  const f32 sin_yaw = sin(RADIANS(horizontal_angle));
  
  vec3 forward{cos_yaw * cos_pitch, sin_pitch, sin_yaw * cos_pitch};
  forward *= -1.0F;
  forward.normalize();
  vec3 left = cross(world_up, forward);
  left.normalize();
  vec3 up = cross(forward, left);

  block->rotation.m[0] = left.x;
  block->rotation.m[1] = up.x;
  block->rotation.m[2] = forward.x;
  block->rotation.m[4] = left.y;
  block->rotation.m[5] = up.y;
  block->rotation.m[6] = forward.y;
  block->rotation.m[8] = left.z;
  block->rotation.m[9] = up.z;
  block->rotation.m[10] = forward.z;
  block->rotation.m[15] = 1.0F;

  if (input->move_forward()) {
    position -= (forward * move_factor);
  }

  if (input->move_back()) {
    position += (forward * move_factor);
  }

  if (input->move_right()) {
    position += (left * move_factor);
  }

  if (input->move_left()) {
    position -= (left * move_factor);
  }

  if (input->move_up()) {
    position.y += (move_factor);
  }

  if (input->move_down()) {
    position.y -= (move_factor);
  }

  block->view = mat4_translate(-position.x, -position.y, -position.z) * block->rotation;
}

} // namespace Themepark
