#include "input.h"

void input_init() {
  game_input.yaw = -90.0f;
  game_input.pitch = 0.0f;
  game_input.mouse_last_x = GAME_WIDTH / 2.0f;
  game_input.mouse_last_y = GAME_HEIGHT / 2.0f;
  game_input.fov = 45.0f;
  game_input.capture_cursor = 1;
  game_input.first_mouse = 1;
  game_input.sensitivity = 0.01f;

  glfwSetKeyCallback(microdrag.window, input_key_callback);
  glfwSetCursorPosCallback(microdrag.window, input_mouse_callback);
  glfwSetMouseButtonCallback(microdrag.window, input_mouse_button_callback);
  glfwSetJoystickCallback(input_joystick_callback);

  // joysticks
  game_input.joystick_1_present = glfwJoystickPresent(GLFW_JOYSTICK_1);
  printf("[input_init] joystick 1 present: %d\n", game_input.joystick_1_present);

  game_input.joystick_2_present = glfwJoystickPresent(GLFW_JOYSTICK_2);
  printf("[input_init] joystick 2 present: %d\n", game_input.joystick_2_present);
}

void input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  
  // editor
  if (game_editor.enabled && action == GLFW_RELEASE) {
    vec3 pos = { 0.0f, 0.0f, 0.0f };
    if (key == GLFW_KEY_E) {
      editor_next_piece();
    } else if (key == GLFW_KEY_Q) {
      editor_rotate_piece();
    } else if (key == GLFW_KEY_LEFT) {
      pos[0] = -1.0f;
    } else if (key == GLFW_KEY_RIGHT) {
      pos[0] = 1.0f;
    } else if (key == GLFW_KEY_UP) {
      pos[2] = -1.0f;
    } else if (key == GLFW_KEY_DOWN) {
      pos[2] = 1.0f;
    } else if (key == GLFW_KEY_ENTER) {
      editor_place_piece();
    } else if (key == GLFW_KEY_R) {
      editor_remove_piece();
    }
    editor_move_piece(pos);
  }

  // capture mouse
  if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_C) {
      game_input.capture_cursor = !(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL);
      glfwSetInputMode(window, GLFW_CURSOR, game_input.capture_cursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
  }

  // recompile shader
  if (action == GLFW_RELEASE && key == GLFW_KEY_O) {
    renderer_recompile_shader(); 
  }
}

void input_mouse_callback(GLFWwindow* window, double x_pos, double y_pos)
{
  if (game_input.capture_cursor != 0) return;

  if (game_input.first_mouse == 1) {
    game_input.mouse_last_x = x_pos;
    game_input.mouse_last_y = y_pos;
    game_input.first_mouse = 0;
  }

  float x_offset = x_pos - game_input.mouse_last_x;
  float y_offset = game_input.mouse_last_y - y_pos; // reversed since y-coordinates go from bottom to top
  game_input.mouse_last_x = x_pos;
  game_input.mouse_last_y = y_pos;

  x_offset *= game_input.sensitivity;
  y_offset *= game_input.sensitivity;

  game_input.yaw += x_offset;
  game_input.pitch += y_offset;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  if (game_input.pitch > 89.0f)
    game_input.pitch = 89.0f;
  if (game_input.pitch < -89.0f)
    game_input.pitch = -89.0f;

  microdrag.game_camera.front[0] = cosf(game_input.yaw) * cosf(game_input.pitch);
  microdrag.game_camera.front[1] = sinf(game_input.pitch);
  microdrag.game_camera.front[2] = sinf(game_input.yaw) * cosf(game_input.pitch);
  //debug_print_vec3(microdrag.game_camera.front);
}

void input_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {}

void input_joystick_callback(int joy, int event) {
  if (event == GLFW_CONNECTED) {
    if (joy == GLFW_JOYSTICK_1) {
      game_input.joystick_1_present = 1;
      printf("[input_joystick_callback] joystick 1 connected\n");
    } else if (joy == GLFW_JOYSTICK_2) {
      game_input.joystick_2_present = 1;
      printf("[input_joystick_callback] joystick 2 connected\n");
    }
  }
  else if (event == GLFW_DISCONNECTED) {
    if (joy == GLFW_JOYSTICK_1) {
      game_input.joystick_1_present = 0;
      printf("[input_joystick_callback] joystick 1 disconnected\n");
    } else if (joy == GLFW_JOYSTICK_2) {
      game_input.joystick_2_present = 0;
      printf("[input_joystick_callback] joystick 2 disconnected\n");
    }
  }
}

