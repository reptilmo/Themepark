// renderer.h
// Kostya Leshenko
// CS447P

#pragma once

#include "defines.h"

namespace Themepark {

class Renderer {

public:
  Renderer() = default;
  ~Renderer() = default;

  b8 startup();
  void shutdown();

  u32 build_shader_program(const DynamicBuffer<u8>* vertex, const DynamicBuffer<u8>* fragment);
  u32 build_vertex_array(const Mesh* mesh);

  void begin_frame();
  void end_frame();

  void use_shader_program(u32 program_handle);
  void use_texture(u32 texture_handle);
  void render_vertex_array(u32 vertex_array_handle);




protected:
};

} // namespace Themepark
