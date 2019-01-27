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
    "assets/racing/roadCornerLarge.obj"
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
  int new_x = game_editor.current_pos[0] + pos[0];
  int new_y = game_editor.current_pos[2] + pos[2];
  if (new_x > -EDITOR_MAP_SIZE && new_x < EDITOR_MAP_SIZE && new_y > -EDITOR_MAP_SIZE && new_y < EDITOR_MAP_SIZE) {
    object* obj = game_editor.objects[game_editor.current_index];
    vec3_add(game_editor.current_pos, game_editor.current_pos, pos);
    vec3_copy(obj->position, game_editor.current_pos);
  }
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
  game_editor.placed_indices[i] = game_editor.current_index;
  game_editor.placed_angles[i] = game_editor.current_angle;

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

void editor_serialize() {
  FILE *f = fopen("map", "w");
  if (f == NULL) {
    printf("[editor_serialize] error opening file!\n");
    return;
  }

  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    object* o = game_editor.placed_objects[i];
    if (o != NULL) {
      fprintf(f, "%d %d %d %d\n", game_editor.placed_indices[i], (int)o->position[0], (int)o->position[2], game_editor.placed_angles[i]);
    }
  }

  printf("[editor_serialize] map saved!\n");
  fclose(f);
}

void editor_deserialize() {
  FILE *f = fopen("map", "r");
  char line[256];
  if (f == NULL) {
    printf("[editor_deserialize] error opening file!\n");
    return;
  }

  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    if (game_editor.placed_objects[i] != NULL) {
      free(game_editor.placed_objects[i]);
      game_editor.placed_objects[i] = NULL;
    }
  }

  int c = 0;
  while (fgets(line, sizeof(line), f)) {
    unsigned int index;
    int x, z;
    int angle;
    sscanf(line, "%d %d %d %d", &index, &x, &z, &angle);
    object* obj_clone = malloc(sizeof(object));
    memcpy(obj_clone, game_editor.objects[index], sizeof(object));
    obj_clone->position[0] = x;
    obj_clone->position[1] = 0;
    obj_clone->position[2] = z;
    vec3 y_axis = {0.0f, 1.0f, 0.0f};
    quat_identity(obj_clone->rotation);
    quat_rotate(obj_clone->rotation, to_radians(angle), y_axis);
    obj_clone->glowing = 0;
    game_editor.placed_objects[c++] = obj_clone;
  }

  fclose(f);
  editor_render_list_update();

  printf("[editor_deserialize] map loaded\n");
}

void editor_free() {
  for (int i = 0; i < EDITOR_OBJECTS_COUNT; i++) {
    if (game_editor.objects[i] != NULL) {
      object_free(game_editor.objects[i]);
      free(game_editor.objects[i]);
    }
  }
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    if (game_editor.placed_objects[i] != NULL) {
      free(game_editor.placed_objects[i]);
    }
  }
}