static void rotate_car(car* c, float dir) {
  vec3 y_axis = { 0.0f, 1.0f, 0.0f };
  quat rot, res;
  //float rot_factor = fabsf(c->speed / CAR_MAX_SPEED);
  float rot_factor = 1.0f;

  c->steering_command = 0.0f;
  if (dir == -1.0f) {
    c->steering_command = to_radians(CAR_TURN_RATE) * rot_factor;
  } else if (dir == 1.0f) {
    c->steering_command = -to_radians(CAR_TURN_RATE) * rot_factor;
  } 
}

void input_update() {
  GLFWwindow* window = microdrag.window;

  // camera
  float camera_delta = microdrag.game_camera.speed * microdrag.delta_time;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    vec3 vec3_scaled;
    vec3_scale(vec3_scaled, microdrag.game_camera.front, camera_delta);
    vec3_add(microdrag.game_camera.pos, microdrag.game_camera.pos, vec3_scaled);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    vec3 vec3_temp;
    vec3_scale(vec3_temp, microdrag.game_camera.front, camera_delta);
    vec3_sub(microdrag.game_camera.pos, microdrag.game_camera.pos, vec3_temp);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    vec3 vec3_crossed, vec3_scaled, vec3_normalized;
    vec3_mul_cross(vec3_crossed, microdrag.game_camera.front, microdrag.game_camera.up);
    vec3_norm(vec3_normalized, vec3_crossed);
    vec3_scale(vec3_scaled, vec3_normalized, camera_delta);
    vec3_sub(microdrag.game_camera.pos, microdrag.game_camera.pos, vec3_scaled);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    vec3 vec3_crossed, vec3_scaled, vec3_normalized;
    vec3_mul_cross(vec3_crossed, microdrag.game_camera.front, microdrag.game_camera.up);
    vec3_norm(vec3_normalized, vec3_crossed);
    vec3_scale(vec3_scaled, vec3_normalized, camera_delta);
    vec3_add(microdrag.game_camera.pos, microdrag.game_camera.pos, vec3_scaled);
  }

  // cars
  if (!game_editor.enabled) {

    // car 1
    car* c = &microdrag.cars[0];
    if (!game_input.joystick_1_present) {
      if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        c->accel = CAR_ACCEL;
      } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        c->accel = -CAR_ACCEL;
      } else {
        c->accel = 0.0f;
      }

      if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        rotate_car(c, -1.0f);
      } else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        rotate_car(c, 1.0f);
      } else {
        rotate_car(c, 0.0f);
      }
    } else {
      int count;
      const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
      const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

      if (buttons != NULL) {
        if (buttons[3] == GLFW_PRESS) {
          c->accel = CAR_ACCEL;
        } else if (buttons[2] == GLFW_PRESS) {
          c->accel = -CAR_ACCEL;
        } else {
          c->accel = 0.0f;
        }

        if (axes[0] == -1.0f) {
          rotate_car(c, -1.0f);
        } else if (axes[0] == 1.0f) {
          rotate_car(c, 1.0f);
        } else {
        rotate_car(c, 0.0f);
        } 
      } else {
        printf("[input_update] error joystick 1\n");
      }
    }

    // car 2
    c = &microdrag.cars[1];
    if (!game_input.joystick_2_present) {
      if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        c->accel = CAR_ACCEL;
      } else if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        c->accel = -CAR_ACCEL;
      } else {
        c->accel = 0.0f;
      }

      if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        rotate_car(c, -1.0f);
      } else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        rotate_car(c, 1.0f);
      } else {
        rotate_car(c, 0.0f);
      } 
    } else {
      int count;
      const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_2, &count);
      const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_2, &count);

      if (buttons != NULL) {
        if (buttons[3] == GLFW_PRESS) {
          c->accel = CAR_ACCEL;
        } else if (buttons[2] == GLFW_PRESS) {
          c->accel = -CAR_ACCEL;
        } else {
          c->accel = 0.0f;
        }

        if (axes[0] == -1.0f) {
          rotate_car(c, -1.0f);
        } else if (axes[0] == 1.0f) {
          rotate_car(c, 1.0f);
        } else {
          rotate_car(c, 0.0f);
        } 
      } else {
        printf("[input_update] error joystick 2\n");
      }
    }

  }
}
