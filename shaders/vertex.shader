#version 430 core
layout (location = 0) in vec3 position; // position
layout (location = 1) in vec3 normal;   // normal
layout (location = 2) in vec2 texture;  // texture uv
out vec3 position_eye;
out vec3 normal_eye;
uniform mat4 model_view;
uniform mat4 projection;

void main() {
  position_eye = vec3(model_view * vec4(position, 1.0));
  normal_eye = vec3(model_view * vec4(normal, 0.0));
  gl_Position = projection * vec4(position_eye, 1.0);
}
