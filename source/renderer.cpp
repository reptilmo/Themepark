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

bool Renderer::startup(DynamicAllocator* allocator) {
  ASSERT(allocator != nullptr);
  global_allocator = allocator;
  vertex_arrays.startup(global_allocator, MemoryTag::Renderer);  
  return true;
}

void Renderer::shutdown() {
  vertex_arrays.shutdown();
}

u32 Renderer::build_shader_program(const DynArray<i8>* vertex, const DynArray<i8>* fragment) {
  ASSERT(vertex != nullptr && vertex->size() > 0);
  ASSERT(fragment != nullptr && fragment->size() > 0);

  u32 program_handle = glCreateProgram();
  ASSERT(program_handle > 0);
  GLint success = 0;

  const GLuint vertex_handle = glCreateShader(GL_VERTEX_SHADER);
  const GLchar* vert_data = (const GLchar*)vertex->data();
  const GLint vert_len = vertex->size();
  glShaderSource(vertex_handle, 1, &vert_data, &vert_len);

  glCompileShader(vertex_handle);
  glGetShaderiv(vertex_handle, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info[MAX_GL_LOG_LEN] = {};
    glGetShaderInfoLog(vertex_handle, MAX_GL_LOG_LEN, nullptr, info);
    LOG_ERROR("Renderer: failed to compile vertex shader!\n%s", info);
    return 0;
  }

  const GLuint fragment_handle = glCreateShader(GL_FRAGMENT_SHADER);
  const GLchar* frag_data = (const GLchar*)fragment->data();
  const GLint frag_len = fragment->size();
  glShaderSource(fragment_handle, 1, &frag_data, &frag_len);

  glCompileShader(fragment_handle);
  glGetShaderiv(fragment_handle, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info[MAX_GL_LOG_LEN] = {};
    glGetShaderInfoLog(fragment_handle, MAX_GL_LOG_LEN, nullptr, info);
    LOG_ERROR("Renderer: failed to compile fragment shader!\n%s", info);
    return 0;
  }

  glAttachShader(program_handle, vertex_handle);
  glAttachShader(program_handle, fragment_handle);
  glLinkProgram(program_handle);
  glGetProgramiv(program_handle, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar info[MAX_GL_LOG_LEN];
    glGetProgramInfoLog(program_handle, MAX_GL_LOG_LEN, nullptr, info);
    LOG_ERROR("Renderer: failed to link shader program!\n%s", info);
    return 0;
  }

  glDetachShader(program_handle, vertex_handle);
  glDeleteShader(vertex_handle);
  glDetachShader(program_handle, fragment_handle);
  glDeleteShader(fragment_handle);

  glValidateProgram(program_handle);
  glGetProgramiv(program_handle, GL_VALIDATE_STATUS, &success);
  if (!success) {
    GLchar info[MAX_GL_LOG_LEN];
    glGetProgramInfoLog(program_handle, MAX_GL_LOG_LEN, nullptr, info);
    LOG_ERROR("Renderer: shader program failed validation!\n%s", info);
    return 0;
  }

  return program_handle;
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

u32 Renderer::build_texture_2d(const Image* image) {
  u32 texture_id = 0;

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height,
      0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
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
  } else {
    glDisable(GL_TEXTURE_2D);
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
  active_texture_units = 0;
}

void Renderer::use_shader_program(u32 program_handle) {
  glUseProgram(program_handle);
}

void Renderer::use_texture_2d(u32 texture_handle) {
  glActiveTexture(GL_TEXTURE0 + active_texture_units);
  glBindTexture(GL_TEXTURE_2D, texture_handle);
  active_texture_units++;
}

void Renderer::render_vertex_array(u32 idx) {
  ASSERT(idx < vertex_arrays.size());
  VertexArray va = vertex_arrays[idx];
  glBindVertexArray(va.vao);
  glDrawArrays(GL_TRIANGLES, 0, va.element_count);
  //glDrawArrays(GL_LINE_LOOP, 0, va.element_count); // TODO:
  glBindVertexArray(0);
}

} // namespace Themepark
