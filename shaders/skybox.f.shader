#version 430 core
in vec3 texture_coords;
uniform samplerCube skybox_texture;
out vec4 frag_color;

void main() {
  frag_color = texture(skybox_texture, texture_coords);
  //frag_color = vec4(0.5, 0.5, 0.9, 1.0);
}
