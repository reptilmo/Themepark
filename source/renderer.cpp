// renderer.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "renderer.h"
#include "logging.h"
#include "mesh.h"
#include "image.h"

#include <glad/glad.h>

#define MAX_GL_LOG_LEN 2048

namespace Themepark {
namespace {
constexpr GLenum ShaderGLType(ShaderType type) {
  switch (type) {
    case ShaderType::Vertex:
      return GL_VERTEX_SHADER;
    case ShaderType::TessCtrl:
      return GL_TESS_CONTROL_SHADER;
    case ShaderType::TessEval:
      return GL_TESS_EVALUATION_SHADER;
    case ShaderType::Fragment:
    default:
      return GL_FRAGMENT_SHADER;
  }
}

} // anon namespace


bool Renderer::startup(DynamicAllocator* allocator) {
  ASSERT(allocator != nullptr);
  global_allocator = allocator;
  shader_programs.init(global_allocator, MemoryTag::Renderer);
  vertex_arrays.init(global_allocator, MemoryTag::Renderer);
  active_textures.init(global_allocator, MemoryTag::Renderer);
  return true;
}

void Renderer::shutdown() {
  shader_programs.clear();
  vertex_arrays.clear();
  active_textures.clear();
}

u32 Renderer::begin_shader_program() {
  ShaderProgram program;
  memset(&program, 0, sizeof(ShaderProgram));
  program.program_handle = glCreateProgram();
  shader_programs.push_back(program);
  return shader_programs.size() - 1;
}

bool Renderer::program_add_shader(u32 program_idx, ShaderType type, const DynArray<i8>* shader_text) {
  ASSERT(program_idx >= 0 && program_idx < shader_programs.size());
  ASSERT(shader_text != nullptr && shader_text->size() > 0);
  GLint success = 0;

  const GLuint shader_handle = glCreateShader(ShaderGLType(type));
  const GLchar* shader_data = (const GLchar*)shader_text->data();
  const GLint shader_len = shader_text->size();
  glShaderSource(shader_handle, 1, &shader_data, &shader_len);

  glCompileShader(shader_handle);
  glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info[MAX_GL_LOG_LEN] = {};
    glGetShaderInfoLog(shader_handle, MAX_GL_LOG_LEN, nullptr, info);
    LOG_ERROR("Renderer: failed to compile %d shader!\n%s",  (u32)type, info);
    return false;
  }

  ShaderProgram& program = shader_programs[program_idx];
  program.shader_handles[program.shader_count] = shader_handle;
  program.shader_count++;

  return true;
}

u32 Renderer::link_shader_program(u32 program_idx) {
  ASSERT(program_idx >= 0 && program_idx < shader_programs.size());
  GLint success = 0;
  u32 i = 0;
  GLchar info[MAX_GL_LOG_LEN];

  ShaderProgram& program = shader_programs[program_idx];
  for (i = 0; i < program.shader_count; ++i) {
    glAttachShader(program.program_handle, program.shader_handles[i]);
  }

  glLinkProgram(program.program_handle);
  glGetProgramiv(program.program_handle, GL_LINK_STATUS, &success);
  if (!success) {
    memset(info, 0, sizeof(GLchar) * MAX_GL_LOG_LEN);
    glGetProgramInfoLog(program.program_handle, MAX_GL_LOG_LEN, nullptr, info);
    LOG_ERROR("Renderer: failed to link shader program!\n%s", info);
    return 0;
  }

  for (i = 0; i < program.shader_count; ++i) {
    glDetachShader(program.program_handle, program.shader_handles[i]);
    glDeleteShader(program.shader_handles[i]);
  }

  glValidateProgram(program.program_handle);
  glGetProgramiv(program.program_handle, GL_VALIDATE_STATUS, &success);
  if (!success) {
    memset(info, 0, sizeof(GLchar) * MAX_GL_LOG_LEN);
    glGetProgramInfoLog(program.program_handle, MAX_GL_LOG_LEN, nullptr, info);
    LOG_ERROR("Renderer: shader program failed validation!\n%s", info);
    return 0;
  }

  return program.program_handle;
}

i32 Renderer::shader_uniform_location(u32 handle, const char* uniform_name) {
  GLint loc = glGetUniformLocation(handle, &uniform_name[0]);
  auto error = glGetError();
  ASSERT(error != GL_INVALID_VALUE);
  ASSERT(error != GL_INVALID_OPERATION);
  return loc;
}

void Renderer::shader_set_uniform(i32 location, const mat4& m) {
  glUniformMatrix4fv(location, 1, GL_FALSE, m.m);
}

void Renderer::shader_set_uniform(i32 location, u32 value) {
  glUniform1i(location, value);
}

void Renderer::shader_set_uniform(i32 location, const vec3* data, u32 count) {
  glUniform3fv(location, count, (GLfloat*)data);
}

u32 Renderer::build_texture_2d(const Image* image) {
  u32 texture_id = 0;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height,
      0, GL_BGR, GL_UNSIGNED_BYTE, image->data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  return texture_id;
}

