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

#include <glad/glad.h>

namespace Themepark {

u32 va_idx = 0;
u32 shader_program = 0;

DynamicAllocator allocator;
Renderer renderer;

bool themepark_startup() {
  if (!allocator.startup(MiB(5))) {
    return false;
  }

  if (!renderer.startup(&allocator)) {
    return false;
  }

  Mesh mesh(&allocator);
  if (!mesh.load_from_obj(system_base_dir("assets/untitled.obj"))) {
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

  shader_program = renderer.build_shader_program(&vert_shader, &frag_shader);
  vert_shader.shutdown();
  frag_shader.shutdown();
  if (shader_program <= 0) {
    return false;
  }

  va_idx = renderer.build_vertex_array(&mesh);
  renderer.set_clear_color(0.5F, 0.3F, 0.3F);
  renderer.set_viewport(0, 0, 800, 600); // TODO:

  return true;
}

void themepark_run(void* param) {
  renderer.begin_frame();
  renderer.use_shader_program(shader_program);

  mat4 model = mat4_translate(0, 0, -10.0F);
  mat4 view = mat4_identity();
  mat4 projection = mat4_perspective(45.0F, 1.0F, 100.0F, 800/600); //TODO:

  renderer.shader_set_uniform(
      renderer.shader_uniform_location(shader_program, "M"), model);
  renderer.shader_set_uniform(
      renderer.shader_uniform_location(shader_program, "V"), view);
  renderer.shader_set_uniform(
      renderer.shader_uniform_location(shader_program, "P"), projection);

  renderer.render_vertex_array(va_idx);
  renderer.end_frame();
}

void themepark_shutdown() {
  renderer.shutdown();
  allocator.shutdown();
}

} // namespace Themepark
