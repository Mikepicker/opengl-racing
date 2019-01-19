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

void input_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
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

  // car
  vec3 y_axis = { 0.0f, 1.0f, 0.0f };
  quat rot, res;
  if (!game_editor.enabled) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      microdrag.cars[0].accel = CAR_ACCEL;
    } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      microdrag.cars[0].accel = -CAR_ACCEL;
    } else {
      microdrag.cars[0].accel = 0.0f;
    }
    
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      quat_rotate(rot, to_radians(CAR_TURN_RATE), y_axis);
      quat_mul(res, microdrag.cars[0].obj->rotation, rot);
      vec4_copy(microdrag.cars[0].obj->rotation, res);
    } else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      quat_rotate(rot, to_radians(-CAR_TURN_RATE), y_axis);
      quat_mul(res, microdrag.cars[0].obj->rotation, rot);
      vec4_copy(microdrag.cars[0].obj->rotation, res);
    } 
  }
}
