#version 430 core
layout (location = 0) in vec3 position; // position
layout (location = 1) in vec3 normal;   // normal
layout (location = 2) in vec2 tex_st;   // texture st

uniform vec3 tent_position[10];
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 position_eye;
out vec3 normal_eye;
out vec2 st;

void main() {
  st = tex_st;
  position_eye = vec3(view * model * vec4(position + tent_position[gl_InstanceID], 1.0));
  normal_eye = vec3(view * model * vec4(normal, 0.0));
  gl_Position = projection * vec4(position_eye, 1.0);
}
