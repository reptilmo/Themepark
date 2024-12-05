#version 460 core

layout (triangles, equal_spacing, ccw) in;

in vec3 norm[];

out vec3 position_eye;
out vec3 normal_eye;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;
  float w = gl_TessCoord.z;

  vec3 position = normalize(vec3(u * gl_in[0].gl_Position + v * gl_in[1].gl_Position + w * gl_in[2].gl_Position));
  vec3 normal = normalize(u * norm[0] + v * norm[1] + w * norm[2]);

  position_eye = vec3(view * model * vec4(position, 1.0));
  normal_eye = vec3(view * model * vec4(normal, 0.0));

  gl_Position = projection * vec4(position_eye, 1.0);
}
