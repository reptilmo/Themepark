#version 460 core
layout (location = 0) in vec3 in_position; // position
layout (location = 1) in vec3 in_normal;   // normal

out vec3 normal;

void main() {
  normal = normalize(in_position);
  gl_Position = vec4(in_position, 1.0);
}
