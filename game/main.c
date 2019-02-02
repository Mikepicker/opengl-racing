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
  if (renderer_init("Microdrag", GAME_WIDTH, GAME_HEIGHT, &window) < 0) {
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

  /* lights */
  light l1;
  l1.position[0] = 0.0f;
  l1.position[1] = 4.0f;
  l1.position[2] = 0.0f;
  l1.color[0] = 1.0f;
  l1.color[1] = 1.0f;
  l1.color[2] = 1.0f;

  microdrag.lights[0] = l1;
  microdrag.num_lights = 1;

  // test audio
  audio_load_sound("assets/audio/test.wav");
  ALuint s;
  audio_add_source(&s);
  audio_play_source(s);

  // cars
  vec3 car_pos = {0.0f, 0.05f, 0.0f};
  microdrag.cars[0] = *entities_new_car(car_pos, "assets/racing/raceCarRed.obj");
  vec3 red_car_pos = {2.0f, 0.05f, 0.0f};
  microdrag.cars[1] = *entities_new_car(red_car_pos, "assets/racing/raceCarGreen.obj");
  microdrag.num_cars = 2;

  int macMoved = 0;
  while (!renderer_should_close()) {
    float current_frame = glfwGetTime();
    microdrag.delta_time = current_frame - microdrag.last_frame;
    microdrag.last_frame = current_frame;

    // microdrag.lights[0].position[0] = 4 + sinf(current_frame);
    //microdrag.cars[0].obj->position[1] = 1.0f + sinf(2.0f * current_frame);

    // audio
    audio_source_playing(s);

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
    render_list_add_batch(microdrag.game_render_list, game_editor.render_list, game_editor.render_list_size);

    // render editor
    renderer_render_objects(microdrag.game_render_list->objects, microdrag.game_render_list->size, &microdrag.lights, microdrag.num_lights, &microdrag.game_camera, ui_render, ui_debug);

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
  audio_free();
  editor_free();
  game_free();

  return 0;
}
