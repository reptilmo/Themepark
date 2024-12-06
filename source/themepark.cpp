// themepark.cpp
// Kostya Leshenko
// CS447P
// Themepark

#include "defines.h"
#include "system.h"
#include "logging.h"
#include "memory.h"
#include "mesh.h"
#include "mat4.h"
#include "vec4.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "image.h"
#include "hierarchical.h"

#define TESSELLATION_MAX 15

namespace Themepark {

u32 va_platform = 0;
u32 va_tent = 0;
u32 va_skybox = 0;
u32 va_octahedron = 0;
u32 world_program = 0;
u32 platform_texture = 0;
u32 ground_texture = 0;
u32 skybox_texture = 0;
u32 skybox_program = 0;
u32 balloon_program = 0;
u32 tent_color = 0;
u32 tent_texture = 0;
u32 ferris_color = 0;

bool wireframe = false;
f32 wheel_rotation_angle = 0.0F;
i32 tess_level = 0;
i32 tess_step = 1;

DynamicAllocator allocator;
Renderer renderer;
Camera camera;
CameraMatrixBlock camera_block;
DynArray<vec4> tent_data;
HierarchicalModel ferris_wheel;

bool load_skybox_images(Image* images, DynamicAllocator* allocator);
void free_skybox_images(Image* images, DynamicAllocator* allocator);
bool load_vec4_file(DynArray<vec4>* data, const char* filename);
bool build_shader_programs();
bool build_mesh_vertex_arrays(); //TODO:
bool build_texture_objects();    //TODO:
bool build_ferris_wheel();

bool themepark_startup(u32 view_width, u32 view_height) {
  if (!allocator.startup(MiB(50))) {
    return false;
  }

  if (!renderer.startup(&allocator)) {
    return false;
  }

  Mesh platform(&allocator);
  if (!platform.load_from_obj(system_base_dir("assets/platform.obj"))) {
    return false;
  }

  Mesh tent(&allocator);
  if (!tent.load_from_obj(system_base_dir("assets/tent.obj"))) {
    return false;
  }

  Mesh skybox(&allocator);
  if (!skybox.load_from_obj(system_base_dir("assets/cube.obj"))) {
    return false;
  }

  Mesh octahedron(&allocator);
  if (!octahedron.load_from_obj(system_base_dir("assets/octahedron.obj"))) {
    return false;
  }

  tent_data.init(&allocator, MemoryTag::Mesh);
  if (!load_vec4_file(&tent_data, system_base_dir("assets/tent.map"))) {
    return false;
  }

  if (!build_shader_programs()) {
    return false;
  }

  if (!build_texture_objects()) {
    return false;

  }

  if (!build_ferris_wheel()) {
    return false;
  }

  va_skybox = renderer.build_vertex_array(&skybox);
  va_platform = renderer.build_vertex_array(&platform);
  va_tent = renderer.build_vertex_array(&tent);
  va_octahedron = renderer.build_vertex_array(&octahedron);

  camera.startup(vec3{0.0F, 10.0F, 5.0F}, vec3(0.0F, 1.0F, 0.0F), -90, 0);
  renderer.set_clear_color(0.0F, 0.2F, 0.5F);
  renderer.set_viewport(0, 0, view_width, view_height);
  renderer.enable_depth_test(true);

  return true;
}

void themepark_run(RunContext* context) {
  static const vec4 zero(0, 0, 0, 0);

  if (context->input->w_key_pressed() && !context->input->w_key_was_pressed()) {
    wireframe = !wireframe;
    renderer.enable_wireframe_mode(wireframe);
  }

  if (context->input->s_key_pressed() && !context->input->s_key_was_pressed()) {
    if (tess_level <= 0) {
      tess_step = 1;
    } else if (tess_level >= TESSELLATION_MAX) {
      tess_step = -1;
    }
    tess_level = tess_level + tess_step;
  }

  mat4 model = mat4_translate(0, 0, 0);
  memset(&camera_block, 0, sizeof(CameraMatrixBlock));
  camera.update_view_matrices(&camera_block, context->input, context->delta_time);
  mat4 projection = mat4_perspective(45.0F, 0.1F, 1000.0F, f32(context->width) / f32(context->height));

  renderer.begin_frame();
  glDepthMask(GL_FALSE); //TODO:
  //glFrontFace(GL_CW);    //TODO:
  renderer.use_shader_program(skybox_program);
  renderer.shader_set_uniform(
      renderer.shader_uniform_location(skybox_program, "view"), camera_block.rotation);
  renderer.shader_set_uniform(
      renderer.shader_uniform_location(skybox_program, "projection"), projection);
  renderer.shader_set_uniform(renderer.shader_uniform_location(skybox_program, "skybox_texture"),
      renderer.use_texture_cube(skybox_texture));

  renderer.draw_vertex_array(va_skybox);
  //glFrontFace(GL_CCW);  //TODO:
  glDepthMask(GL_TRUE); //TODO:

  renderer.use_shader_program(world_program);
  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "instance_data"), &zero, 1);

  model = mat4_scale(0.5F, 1.0F, 0.5F);
  renderer.shader_set_uniform(
      renderer.shader_uniform_location(world_program, "model"), model);
  renderer.shader_set_uniform(
      renderer.shader_uniform_location(world_program, "view"), camera_block.view);
  renderer.shader_set_uniform(
      renderer.shader_uniform_location(world_program, "projection"), projection);

  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "first_texture"),
      renderer.use_texture_2d(platform_texture));
  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "second_texture"),
      renderer.use_texture_2d(ground_texture));
  renderer.draw_vertex_array(va_platform);

  wheel_rotation_angle += (10.0F * context->delta_time);
  ferris_wheel.shader_program = world_program;

  ferris_wheel.hierarchy[1].rotation = mat4_rotate_z(Math::RADIANS(wheel_rotation_angle));
  const ModelNode& wheel = ferris_wheel.hierarchy[1];
  for (i8 i = 0; i < wheel.child_count; ++i) {
    u32 idx = wheel.child_idx[i];
    ferris_wheel.hierarchy[idx].rotation = mat4_rotate_z(Math::RADIANS(-wheel_rotation_angle));
  }

  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "first_texture"),
      renderer.use_texture_2d(ferris_color));
  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "second_texture"),
      renderer.use_texture_2d(ferris_color));

  ferris_wheel.hierarchy[0].rotation = mat4_identity();
  ferris_wheel.hierarchy[0].translation = mat4_translate(-6, 11.45F, -39);
  renderer.draw_hierarchical(&ferris_wheel);

  ferris_wheel.hierarchy[0].rotation = mat4_rotate_y(Math::RADIANS(90));
  ferris_wheel.hierarchy[0].translation = mat4_translate(-59.7, 11.45F, 43.9);
  renderer.draw_hierarchical(&ferris_wheel);

  model = mat4_scale(2.5F, 2.5F, 2.5F);
  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "model"), model);
  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "instance_data"),
      tent_data.data(), tent_data.size());
  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "first_texture"),
      renderer.use_texture_2d(tent_texture));
  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "second_texture"),
      renderer.use_texture_2d(tent_texture));

  renderer.draw_vertex_array_instanced(va_tent, tent_data.size());

  renderer.use_shader_program(balloon_program);
  renderer.shader_set_uniform(renderer.shader_uniform_location(balloon_program, "tess_level"), tess_level);
  renderer.shader_set_uniform(renderer.shader_uniform_location(balloon_program, "view"), camera_block.view);
  renderer.shader_set_uniform(renderer.shader_uniform_location(balloon_program, "projection"), projection);
  renderer.shader_set_uniform(renderer.shader_uniform_location(balloon_program, "skybox_texture"),
      renderer.use_texture_cube(skybox_texture));

  //renderer.draw_vertex_array_triangle_patches_instanced(va_octahedron, tent_data.size()); //TODO: Doesn't work correctly :/
  f32 wind_x = 0.9F * Math::sin(Math::RADIANS(wheel_rotation_angle));
  f32 wind_y = 0.5F * Math::cos(Math::RADIANS(wheel_rotation_angle));
  f32 wind_z = 0.7F * Math::sin(Math::RADIANS(wheel_rotation_angle));

  for (u32 i = 0; i < tent_data.size(); ++i) {
    model = mat4_translate(tent_data[i].x + wind_x, tent_data[i].y + 9.0 + wind_y, tent_data[i].z + wind_z);
    renderer.shader_set_uniform(renderer.shader_uniform_location(balloon_program, "model"), model);
    renderer.draw_vertex_array_triangle_patches(va_octahedron);
  }

  renderer.end_frame();
}

