#include "../engine/steve.h"

#include "game.h"
#include "ui.h"
#include "editor.h"
#include "input.h"

// game objects
object* car;

void init_track() {
  car = importer_load_obj("assets/racing/raceCarRed.obj");
  vec3 pos_2 = {0.0f, 0.0f, 0.0f};
  vec3_copy(car->position, pos_2);
  //car->scale = 0.25f;
  car->box = physics_compute_aabb(car);
  renderer_add_object(car);
}

int main()
{
  // Init context
  GLFWwindow* window;
  if (renderer_init("Microdrag", GAME_WIDTH, GAME_HEIGHT, input_key_callback, input_mouse_callback, input_mouse_button_callback, &window) < 0) {
    printf("Error initializing renderer!\n");
    return -1;
  }

  // init game
  game_init(window);

  // import obj
  init_track();

  // init ui
  ui_init(window);

  // init editor
  editor_init();

  /* lights */
  light* lights[1];
  light l1;
  l1.position[0] = 0.0f;
  l1.position[1] = 1.0f;
  l1.position[2] = 0.0f;
  l1.color[0] = 1.0f;
  l1.color[1] = 1.0f;
  l1.color[2] = 1.0f;

  lights[0] = &l1;

  int macMoved = 0;
  while (!renderer_should_close()) {
    // per-frame time logic
    // --------------------
    float current_frame = glfwGetTime();
    microdrag.delta_time = current_frame - microdrag.last_frame;
    microdrag.last_frame = current_frame;

    // render
    vec3 y_axis = {0.0f, 1.0f, 0.0f};
    // quat_rotate(t1.rotation, (float)glfwGetTime(), y_axis);
    // t1.scale = 1.0f + sinf(t1.scale + (float)glfwGetTime());
    // t1.position[1] = sinf((float)glfwGetTime());
    // quat_rotate(t2.rotation, (float)glfwGetTime(), z_axis);
    
    car->position[0] = sinf((float)glfwGetTime());
    quat_rotate(car->rotation, (float)glfwGetTime(), y_axis);

    // test road collision
    for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
      object* o = game_editor.placed_objects[i];
      if (o != NULL) {
        // vec3 p = { -0.1f, 10.0f, 0 };
        vec3 p = { car->position[0], car->position[1], car->position[2] };
        ray ray_test;
        vec3 ray_dir = { 0, -1.0f, 0 };
        vec3_copy(ray_test.o, p);
        vec3_copy(ray_test.dir, ray_dir);
        ray_test.length = 10;
        mesh* m = physics_ray_hit_mesh(ray_test, o);
        if (m != NULL) {
          //printf("COLLIDE MESH %s\n", m->mat.name);
        }
      }
    }

    // render entities
    render_list_clear(microdrag.game_render_list);
    render_list_add(microdrag.game_render_list, car);
    render_list_add_batch(microdrag.game_render_list, game_editor.render_list, game_editor.render_list_size);

    // render editor
    renderer_render_objects(microdrag.game_render_list->objects, microdrag.game_render_list->size, lights, 1, &microdrag.game_camera, ui_render, ui_debug);

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
  ui_free();
  renderer_cleanup();
  render_list_free(microdrag.game_render_list);
  editor_free();

  return 0;
}
