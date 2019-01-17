#include "../engine/steve.h"

#include "game.h"
#include "ui.h"
#include "editor.h"
#include "input.h"
#include "entities.h"
#include "data/car.h"

// game objects
car* red_car;

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

  // init input
  input_init();

  // init ui
  ui_init(&microdrag);

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

  // init car
  vec3 car_pos = {-5.0f, 0.0f, 0.0f};
  red_car = entities_new_car(car_pos);

  int macMoved = 0;
  while (!renderer_should_close()) {
    float current_frame = glfwGetTime();
    microdrag.delta_time = current_frame - microdrag.last_frame;
    microdrag.last_frame = current_frame;

    // render
    vec3 y_axis = {0.0f, 1.0f, 0.0f};
    // quat_rotate(t1.rotation, (float)glfwGetTime(), y_axis);
    // t1.scale = 1.0f + sinf(t1.scale + (float)glfwGetTime());
    // t1.position[1] = sinf((float)glfwGetTime());
    // quat_rotate(t2.rotation, (float)glfwGetTime(), z_axis);
    
    // test car
    red_car->obj->position[0] += red_car->speed;
    quat_rotate(red_car->obj->rotation, (float)glfwGetTime(), y_axis);

    // input
    input_update();

    // update editor
    editor_update();

    // update entities
    car* cars[1];
    cars[0] = red_car;
    entities_update(cars, 1, game_editor.placed_objects, EDITOR_MAX_PLACED_OBJECTS);

    // render entities
    render_list_clear(microdrag.game_render_list);
    render_list_add(microdrag.game_render_list, red_car->obj);
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
