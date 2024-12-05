#version 460 core

layout (vertices = 3) out;

in vec3 normal[];

out vec3 norm[];

uniform int tess_level = 0;

void main() {
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  norm[gl_InvocationID] = normal[gl_InvocationID];

  gl_TessLevelInner[0] = tess_level;
  gl_TessLevelOuter[0] = tess_level + 1;
  gl_TessLevelOuter[1] = tess_level + 1;
  gl_TessLevelOuter[2] = tess_level + 1;
}

