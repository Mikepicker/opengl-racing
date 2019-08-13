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
    "assets/racing/roadStraight.obj"
  };

  game_editor.enabled = 0;
  game_editor.current_index = 0;
  game_editor.current_angle = 0;
  game_editor.placed_count = 0;
  game_editor.start_index = -1;
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

    physics_compute_aabb(game_editor.objects[i]);
    renderer_init_object(game_editor.objects[i]);

    // start object
    if (i == 3) {
      vec3 cm = { 0, 0, 1.0f };
      vec3_copy(game_editor.objects[i]->color_mask, cm);
    }
  }

  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    game_editor.placed_objects[i] = NULL;
  }

  editor_render_list_update();
}

void editor_next_piece() {
  game_editor.current_index = (game_editor.current_index + 1) % EDITOR_OBJECTS_COUNT;

  if (game_editor.current_index == 3 && game_editor.start_index != -1) {
    game_editor.current_index = 0;
  }

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

  if (game_editor.current_index == 3) {
    game_editor.start_index = i; 
    editor_next_piece();
  }

  game_editor.placed_count++;

  editor_render_list_update();
}

void editor_remove_piece() {
  object* o = game_editor.objects[game_editor.current_index];
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    if (game_editor.placed_objects[i] != NULL && physics_objects_collide(o, game_editor.placed_objects[i])) {
      free(game_editor.placed_objects[i]);
      game_editor.placed_objects[i] = NULL;

      if (i == game_editor.start_index) { game_editor.start_index = -1; }
      game_editor.placed_count--;
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

object* editor_start_object() {
  if (game_editor.start_index != -1) { return game_editor.placed_objects[game_editor.start_index]; }
  return NULL;
}

void editor_color_car_pieces(const car* red_car, const car* green_car) {
  vec3 zero_vec = { 0, 0, 0 };
  vec3 red_mask = { 0.1f, 0, 0 };
  vec3 green_mask = { 0, 0.1f, 0 };
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    object* o = game_editor.placed_objects[i];
    if (o != NULL && i != game_editor.start_index) {
      if (i == red_car->last_piece_index && i == green_car->last_piece_index) {
        vec3_add(o->color_mask, red_mask, green_mask);
      } else if (i == red_car->last_piece_index) {
        vec3_copy(o->color_mask, red_mask);
      } else if (i == green_car->last_piece_index) {
        vec3_copy(o->color_mask, green_mask);
      } else {
        vec3_copy(o->color_mask, zero_vec);
      }
    }
  }
}

void editor_serialize(const char* filename) {
  char path[256];
  strcpy(path, EDITOR_MAP_DIR);
  strcat(path, filename);
  FILE *f = fopen(path, "w");
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

  printf("[editor_serialize] map \"%s\" saved!\n", filename);
  fclose(f);
}

void editor_deserialize(const char* filename) {
  char path[256];
  strcpy(path, EDITOR_MAP_DIR);
  strcat(path, filename);
  FILE *f = fopen(path, "r");
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

    game_editor.placed_indices[c] = index;
    game_editor.placed_angles[c] = angle;

    if (index == 3) {
      game_editor.start_index = c;
    }

    game_editor.placed_count++;
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
