// hierarchical.h
// Kostya Leshenko
// CS447P
// Themepark

#pragma once

#include "defines.h"
#include "memory.h"
#include "dynarray.h"
#include "vec3.h"
#include "mat4.h"

#define HIERARCHICAL_MAX_CHILD 9

namespace Themepark {

struct ModelNode {
  u32 vertex_array_idx;
  u32 instances;
  vec3* instance_positions;
  mat4 rotation;
  mat4 translation;
  u32 child_idx[HIERARCHICAL_MAX_CHILD];
  u32 child_count;
};

class HierarchicalModel final {
  DISABLE_COPY_AND_MOVE(HierarchicalModel);
public:
  HierarchicalModel() = default;
  ~HierarchicalModel() = default;

  bool init(DynamicAllocator* allocator);
  void cleanup();

  u32 set_root_node(u32 vertex_array_idx,
      const mat4& rotation,
      const mat4& translation);

  u32 add_child_node(u32 parent_idx,
      u32 vertex_array_idx,
      const mat4& rotation,
      const mat4& translation,
      vec3* instance_positions,
      u32 instances);

  DynArray<ModelNode> hierarchy;
  u32 shader_program;
};

} // namespace Themepark
