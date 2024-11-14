// mesh.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "dynarray.h"
#include "vec3.h"
#include "vec2.h"

namespace Themepark {

struct Triangle {
  u32 v_idx[3];
  u32 uv_idx[3];
  u32 n_idx[3]; 
};

struct Vertex {
  vec3 position;
  vec3 normal;
  vec2 uv;
};

class Mesh final {
  DISABLE_COPY_AND_MOVE(Mesh);
public:
  Mesh(DynamicAllocator* allocator);
  ~Mesh();

  bool load_from_obj(const char* filename);

  DynArray<Vertex> vertices;
  DynArray<vec3> positions;
  DynArray<vec3> normals;
  DynArray<vec2> texture_uvs;
  DynArray<Triangle> triangles;
};

} // namespace Themepark
