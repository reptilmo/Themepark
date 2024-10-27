// themepark.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "defines.h"
#include "logging.h"
#include "memory.h"
#include "vec3.h"

#include <glad/glad.h>

namespace Themepark {

const char* vertex_shader = 
"#version 400\n"
"in vec3 vp;\n"
"void main() {\n"
"   gl_Position = vec4(vp, 1.0);\n"
"}\n";

const char* fragment_shader = 
"#version 400\n"
"out vec4 frag_color;\n"
"void main() {\n"
"   frag_color = vec4(0.5, 0.0, 0.5, 1.0);\n"
"}\n";


u32 vao = 0;
u32 shader_program = 0;

DynamicAllocator allocator;


bool themepark_startup() {
  if (!allocator.startup(MiB(2))) {
    return false;
  }

  vec3* triangle = (vec3*) allocator.allocate(sizeof(vec3) * 3, MemoryTag::Mesh);
  if (triangle == nullptr) {
    return false;
  }

  triangle[0].set(0.0f, 0.5f, 0.0f);
  triangle[1].set(0.5f, -0.5f, 0.0f);
  triangle[2].set(-0.5f, -0.5f, 0.0f);

  u32 vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), triangle, GL_STATIC_DRAW);

  allocator.free(triangle, sizeof(vec3) * 3, MemoryTag::Mesh);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindVertexArray(0);

  u32 vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, nullptr);
  glCompileShader(vs);

  u32 fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, nullptr);
  glCompileShader(fs);

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vs);
  glAttachShader(shader_program, fs);
  glLinkProgram(shader_program);

  glClearColor(0.5f, 0.3f, 0.3f, 1.0f);

  return true;
}

void themepark_run(void* param) {
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(shader_program);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
}

void themepark_shutdown() {
  allocator.shutdown();
}

} // namespace Themepark