void themepark_shutdown() {
  tent_data.clear();
  renderer.shutdown();
  allocator.shutdown();
}

bool build_ferris_wheel() {
  mat4 rotation = mat4_identity();
  mat4 translation = mat4_identity();

  ferris_wheel.init(&allocator);

  Mesh base(&allocator);
  if (!base.load_from_obj(system_base_dir("assets/base.obj"))) {
    return false;
  }

  u32 va = renderer.build_vertex_array(&base);
  u32 parent = ferris_wheel.set_root_node(va, rotation, translation);

  Mesh wheel(&allocator);
  if (!wheel.load_from_obj(system_base_dir("assets/wheel.obj"))) {
    return false;
  }

  va = renderer.build_vertex_array(&wheel);
  parent = ferris_wheel.add_child_node(parent, va, rotation, translation, nullptr, 0);

  Mesh basket(&allocator);
  if (!basket.load_from_obj(system_base_dir("assets/basket.obj"))) {
    return false;
  }

  DynArray<vec4> basket_positions;
  basket_positions.init(&allocator, MemoryTag::Mesh);
  if (!load_vec4_file(&basket_positions, system_base_dir("assets/basket.map"))) {
    return false;
  }

  va = renderer.build_vertex_array(&basket);
  for (u64 i = 0; i < basket_positions.size(); ++i) {
    const vec4& p = basket_positions[i];
    ferris_wheel.add_child_node(parent, va, rotation, mat4_translate(p.x, p.y, p.z), nullptr, 0);
  }

  return true;
}

