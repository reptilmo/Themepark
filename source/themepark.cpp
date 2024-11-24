// themepark.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "defines.h"
#include "system.h"
#include "logging.h"
#include "memory.h"
#include "mesh.h"
#include "mat4.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "image.h"

#include <glad/glad.h>

namespace Themepark {

u32 va_platform = 0;
u32 va_cube = 0;
u32 shader_program = 0;
u32 platform_texture = 0;
u32 ground_texture = 0;

DynamicAllocator allocator;
Renderer renderer;
Camera camera;

bool themepark_startup(u32 view_width, u32 view_height) {
  if (!allocator.startup(MiB(200))) {
    return false;
  }

  if (!renderer.startup(&allocator)) {
    return false;
  }

  Image image(&allocator);
  if (!image.load_tga_file(system_base_dir("assets/platform2.tga"))) {
    return false;
  }

  Image image2(&allocator);
  if (!image2.load_tga_file(system_base_dir("assets/ground.tga"))) {
    return false;
  }

  Mesh platform(&allocator);
  if (!platform.load_from_obj(system_base_dir("assets/platform.obj"))) {
    return false;
  }

  Mesh cube(&allocator);
  if (!cube.load_from_obj(system_base_dir("assets/monkey.obj"))) {
    return false;
  }

  DynArray<i8> vert_shader;
  vert_shader.startup(&allocator, MemoryTag::Shader);
  if (!read_file(&vert_shader, system_base_dir("shaders/vertex.shader"))) {
    return false;
  }

  DynArray<i8> frag_shader;
  frag_shader.startup(&allocator, MemoryTag::Shader);
  if (!read_file(&frag_shader, system_base_dir("shaders/fragment.shader"))) {
    return false;
  }

  platform_texture = renderer.build_texture_2d(&image);
  ground_texture = renderer.build_texture_2d(&image2);
  shader_program = renderer.build_shader_program(&vert_shader, &frag_shader);

  vert_shader.shutdown();
  frag_shader.shutdown();
  if (shader_program <= 0) {
    return false;
  }

  camera.startup(vec3{0.0F, 10.0F, 5.0F}, vec3(0.0F, 1.0F, 0.0F), -90, 0);

  va_platform = renderer.build_vertex_array(&platform);
  renderer.use_texture_2d(ground_texture);
  va_cube = renderer.build_vertex_array(&cube);

  renderer.set_clear_color(0.0F, 0.2F, 0.5F);
  renderer.set_viewport(0, 0, view_width, view_height);
  renderer.enable_depth_test(true);

  return true;
}

void themepark_run(RunContext* context) {
  mat4 model = mat4_translate(0, 0, 0);
  mat4 view = camera.view_matrix(context->input, context->delta_time);
  mat4 projection = mat4_perspective(45.0F, 0.1F, 1000.0F, context->width / context->height);

  renderer.begin_frame();
  renderer.use_shader_program(shader_program);

  renderer.shader_set_uniform(
      renderer.shader_uniform_location(shader_program, "model"),
      model);
  renderer.shader_set_uniform(
      renderer.shader_uniform_location(shader_program, "view"),
      view);
  renderer.shader_set_uniform(
      renderer.shader_uniform_location(shader_program, "projection"), projection);

  renderer.use_texture_2d(platform_texture);
  renderer.shader_set_uniform(renderer.shader_uniform_location(shader_program, "first_texture"), 0);

  renderer.use_texture_2d(ground_texture);
  renderer.shader_set_uniform(renderer.shader_uniform_location(shader_program, "second_texture"), 1);

  renderer.render_vertex_array(va_platform);
  model = mat4_translate(0, 5, 5);

  renderer.shader_set_uniform(
      renderer.shader_uniform_location(shader_program, "model"),
      model);

  renderer.render_vertex_array(va_cube);
  renderer.end_frame();
}

void themepark_shutdown() {
  renderer.shutdown();
  allocator.shutdown();
}

} // namespace Themepark
