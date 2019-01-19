#ifndef game_h
#define game_h

#include "../engine/steve.h"
#include "data/car.h"

#define GAME_WIDTH 1200
#define GAME_HEIGHT 800

#define MAX_CARS 128
#define MAX_LIGHTS 128

typedef struct {
  GLFWwindow* window;
  camera game_camera;
  render_list* game_render_list;
  float delta_time;
  float last_frame;
  car* cars;
  int num_cars;
  light* lights;
  int num_lights;
} game;

game microdrag;

void game_init(GLFWwindow* window);
void game_free();

#endif
