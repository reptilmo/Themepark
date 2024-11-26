#version 430 core
in vec3 position_eye;
in vec3 normal_eye;
in vec2 st;

out vec4 frag_color;

uniform sampler2D first_texture;
uniform sampler2D second_texture;

uniform mat4 view; //TODO
vec3 light_position = vec3(-10.0, 20.0, -10.0); //TODO

// Light colors
vec3 La = vec3(0.9, 0.9, 0.9); // Ambient
vec3 Ld = vec3(0.9, 0.9, 0.9); // Diffuse
vec3 Ls = vec3(1.0, 1.0, 1.0); // Specular

// Surface reflectance
vec3 Ka = vec3(0.5, 0.5, 0.5); // Ambient light reflectance
vec3 Kd = vec3(1.0, 0.5, 0.0); // Orange diffuse surface reflectance
vec3 Ks = vec3(1.0, 1.0, 1.0); // Fully reflect specular light

const float specular_power = 100.0;

void main() {
  // Ambient intensity
  vec3 Ia = La * Ka * vec3(float(gl_FrontFacing), float(gl_FrontFacing), float(gl_FrontFacing));
  // Diffuse intensity
  vec3 light_position_eye = vec3(view * vec4(light_position, 1.0));
  vec3 direction_to_light_eye = normalize(light_position_eye - position_eye);
  float d = max(dot(direction_to_light_eye, normal_eye), 0.0);
  vec3 Id = Ld * Kd * d * vec3(float(gl_FrontFacing), float(gl_FrontFacing), float(gl_FrontFacing));
  // Specular intencity
  vec3 reflection_eye = reflect(-direction_to_light_eye, normal_eye);
  vec3 surface_to_viewer = normalize(-position_eye);
  d = max(dot(reflection_eye, surface_to_viewer), 0.0);
  //vec3 Is = Ls * Ks * pow(d, specular_power);
  vec3 Is = vec3(0.0, 0.0, 0.0);
  frag_color = vec4(Is + Id + Ia, 1.0) * texture(first_texture, st) * texture(second_texture, st);
}
