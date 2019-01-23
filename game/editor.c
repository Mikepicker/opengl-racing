#include "editor.h"

static void editor_render_list_update() {
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS + 1; i++)
    game_editor.render_list[i] = NULL;

  int c = 0;

  if (game_editor.enabled > 0) {
    game_editor.render_list[0] = editor_current_object();
    c = 1;
  }

  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    if (game_editor.placed_objects[i] != NULL) {
      game_editor.render_list[c++] = game_editor.placed_objects[i];
    }
  }
  game_editor.render_list_size = c;
}

void editor_set_enabled(int on) {
  game_editor.enabled = on;
  editor_render_list_update();
}


void editor_init() {
  const char* editor_objects_names[EDITOR_OBJECTS_COUNT] = {
    "assets/racing/roadStraight.obj",
    "assets/racing/roadCornerSmall.obj",
    "assets/racing/roadCornerLarge.obj",
    "assets/racing/grass.obj",
    "assets/racing/pylon.obj",
  };

  game_editor.enabled = 0;
  game_editor.current_index = 0;
  game_editor.current_angle = 0;
  game_editor.render_list_size = 1;
  game_editor.current_pos[0] = 0.0f;
  game_editor.current_pos[1] = 0.001f; // y to 0.001 to avoid z-fighting
  game_editor.current_pos[2] = 0.0f;
  
  for (int i = 0; i < EDITOR_OBJECTS_COUNT; i++) {
    game_editor.objects[i] = importer_load_obj(editor_objects_names[i]);
    vec3_copy(game_editor.objects[i]->position, game_editor.current_pos);

    game_editor.objects[i]->scale = 4.0f;

    game_editor.objects[i]->glowing = 1;
    vec3 glow_color = { 1.0f, 1.0f, 1.0f };
    vec3_copy(game_editor.objects[i]->glow_color, glow_color);

    game_editor.objects[i]->receive_shadows = 1;

    game_editor.objects[i]->box = physics_compute_aabb(game_editor.objects[i]);
    renderer_add_object(game_editor.objects[i]);
  }

  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    game_editor.placed_objects[i] = NULL;
  }

  editor_render_list_update();
}

void editor_next_piece() {
  game_editor.current_index = (game_editor.current_index + 1) % EDITOR_OBJECTS_COUNT;
  object* obj = game_editor.objects[game_editor.current_index];
  vec3_copy(obj->position, game_editor.current_pos);
  editor_render_list_update();
}

void editor_rotate_piece() {
  game_editor.current_angle = (game_editor.current_angle + 90) % 360;
  editor_render_list_update();
}

void editor_move_piece(vec3 pos) {
  object* obj = game_editor.objects[game_editor.current_index];
  vec3_add(game_editor.current_pos, game_editor.current_pos, pos);
  vec3_copy(obj->position, game_editor.current_pos);
}

static object* collide_with(object* o) {
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    if (game_editor.placed_objects[i] != NULL && physics_objects_collide(o, game_editor.placed_objects[i])) {
      return game_editor.placed_objects[i];
    }
  }
  return NULL;
}

void editor_update() {
  object* obj = game_editor.objects[game_editor.current_index];
  vec3 red = { 1.0f, 0, 0 };
  vec3 white = { 1.0f, 1.0f, 1.0f };
  vec3_copy(obj->glow_color, white);
  if (collide_with(obj) != NULL) vec3_copy(obj->glow_color, red);
}

static int editor_find_empty_index() {
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    if (game_editor.placed_objects[i] == NULL) {
      return i;
    }
  }
  return -1;
}

void editor_place_piece() {
  int i = editor_find_empty_index();
  if (i == -1) {
    printf("[editor] no more space!");
  }

  object* obj = game_editor.objects[game_editor.current_index];
  if (collide_with(obj) != NULL) {
    printf("[editor] collision!\n");
    return;
  }
  object* obj_clone = malloc(sizeof(object));
  memcpy(obj_clone, obj, sizeof(object));
  obj_clone->position[1] = 0.0f;
  obj_clone->glowing = 0;

  game_editor.placed_objects[i] = obj_clone;

  editor_render_list_update();
}

void editor_remove_piece() {
  object* o = game_editor.objects[game_editor.current_index];
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    if (game_editor.placed_objects[i] != NULL && physics_objects_collide(o, game_editor.placed_objects[i])) {
      free(game_editor.placed_objects[i]);
      game_editor.placed_objects[i] = NULL;
    }
  }
  editor_render_list_update();
}

object* editor_current_object() {
  object* obj = game_editor.objects[game_editor.current_index];
  vec3 y_axis = {0.0f, 1.0f, 0.0f};
  quat_rotate(obj->rotation, to_radians(game_editor.current_angle), y_axis);
  return obj;
}

void editor_free() {
  for (int i = 0; i < EDITOR_OBJECTS_COUNT; i++) {
    free(game_editor.objects[i]);
  }
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    free(game_editor.placed_objects[i]);
  }
}
