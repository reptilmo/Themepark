#version 460 core
layout (location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 projection;

out vec3 texture_coords;

void main() {
  texture_coords = position;
  gl_Position = projection * view * vec4(position, 1.0);
}
