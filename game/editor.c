#include "editor.h"

void editor_init() {
  const char* editor_objects_names[EDITOR_OBJECTS_COUNT] = {
    "assets/racing/roadStraight.obj",
    "assets/racing/roadCornerSmall.obj",
    "assets/racing/roadCornerLarge.obj",
    "assets/racing/roadCurved.obj"
  };

  editor_current_index = 0;
  for (int i = 0; i < EDITOR_OBJECTS_COUNT; i++) {
    editor_objects[i] = importer_load_obj(editor_objects_names[i]);
    renderer_add_object(editor_objects[i]);
  }
}

void editor_next_piece() {
  editor_current_index = (editor_current_index + 1) % EDITOR_OBJECTS_COUNT;
}

object* editor_current_object() {
  return editor_objects[editor_current_index];
}
