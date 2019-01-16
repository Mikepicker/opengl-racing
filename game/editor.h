#ifndef editor_h
#define editor_h

#include "../engine/steve.h"

#define EDITOR_OBJECTS_COUNT 5
#define EDITOR_MAX_PLACED_OBJECTS 256

typedef struct {
  int enabled;
  object* objects[EDITOR_OBJECTS_COUNT];
  object* placed_objects[EDITOR_MAX_PLACED_OBJECTS];
  object* render_list[EDITOR_MAX_PLACED_OBJECTS + 1];
  unsigned int current_index;
  unsigned int current_angle;
  unsigned int placed_objects_size;
  unsigned int render_list_size;
  vec3 current_pos;
} editor;

editor game_editor;

void editor_init();
void editor_set_enabled(int on);
void editor_next_piece();
void editor_rotate_piece();
void editor_move_piece(vec3 pos);
void editor_update();
void editor_place_piece();
void editor_remove_piece();
object* editor_current_object();
void editor_free();

#endif
