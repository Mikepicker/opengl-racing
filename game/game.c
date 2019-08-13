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

  microdrag.cars = malloc(MAX_CARS * sizeof(car));
  microdrag.lights = malloc(MAX_LIGHTS * sizeof(light));

  microdrag.state = MENU;
}

void game_start() {
  if (game_editor.start_index != -1) {
    object* start = game_editor.placed_objects[game_editor.start_index];
    vec3 start_pos;
    vec3_copy(start_pos, start->position);
    vec3_scale(start_pos, start->position, start->scale);

    vec3_copy(microdrag.cars[0].obj->position, start_pos);
    microdrag.cars[0].last_piece_index = game_editor.start_index;

    vec3_copy(microdrag.cars[1].obj->position, start_pos);
    microdrag.cars[1].last_piece_index = game_editor.start_index;

    microdrag.state = RACE;
  }
}

void game_update() {
  // camera
  if (microdrag.state == RACE) {
    camera* cam = &microdrag.game_camera;
    vec3_copy(cam->pos, microdrag.cars[0].obj->position);
    cam->pos[1] = 0.6f;
    vec4 front = { 0.0f, 0.0f, -1.0f, 1.0f };
    vec3 dir;
    mat4x4 m;
    mat4x4_from_quat(m, microdrag.cars[0].obj->rotation);
    mat4x4_mul_vec4(dir, m, front);
    vec3_norm(dir, dir);
    vec3_copy(cam->front, dir);

    vec3 off = { dir[0], dir[1], dir[2] };
    vec3_scale(off, off, -2);
    vec3_add(cam->pos, off, cam->pos);
  }
}

void game_free() {
  render_list_free(microdrag.game_render_list);
  free(microdrag.cars);
  free(microdrag.lights);
}
