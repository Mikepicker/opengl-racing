#ifndef input_h
#define input_h

#include "../engine/steve.h"
#include "game.h"
#include "editor.h"

typedef struct {
  int first_mouse;
  float yaw;
  float pitch;
  float mouse_last_x;
  float mouse_last_y;
  float fov;
  float sensitivity;
  int capture_cursor;
  int joystick_1_present;
  int joystick_2_present;
} input;

input game_input;

void input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void input_mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void input_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void input_joystick_callback(int joy, int event);

void input_init();
void input_update();

#endif
