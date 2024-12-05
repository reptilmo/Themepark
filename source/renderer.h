// renderer.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "dynarray.h"
#include "mat4.h"
#include "hierarchical.h"

namespace Themepark {

class Mesh;
class Image;

enum class ShaderType {
  Vertex = 0,
  TessCtrl,
  TessEval,
  Fragment,
};

class Renderer {
  DISABLE_COPY_AND_MOVE(Renderer);
public:
  Renderer() = default;
  ~Renderer() = default;

  bool startup(DynamicAllocator* allocator);
  void shutdown();

  u32 begin_shader_program(); // returns index
  bool program_add_shader(u32 program_idx, ShaderType type, const DynArray<i8>* shader_text);
  u32 link_shader_program(u32 program_idx); // returns handle

  u32 build_vertex_array(const Mesh* mesh);
  u32 build_texture_2d(const Image* image);
  u32 build_texture_cube(const Image* images);

  void delete_textures();

  void set_depth_range(f32 min_z, f32 max_z);
  void set_clear_color(f32 r, f32 g, f32 b);
  void set_viewport(i32 left, i32 top, i32 width, i32 height);

  void enable_texture_mapping(bool enable);
  void enable_depth_test(bool enable);
  void enable_wireframe_mode(bool enable);

  void begin_frame();
  void end_frame();

  void use_shader_program(u32 program_handle);
  u32 use_texture_2d(u32 texture_handle); // returns texture unit
  u32 use_texture_cube(u32 texture_handle); // returns texture unit

  i32 shader_uniform_location(u32 handle, const char* uniform_name);
  void shader_set_uniform(i32 location, const mat4& m);
  void shader_set_uniform(i32 location, u32 value);
  void shader_set_uniform(i32 location, const vec3* data, u32 count);
  void shader_set_uniform(i32 location, const vec4* data, u32 count);

  void draw_vertex_array(u32 idx);
  void draw_vertex_array_instanced(u32 idx, u32 instances);
  void draw_vertex_array_triangle_patches(u32 idx);
  void draw_vertex_array_triangle_patches_instanced(u32 idx, u32 instances);
  void draw_hierarchical(const HierarchicalModel* model);

protected:
  void draw_hierarchical_impl(
      const DynArray<ModelNode>& nodes,
      const mat4& parent_transform,
      i32 transform_uniform,
      i32 instance_uniform,
      u32 current_node_idx);

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

  struct ShaderProgram {
    u32 program_handle;
    u32 shader_handles[6];
    u32 shader_count;
  };

  u32 active_texture_units = 0;
  DynArray<ShaderProgram> shader_programs;
  DynArray<VertexArray> vertex_arrays;
  DynArray<ActiveTexture> active_textures;
  DynamicAllocator* global_allocator = nullptr;
};

} // namespace Themepark
