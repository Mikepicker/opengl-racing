#ifndef game_h
#define game_h

#include "../engine/steve.h"
#include "editor.h"
#include "data/car.h"

#define GAME_WIDTH 1280
#define GAME_HEIGHT 720

#define MAX_CARS 128
#define MAX_LIGHTS 128

enum game_state { MENU, RACE };

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
  ALuint sound_car;
  enum game_state state;
} game;

game microdrag;

void game_init(GLFWwindow* window);
void game_start();
void game_update();
void game_free();

#endif
