#ifndef game_h
#define game_h

#include "../engine/steve.h"

#define GAME_WIDTH 1200
#define GAME_HEIGHT 800

typedef struct {
  GLFWwindow* window;
  camera game_camera;
  render_list* game_render_list;
  float delta_time;
  float last_frame;
} game;

game microdrag;

void game_init(GLFWwindow* window);

#endif
