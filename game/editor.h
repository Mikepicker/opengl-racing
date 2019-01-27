#ifndef editor_h
#define editor_h

#include "../engine/steve.h"

#define EDITOR_OBJECTS_COUNT 3
#define EDITOR_MAX_PLACED_OBJECTS 256
#define EDITOR_MAP_SIZE 10
#define EDITOR_MAP_DIR "maps/"

typedef struct {
  int enabled;
  object* objects[EDITOR_OBJECTS_COUNT];
  object* placed_objects[EDITOR_MAX_PLACED_OBJECTS];
  object* render_list[EDITOR_MAX_PLACED_OBJECTS + 1];
  unsigned int placed_indices[EDITOR_MAX_PLACED_OBJECTS];
  int placed_angles[EDITOR_MAX_PLACED_OBJECTS];
  unsigned int current_index;
  int current_angle;
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
void editor_serialize(const char* filename);
void editor_deserialize(const char* filename);
void editor_free();

#endif