bool build_shader_programs() {
  DynArray<i8> vertex;
  DynArray<i8> fragment;
  DynArray<i8> tess_ctrl;
  DynArray<i8> tess_eval;

  vertex.init(&allocator, MemoryTag::Shader);
  fragment.init(&allocator, MemoryTag::Shader);
  tess_ctrl.init(&allocator, MemoryTag::Shader);
  tess_eval.init(&allocator, MemoryTag::Shader);

  if (read_file(&vertex, system_base_dir("shaders/skybox.v.shader"))
      && read_file(&fragment, system_base_dir("shaders/skybox.f.shader"))) {
    u32 idx = renderer.begin_shader_program();
    renderer.program_add_shader(idx, ShaderType::Vertex, &vertex);
    renderer.program_add_shader(idx, ShaderType::Fragment, &fragment);
    skybox_program = renderer.link_shader_program(idx);
  } else {
    return false;
  }

  vertex.reset();
  fragment.reset();

  if (read_file(&vertex, system_base_dir("shaders/world.v.shader"))
      && read_file(&fragment, system_base_dir("shaders/world.f.shader"))) {
    u32 idx = renderer.begin_shader_program();
    renderer.program_add_shader(idx, ShaderType::Vertex, &vertex);
    renderer.program_add_shader(idx, ShaderType::Fragment, &fragment);
    world_program = renderer.link_shader_program(idx);
  } else {
    return false;
  }

  vertex.reset();
  fragment.reset();

  if (read_file(&vertex, system_base_dir("shaders/balloon.v.shader"))
      && read_file(&tess_ctrl, system_base_dir("shaders/balloon.tc.shader"))
      && read_file(&tess_eval, system_base_dir("shaders/balloon.te.shader"))
      && read_file(&fragment, system_base_dir("shaders/balloon.f.shader"))) {
    u32 idx = renderer.begin_shader_program();
    renderer.program_add_shader(idx, ShaderType::Vertex, &vertex);
    renderer.program_add_shader(idx, ShaderType::TessCtrl, &tess_ctrl);
    renderer.program_add_shader(idx, ShaderType::TessEval, &tess_eval);
    renderer.program_add_shader(idx, ShaderType::Fragment, &fragment);
    balloon_program = renderer.link_shader_program(idx);
  } else {
    return false;
  }

  vertex.clear();
  fragment.clear();
  tess_ctrl.clear();
  tess_eval.clear();
  return true;
}

bool build_texture_objects() {
  Image skybox_images[6];
  if (!load_skybox_images(skybox_images, &allocator)) {
    return false;
  }
  skybox_texture = renderer.build_texture_cube(skybox_images);
  free_skybox_images(skybox_images, &allocator);
  Image tent_color{};
  if (!load_tga_file(&tent_color, &allocator, system_base_dir("assets/tent_color.tga"))) {
    return false;
  }
  tent_texture = renderer.build_texture_2d(&tent_color);
  free_image(&tent_color, &allocator);

  Image image{};
  if (!load_tga_file(&image, &allocator, system_base_dir("assets/platform2.tga"))) {
    return false;
  }

  Image image2{};
  if (!load_tga_file(&image2, &allocator, system_base_dir("assets/ground.tga"))) {
    return false;
  }

  platform_texture = renderer.build_texture_2d(&image);
  ground_texture = renderer.build_texture_2d(&image2);

  Image image3{};
  if (!load_tga_file(&image3, &allocator, system_base_dir("assets/ferris_color.tga"))) {
    return false;
  }

  ferris_color = renderer.build_texture_2d(&image3);

  return true;
}

bool load_skybox_images(Image* images, DynamicAllocator* allocator) {
  if (load_tga_file(&images[0], allocator, system_base_dir("assets/pz.tga"))) {
    if (load_tga_file(&images[1], allocator, system_base_dir("assets/nz.tga"))) {
      if (load_tga_file(&images[2], allocator, system_base_dir("assets/px.tga"))) {
        if (load_tga_file(&images[3], allocator, system_base_dir("assets/nx.tga"))) {
          if (load_tga_file(&images[4], allocator, system_base_dir("assets/py.tga"))) {
            if (load_tga_file(&images[5], allocator, system_base_dir("assets/ny.tga"))) {
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

void free_skybox_images(Image* images, DynamicAllocator* allocator) {
  for (i8 i = 0; i < 6; i++) {
    free_image(&images[i], allocator);
  }
}

bool load_vec4_file(DynArray<vec4>* data, const char* filename) {
  ASSERT(data != nullptr);
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

    if (strncmp(buf, "#", 1) != 0) {
      vec4 v;

      if (sscanf(buf, "%f %f %f %f\n", &v.x, &v.y, &v.z, &v.w) == 4) {
        data->push_back(v);
      } else {
        LOG_ERROR("Failed to parse \"%s\"", buf);
      }
    }
  }

  return true;
}

} // namespace Themepark
