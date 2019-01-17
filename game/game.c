#include "game.h"

void game_init(GLFWwindow* window) {
  camera game_camera = {
    .front = {0.0f, 0.0f, -1.0f},
    .up = {0.0f, 1.0f, 0.0f},
    .pos = {0.0f, 2.0f, 9.0f},
    .speed = 10.0f
  };

  microdrag.game_camera = game_camera;

  microdrag.window = window;
  microdrag.delta_time = 0.0f;
  microdrag.last_frame = 0.0f;

  microdrag.game_render_list = render_list_new();
}
