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
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "image.h"

#include <glad/glad.h>

namespace Themepark {

u32 va_platform = 0;
u32 va_cube = 0;
u32 va_skybox = 0;
u32 world_program = 0;
u32 platform_texture = 0;
u32 ground_texture = 0;
u32 skybox_texture = 0;
u32 skybox_program = 0;
u32 tent_color = 0;
u32 tent_texture = 0;

DynamicAllocator allocator;
Renderer renderer;
Camera camera;
CameraMatrixBlock camera_block;
DynArray<vec3> tent_locations;

bool load_skybox_images(Image* images, DynamicAllocator* allocator);
void free_skybox_images(Image* images, DynamicAllocator* allocator);
bool load_tent_locations(DynArray<vec3>* locations);
bool build_shader_programs();
bool build_mesh_vertex_arrays(); //TODO:
bool build_texture_objects();    //TODO:

bool themepark_startup(u32 view_width, u32 view_height) {
  if (!allocator.startup(MiB(200))) {
    return false;
  }

  if (!renderer.startup(&allocator)) {
    return false;
  }

  Mesh platform(&allocator);
  if (!platform.load_from_obj(system_base_dir("assets/platform.obj"))) {
    return false;
  }

  Mesh cube(&allocator);
  if (!cube.load_from_obj(system_base_dir("assets/tent.obj"))) {
    return false;
  }

  Mesh skybox(&allocator);
  if (!skybox.load_from_obj(system_base_dir("assets/cube.obj"))) {
    return false;
  }

  tent_locations.init(&allocator, MemoryTag::Mesh);
  if (!load_tent_locations(&tent_locations)) {
    return false;
  }

  if (!build_shader_programs()) {
    return false;
  }

  if (!build_texture_objects()) {
    return false;
  }


  va_platform = renderer.build_vertex_array(&platform);
  va_cube = renderer.build_vertex_array(&cube);
  va_skybox = renderer.build_vertex_array(&skybox);

  camera.startup(vec3{0.0F, 10.0F, 5.0F}, vec3(0.0F, 1.0F, 0.0F), -90, 0);
  renderer.set_clear_color(0.0F, 0.2F, 0.5F);
  renderer.set_viewport(0, 0, view_width, view_height);
  renderer.enable_depth_test(true);

  return true;
}

void themepark_run(RunContext* context) {
  mat4 model = mat4_translate(0, 0, 0);
  memset(&camera_block, 0, sizeof(CameraMatrixBlock));
  camera.update_view_matrices(&camera_block, context->input, context->delta_time);
  mat4 projection = mat4_perspective(45.0F, 0.1F, 1000.0F, f32(context->width) / f32(context->height));

  renderer.begin_frame();
  glDepthMask(GL_FALSE); //TODO:
  glFrontFace(GL_CW);    //TODO:
  renderer.use_shader_program(skybox_program);
  renderer.shader_set_uniform(
      renderer.shader_uniform_location(skybox_program, "view"), camera_block.rotation);
  renderer.shader_set_uniform(
      renderer.shader_uniform_location(skybox_program, "projection"), projection);
  renderer.shader_set_uniform(renderer.shader_uniform_location(skybox_program, "skybox_texture"),
      renderer.use_texture_cube(skybox_texture));

  renderer.draw_vertex_array(va_skybox);
  glFrontFace(GL_CCW);  //TODO:
  glDepthMask(GL_TRUE); //TODO:

  renderer.use_shader_program(world_program);
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

  model = mat4_scale(3, 2, 3); //* mat4_translate(0, 3, 5);
  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "model"), model);
  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "tent_position"),
      tent_locations.data(), tent_locations.size());
  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "first_texture"),
      renderer.use_texture_2d(tent_texture));
  renderer.shader_set_uniform(renderer.shader_uniform_location(world_program, "second_texture"),
      renderer.use_texture_2d(tent_texture));

  renderer.draw_vertex_array_instanced(va_cube, tent_locations.size());
  renderer.end_frame();
}

void themepark_shutdown() {
  tent_locations.clear();
  renderer.shutdown();
  allocator.shutdown();
}

bool build_shader_programs() {
//TODO: Improve renderer shader building interface.
  DynArray<i8> vertex_shader;
  DynArray<i8> fragment_shader;

  vertex_shader.init(&allocator, MemoryTag::Shader);
  fragment_shader.init(&allocator, MemoryTag::Shader);

  if (read_file(&vertex_shader, system_base_dir("shaders/skybox.v.shader"))
      && read_file(&fragment_shader, system_base_dir("shaders/skybox.f.shader"))) {
    skybox_program = renderer.build_shader_program(&vertex_shader, &fragment_shader);
  } else {
    return false;
  }

  vertex_shader.reset();
  fragment_shader.reset();

  if (read_file(&vertex_shader, system_base_dir("shaders/world.v.shader"))
      && read_file(&fragment_shader, system_base_dir("shaders/world.f.shader"))) {
    world_program = renderer.build_shader_program(&vertex_shader, &fragment_shader);
  } else {
    return false;
  }

  vertex_shader.clear();
  fragment_shader.clear();
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

bool load_tent_locations(DynArray<vec3>* locations) {
  ASSERT(locations != nullptr);
  FILE* file = fopen(system_base_dir("assets/tent.map"), "r");
  if (file == nullptr) {
    LOG_ERROR("Failed to open %s!", "assets/tent.map");
    return false;
  }

  char buf[MAX_READ_LEN];

  for (;;) {
    memset(buf, 0, sizeof(buf));
    if (fgets(buf, sizeof(buf), file) == nullptr) {
      break;
    }

    if (strncmp(buf, "#", 1) != 0) {
      vec3 loc;

      if (sscanf(buf, "%f %f %f\n", &loc.x, &loc.y, &loc.z) == 3) {
        locations->push_back(loc);
      } else {
        LOG_ERROR("MAP loader: failed to parse \"%s\"", buf);
      }
    }
  }

  return true;
}


} // namespace Themepark
