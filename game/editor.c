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
    vec3 pos = {0.0f, 0.001f, 0.0f}; // y to 0.001 to avoid z-fighting
    vec3_copy(editor_objects[i]->position, pos);
    editor_objects[i]->glowing = 1;
    editor_objects[i]->aabb = physics_compute_aabb(editor_objects[i]);
    renderer_add_object(editor_objects[i]);
  }

  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    editor_placed_objects[i] = NULL;
  }
}

int editor_placed_count() {
  int count = 0;
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    if (editor_placed_objects[i] != NULL) 
      count++;
  }
  return count;
}

void editor_next_piece() {
  editor_current_index = (editor_current_index + 1) % EDITOR_OBJECTS_COUNT;
  object* obj = editor_objects[editor_current_index];
  vec3_copy(obj->position, editor_current_pos);
}

void editor_rotate_piece() {
  editor_current_angle = (editor_current_angle + 90) % 360;
}

void editor_move_piece(vec3 pos) {
  object* obj = editor_objects[editor_current_index];
  vec3_add(editor_current_pos, editor_current_pos, pos);
  vec3_copy(obj->position, editor_current_pos);
}

static object* collide_with(object* o) {
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    if (editor_placed_objects[i] != NULL && physics_objects_collide(o, editor_placed_objects[i])) {
      return editor_placed_objects[i];
    }
  }
  return NULL;
}

static int editor_find_empty_index() {
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    if (editor_placed_objects[i] == NULL) {
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

  object* obj = editor_objects[editor_current_index];
  if (collide_with(obj) != NULL) {
    printf("[editor] collision!\n");
    return;
  }
  object* obj_clone = malloc(sizeof(object));
  memcpy(obj_clone, obj, sizeof(object));
  obj_clone->position[1] = 0.0f;
  obj_clone->glowing = 0;

  editor_placed_objects[i] = obj_clone;
}

void editor_remove_piece() {
  object* o = editor_objects[editor_current_index];
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    if (editor_placed_objects[i] != NULL && physics_objects_collide(o, editor_placed_objects[i])) {
      free(editor_placed_objects[i]);
      editor_placed_objects[i] = NULL;
    }
  }
}

void editor_objects_to_render(object* out_objects[]) {
  out_objects[0] = editor_current_object();
  int c = 1;
  for (int i = 0; i < EDITOR_MAX_PLACED_OBJECTS; i++) {
    if (editor_placed_objects[i] != NULL) {
      out_objects[c++] = editor_placed_objects[i];
    }
  }
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
