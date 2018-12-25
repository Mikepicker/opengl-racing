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
  vec3 y_axis = {0.0f, 1.0f, 0.0f};
  editor_current_angle = (editor_current_angle + 90) % 360;
  quat_rotate(obj->rotation, to_radians(editor_current_angle), y_axis);
}

void editor_move_piece(vec3 pos) {
  object* obj = editor_objects[editor_current_index];
  vec3_add(obj->position, obj->position, pos);
}

object* editor_current_object() {
  return editor_objects[editor_current_index];
}

