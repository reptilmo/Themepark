// hierarchical.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "hierarchical.h"

namespace Themepark {

bool HierarchicalModel::init(DynamicAllocator* allocator) {
  ASSERT(allocator != NULL);
  hierarchy.init(allocator, MemoryTag::Mesh);
  return true;
}

void HierarchicalModel::cleanup() {
  hierarchy.clear();
}

u32 HierarchicalModel::set_root_node(u32 vertex_array_idx,
    const mat4& rotation,
    const mat4& translation) {

  if (hierarchy.size() > 0) {
    hierarchy[0].vertex_array_idx = vertex_array_idx;
    hierarchy[0].rotation = rotation;
    hierarchy[0].translation = translation;
    hierarchy[0].instances = 0;
    return 0;
  } else {
    ModelNode node;
    memset(&node, 0, sizeof(ModelNode));
    node.vertex_array_idx = vertex_array_idx;
    node.rotation = rotation;
    node.translation = translation;
    hierarchy.push_back(node);
    return (u32)(hierarchy.size() - 1);
  }
}

u32 HierarchicalModel::add_child_node(u32 parent_idx,
    u32 vertex_array_idx,
    const mat4& rotation,
    const mat4& translation,
    vec3* instance_positions,
    u32 instances) {

  ASSERT(hierarchy.size() > parent_idx);
  ModelNode node;
  memset(&node, 0, sizeof(ModelNode));
  node.vertex_array_idx = vertex_array_idx;
  node.instances = instances;
  node.instance_positions = instance_positions;
  node.rotation = rotation;
  node.translation = translation;
  hierarchy.push_back(node);

  ModelNode& parent = hierarchy[parent_idx];
  ASSERT(parent.child_count < HIERARCHICAL_MAX_CHILD);
  parent.child_idx[parent.child_count] = (u32)(hierarchy.size() - 1);
  parent.child_count++;

  return (u32)(hierarchy.size() - 1);
}

} // namespace Themepark
