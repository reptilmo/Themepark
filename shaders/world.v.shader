#version 460 core
layout (location = 0) in vec3 position; // position
layout (location = 1) in vec3 normal;   // normal
layout (location = 2) in vec2 tex_st;   // texture st

uniform vec4 instance_data[20];
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 position_eye;
out vec3 normal_eye;
out vec2 st;

mat4 translate(float x, float y, float z) {
  return mat4(1.0, 0.0, 0.0, 0.0,
              0.0, 1.0, 0.0, 0.0,
              0.0, 0.0, 1.0, 0.0,
              x,   y,   z,   1.0);
}

mat4 rotate_y(float deg) {
  const float c = cos(radians(deg));
  const float s = sin(radians(deg));
  return mat4(c,   0.0, s,   0.0,
              0.0, 1.0, 0.0, 0.0,
              -s,  0.0, c,   0.0,
              0.0, 0.0, 0.0, 1.0);
}

void main() {
  mat4 translation = translate(
    instance_data[gl_InstanceID].x,
    instance_data[gl_InstanceID].y,
    instance_data[gl_InstanceID].z);

  mat4 rotation = rotate_y(instance_data[gl_InstanceID].w);

  st = tex_st;

  position_eye = vec3(view * translation * rotation * model * vec4(position, 1.0));
  normal_eye = vec3(view * model * vec4(normal, 0.0));
  gl_Position = projection * vec4(position_eye, 1.0);
}
