#include "../engine/steve.h"

#include "game.h"
#include "ui.h"
#include "editor.h"
#include "input.h"
#include "entities.h"
#include "data/car.h"

int main()
{
  // Init context
  GLFWwindow* window;
  if (renderer_init("Microdrag", GAME_WIDTH, GAME_HEIGHT, 1, &window) < 0) {
    printf("Error initializing renderer!\n");
    return -1;
  }

  // Init audio
  if (audio_init() < 0) {
    printf("Error initializing audio\n");
    return -1;
  }

  // init game
  game_init(window);

  // init input
  input_init();

  // init ui
  ui_init(&microdrag);

  // init editor
  editor_init();

  // lights
  light l1;
  l1.position[0] = 0.0f;
  l1.position[1] = 4.0f;
  l1.position[2] = 0.0f;
  l1.color[0] = 1.0f;
  l1.color[1] = 1.0f;
  l1.color[2] = 1.0f;

  microdrag.lights[0] = l1;
  microdrag.num_lights = 1;

  // audio
  audio_load_sound("assets/audio/test.wav", &microdrag.sound_car);

  // cars
  vec3 car_pos = {0.0f, 0.05f, 0.0f};
  microdrag.cars[0] = *entities_new_car(car_pos, "assets/racing/raceCarRed.obj");
  vec3 red_car_pos = {2.0f, 0.05f, 0.0f};
  microdrag.cars[1] = *entities_new_car(red_car_pos, "assets/racing/raceCarGreen.obj");
  microdrag.num_cars = 2;

  // test sphere
  object* sphere = factory_create_sphere(1, 36, 18);
  material mat;
  material_init(&mat);
  strcpy(mat.name, "sphere_mat");
  sphere->meshes[0].mat = mat;
  object_set_center(sphere);
  renderer_init_object(sphere);

  // plane
  object* plane = factory_create_plane(80, 80);
  plane->position[1] = -0.001;
  plane->meshes[0].mat = mat;
  plane->receive_shadows = 1;
  material mat_plane;
  material_init(&mat_plane);
  strcpy(mat_plane.name, "plane_mat");
  strcpy(mat_plane.texture_path, "assets/textures/Wood_Grain_DIFF.png");
  strcpy(mat_plane.normal_map_path, "assets/textures/Wood_Grain_NRM.png");
  strcpy(mat_plane.specular_map_path, "assets/textures/Wood_Grain_SPEC.png");
  mat_plane.texture_subdivision = 5;
  plane->meshes[0].mat = mat_plane;
  object_set_center(plane);
  mesh_compute_tangent(&plane->meshes[0]);
  renderer_init_object(plane);

  // init skybox
  skybox sky;
  const char* faces[6];
  faces[0] = "assets/skybox/skybox_rt.bmp";
  faces[1] = "assets/skybox/skybox_lf.bmp";
  faces[2] = "assets/skybox/skybox_up.bmp";
  faces[3] = "assets/skybox/skybox_dn.bmp";
  faces[4] = "assets/skybox/skybox_ft.bmp";
  faces[5] = "assets/skybox/skybox_bk.bmp";
  skybox_init(&sky, faces);

  int macMoved = 0;
  while (!renderer_should_close()) {
    float current_frame = glfwGetTime();
    microdrag.delta_time = current_frame - microdrag.last_frame;
    microdrag.last_frame = current_frame;

    // microdrag.lights[0].position[0] = 4 + sinf(current_frame);
    //microdrag.cars[0].obj->position[1] = 1.0f + sinf(2.0f * current_frame);
    microdrag.lights[0].position[0] =  24 * sinf(0.5f * current_frame);
    vec3_copy(sphere->position, microdrag.lights[0].position);

    // game
    game_update();
    
    // audio
    audio_move_listener(microdrag.game_camera.pos);

    // input
    input_update();

    // update editor
    editor_update();

    // update entities
    entities_update();

    // render entities
    render_list_clear(microdrag.game_render_list);
    render_list_add(microdrag.game_render_list, microdrag.cars[0].obj);
    render_list_add(microdrag.game_render_list, microdrag.cars[1].obj);
    // render_list_add(microdrag.game_render_list, sphere);
    render_list_add(microdrag.game_render_list, plane);
    render_list_add_batch(microdrag.game_render_list, game_editor.render_list, game_editor.render_list_size);

    // render editor
    renderer_render_objects(microdrag.game_render_list->objects, microdrag.game_render_list->size, &microdrag.lights, microdrag.num_lights, &microdrag.game_camera, ui_render, &sky);

#ifdef __APPLE__ // TODO: remove this workaround with glfw 3.3
      if (macMoved == 0)
      {
        int x, y;
        glfwGetWindowPos(window, &x, &y);
        glfwSetWindowPos(window, ++x, y);
        macMoved = 1;
      }
#endif
  }

  // cleanup
  editor_free();
  ui_free();
  audio_free();
  game_free();
  skybox_free(&sky);
  renderer_cleanup();

  return 0;
}
