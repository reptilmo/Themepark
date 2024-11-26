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

  //TODO: Improve shader building interface, extend to other types of shaders.
  u32 build_shader_program(const DynArray<i8>* vertex, const DynArray<i8>* fragment);
  u32 build_vertex_array(const Mesh* mesh);
  u32 build_texture_2d(const Image* image);
  u32 build_texture_cube(const Image* images);

  void delete_textures();

  void set_depth_range(f32 min_z, f32 max_z);
  void set_clear_color(f32 r, f32 g, f32 b);
  void set_viewport(i32 left, i32 top, i32 width, i32 height);

  void enable_texture_mapping(bool enable);
  void enable_depth_test(bool enable);

  void begin_frame();
  void end_frame();

  void use_shader_program(u32 program_handle);
  u32 use_texture_2d(u32 texture_handle); // returns texture unit
  u32 use_texture_cube(u32 texture_handle); // returns texture unit

  void draw_vertex_array(u32 idx);
  void draw_vertex_array_instanced(u32 idx, u32 instances);

  i32 shader_uniform_location(u32 handle, const char* uniform_name);
  void shader_set_uniform(i32 location, const mat4& m);
  void shader_set_uniform(i32 location, u32 value);
  void shader_set_uniform(i32 location, const vec3* data, u32 count);

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
  DynArray<VertexArray> vertex_arrays;
  DynArray<ActiveTexture> active_textures;
  DynamicAllocator* global_allocator = nullptr;
};

} // namespace Themepark
