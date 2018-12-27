#include "editor.h"

void editor_init() {
  const char* editor_objects_names[EDITOR_OBJECTS_COUNT] = {
    "assets/racing/roadStraight.obj",
    "assets/racing/roadCornerSmall.obj",
    "assets/racing/roadCornerLarge.obj",
    "assets/racing/roadCurved.obj",
    "assets/racing/grass.obj"
  };

  editor_current_index = 0;
  editor_current_angle = 0;

  for (int i = 0; i < EDITOR_OBJECTS_COUNT; i++) {
    editor_objects[i] = importer_load_obj(editor_objects_names[i]);
    vec3 pos = {0.0f, 0.0f, 0.0f};
    vec3_copy(editor_objects[i]->position, pos);
    renderer_add_object(editor_objects[i]);
  }
}

void editor_next_piece() {
  editor_current_index = (editor_current_index + 1) % EDITOR_OBJECTS_COUNT;
}

void editor_rotate_piece() {
  object* obj = editor_objects[editor_current_index];
  editor_current_angle = (editor_current_angle + 90) % 360;
}

void editor_move_piece(vec3 pos) {
  object* obj = editor_objects[editor_current_index];
  vec3_add(obj->position, obj->position, pos);
}

void editor_place_piece() {
  object* obj = editor_objects[editor_current_index];
  object* obj_clone = malloc(sizeof(object));
  memcpy(obj_clone, obj, sizeof(object));
  editor_placed_objects[editor_placed_count++] = obj_clone;
}

object* editor_current_object() {
  object* obj = editor_objects[editor_current_index];
  vec3 y_axis = {0.0f, 1.0f, 0.0f};
  quat_rotate(obj->rotation, to_radians(editor_current_angle), y_axis);
  return obj;
}

void editor_free() {
  for (int i = 0; i < EDITOR_OBJECTS_COUNT; i++) {
    free(editor_objects[i]);
  }
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    free(editor_placed_objects[i]);
  }
}
