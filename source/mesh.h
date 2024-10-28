// mesh.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "vec3.h"

namespace Themepark {

typedef struct Vertex {
  vec3 pos;
} Vertex;

typedef struct Mesh {
  Vertex* vertices;
} Mesh;

} // namespace Themepark
