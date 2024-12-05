#version 460 core
in vec3 position_eye;
in vec3 normal_eye;

out vec4 frag_color;

uniform samplerCube skybox_texture;
uniform mat4 view;

float factor = 1.0/2.4;

void main() {
  mat4 inv = inverse(view);
  vec3 incident_eye = normalize(position_eye);
  vec3 normal = normalize(normal_eye);
  vec3 reflected = reflect(incident_eye, normal);
  vec3 refracted = refract(incident_eye, normal, factor);
  reflected = vec3(inv * vec4(reflected, 0.0));
  refracted = vec3(inv * vec4(refracted, 0.0));
  frag_color = texture(skybox_texture, refracted) * texture(skybox_texture, reflected);
}
