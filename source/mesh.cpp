// mesh.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "mesh.h"
#include "logging.h"

namespace Themepark {

Mesh::Mesh(DynamicAllocator* allocator) {
  positions.startup(allocator, MemoryTag::Mesh);
  normals.startup(allocator, MemoryTag::Mesh);
  texture_uvs.startup(allocator, MemoryTag::Mesh);
  triangles.startup(allocator, MemoryTag::Mesh);
  vertices.startup(allocator, MemoryTag::Mesh);
}

Mesh::~Mesh() {
  vertices.shutdown();
}

bool Mesh::load_from_obj(const char* filename) {

  FILE* file = fopen(filename, "r");
  if (file == nullptr) {
    LOG_ERROR("Failed to open %s!", filename);
    return false;
  }

  char buf[MAX_READ_LEN];

  for (;;) {
    memset(buf, 0, sizeof(buf));
    if (fgets(buf, sizeof(buf), file) == nullptr) {
      break;
    }

    if (strncmp(buf, "v ", 2) == 0) {
      vec3 p;

      if (sscanf(buf, "v %f %f %f\n", &p.x, &p.y, &p.z) == 3) {
        positions.push_back(p);
      } else {
        LOG_ERROR("OBJ loader: failed to parse \"%s\"", buf);
      }

    } else if (strncmp(buf, "vn ", 3) == 0) {
      vec3 n;

      if (sscanf(buf, "vn %f %f %f\n", &n.x, &n.y, &n.z) == 3) {
        normals.push_back(n);
      } else {
        LOG_ERROR("OBJ loader: failed to parse \"%s\"", buf);
      }

    } else if (strncmp(buf, "vt ", 3) == 0) {
      vec2 uv;

      if (sscanf(buf, "vt %f %f\n", &uv.u, &uv.v) == 2) {
        texture_uvs.push_back(uv);
      } else {
        LOG_ERROR("OBJ loader: failed to parse \"%s\"", buf);
      }

    } else if (strncmp(buf, "f ", 2) == 0) {
      u32 v[3], uv[3], n[3];

      if (sscanf(buf, "f %i/%i/%i\t%i/%i/%i\t%i/%i/%i\n",
                 &v[0], &uv[0], &n[0],
                 &v[1], &uv[1], &n[1],
                 &v[2], &uv[2], &n[2]) == 9) {

        Triangle tri;
        tri.v_idx[0] = v[0] - 1;
        tri.v_idx[1] = v[1] - 1;
        tri.v_idx[2] = v[2] - 1;
        tri.uv_idx[0] = uv[0] - 1;
        tri.uv_idx[1] = uv[1] - 1;
        tri.uv_idx[2] = uv[2] - 1;
        tri.n_idx[0] = n[0] - 1;
        tri.n_idx[1] = n[1] - 1;
        tri.n_idx[2] = n[2] - 1;

        triangles.push_back(tri);
      } else {
        LOG_ERROR("OBJ loader: failed to parse \"%s\"", buf);
      }
    }
  }

  u64 triangle_count = triangles.size();
  for (u64 i = 0; i < triangle_count; ++i) {
    Vertex a = {
      positions[triangles[i].v_idx[0]],
      normals[triangles[i].n_idx[0]],
      texture_uvs[triangles[i].uv_idx[0]],
    };

    Vertex b = {
      positions[triangles[i].v_idx[1]],
      normals[triangles[i].n_idx[1]],
      texture_uvs[triangles[i].uv_idx[1]],
    };

    Vertex c = {
      positions[triangles[i].v_idx[2]],
      normals[triangles[i].n_idx[2]],
      texture_uvs[triangles[i].uv_idx[2]],
    };

    vertices.push_back(a);
    vertices.push_back(b);
    vertices.push_back(c);
  }

  positions.shutdown();
  normals.shutdown();
  texture_uvs.shutdown();
  triangles.shutdown();

  fclose(file);
  return true;
}

} // namespace Themepark
