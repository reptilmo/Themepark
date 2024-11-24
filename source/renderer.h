// renderer.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "dynarray.h"
#include "mat4.h"

namespace Themepark {

class Mesh;
class Image;

class Renderer {
  DISABLE_COPY_AND_MOVE(Renderer);
public:
  Renderer() = default;
  ~Renderer() = default;

  bool startup(DynamicAllocator* allocator);
  void shutdown();

  u32 build_shader_program(const DynArray<i8>* vertex, const DynArray<i8>* fragment);
  u32 build_vertex_array(const Mesh* mesh);
  u32 build_texture_2d(const Image* image);

  void delete_textures();

  void set_depth_range(f32 min_z, f32 max_z);
  void set_clear_color(f32 r, f32 g, f32 b);
  void set_viewport(i32 left, i32 top, i32 width, i32 height);

  void enable_texture_mapping(bool enable);
  void enable_depth_test(bool enable);

  void begin_frame();
  void end_frame();

  void use_shader_program(u32 program_handle);
  void use_texture_2d(u32 texture_handle);
  void render_vertex_array(u32 idx);

  i32 shader_uniform_location(u32 handle, const char* uniform_name);
  void shader_set_uniform(i32 location, const mat4& m);
  void shader_set_uniform(i32 location, u32 value);

protected:
  struct VertexArray {
    u32 vao;
    u32 vbo;
    u32 element_count;
  };

  struct ActiveTexture {
    u32 texture_unit;
    u32 texture_id;
  };

  u32 active_texture_units = 0;

  DynamicAllocator* global_allocator = nullptr;
  DynArray<VertexArray> vertex_arrays;
  DynArray<ActiveTexture> active_textures;
};

} // namespace Themepark