u32 Renderer::build_texture_cube(const Image* images) {
  u32 texture_id = 0;
  glGenTextures(1, &texture_id);

  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB,
      images[0].width, images[0].height, 0, GL_BGRA, GL_UNSIGNED_BYTE, images[0].data);

  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB,
      images[1].width, images[1].height, 0, GL_BGRA, GL_UNSIGNED_BYTE, images[1].data);

  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB,
      images[2].width, images[2].height, 0, GL_BGRA, GL_UNSIGNED_BYTE, images[2].data);

  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB,
      images[3].width, images[3].height, 0, GL_BGRA, GL_UNSIGNED_BYTE, images[3].data);

  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB,
      images[4].width, images[4].height, 0, GL_BGRA, GL_UNSIGNED_BYTE, images[4].data);

  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB,
      images[5].width, images[5].height, 0, GL_BGRA, GL_UNSIGNED_BYTE, images[5].data);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return texture_id;
}

u32 Renderer::build_vertex_array(const Mesh* mesh) {
  VertexArray va = {0};
  
  // Per vertex data
  glGenBuffers(1, &va.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, va.vbo);
  glBufferData(GL_ARRAY_BUFFER,
      mesh->vertices.size() * sizeof(Vertex), 
      mesh->vertices.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &va.vao);
  glBindVertexArray(va.vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)); 
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));  
  
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  va.element_count = (u32)mesh->vertices.size();
  vertex_arrays.push_back(va);
  return (u32)(vertex_arrays.size() - 1);
}

void Renderer::set_clear_color(f32 r, f32 g, f32 b) {
  glClearColor(r, g, b, 1.0F);
}

void Renderer::set_viewport(i32 left, i32 top, i32 width, i32 height) {
  glViewport(left, top, width, height);
}

void Renderer::enable_texture_mapping(bool enable) {
  if (enable) {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP);
  } else {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_CUBE_MAP);
  }
}

void Renderer::enable_depth_test(bool enable) {
  if (enable) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }
}
  
void Renderer::begin_frame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::end_frame() {
  active_textures.reset();
  active_texture_units = 0;
}

void Renderer::use_shader_program(u32 program_handle) {
  glUseProgram(program_handle);
}

u32 Renderer::use_texture_2d(u32 texture_handle) {
  ActiveTexture at{active_texture_units, texture_handle};
  glActiveTexture(GL_TEXTURE0 + at.texture_unit);
  glBindTexture(GL_TEXTURE_2D, at.texture_id);
  active_textures.push_back(at);
  active_texture_units++;
  return at.texture_unit;
}

u32 Renderer::use_texture_cube(u32 texture_handle) {
  ActiveTexture at{active_texture_units, texture_handle};
  glActiveTexture(GL_TEXTURE0 + at.texture_unit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, at.texture_id);
  active_textures.push_back(at);
  active_texture_units++;
  return at.texture_unit;
}

void Renderer::draw_vertex_array(u32 idx) {
  ASSERT(idx < vertex_arrays.size());
  VertexArray va = vertex_arrays[idx];
  glBindVertexArray(va.vao);
  glDrawArrays(GL_TRIANGLES, 0, va.element_count);
  //glDrawArrays(GL_LINE_LOOP, 0, va.element_count); // TODO:
  glBindVertexArray(0);
}

void Renderer::draw_vertex_array_instanced(u32 idx, u32 instances) {
  ASSERT(idx < vertex_arrays.size());
  VertexArray va = vertex_arrays[idx];
  glBindVertexArray(va.vao);
  glDrawArraysInstanced(GL_TRIANGLES, 0, va.element_count, instances);
  glBindVertexArray(0);
}

void Renderer::draw_hierarchical(const HierarchicalModel* model) {
  i32 transform_uniform = shader_uniform_location(model->shader_program, "model");
  i32 instance_uniform = shader_uniform_location(model->shader_program, "instance_position");
  mat4 identity = mat4_identity();
  draw_hierarchical_impl(model->hierarchy, identity, transform_uniform, instance_uniform, 0);
}

void Renderer::draw_hierarchical_impl(
    const DynArray<ModelNode>& nodes,
    const mat4& parent_transform,
    i32 transform_uniform,
    i32 instance_uniform,
    u32 current_node_idx) {

  static const vec3 zero{};

  const ModelNode& node = nodes[current_node_idx];
  mat4 transform = node.rotation * node.translation * parent_transform;
  shader_set_uniform(transform_uniform, transform);
  if (node.instances > 1) {
    shader_set_uniform(instance_uniform, node.instance_positions, node.instances);
    draw_vertex_array_instanced(node.vertex_array_idx, node.instances);
  } else {
    shader_set_uniform(instance_uniform, &zero, 1);
    draw_vertex_array(node.vertex_array_idx);
  }

  for (i8 i = 0; i < node.child_count; ++i) {
    draw_hierarchical_impl(nodes, transform,
        transform_uniform,
        instance_uniform,
        node.child_idx[i]);
  }
}

} // namespace Themepark
