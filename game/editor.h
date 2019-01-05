#ifndef editor_h
#define editor_h

#include "../engine/steve.h"

#define EDITOR_OBJECTS_COUNT 5
#define EDITOR_MAX_PLACED_OBJECTS 256

object* editor_objects[EDITOR_OBJECTS_COUNT];
object* editor_placed_objects[EDITOR_MAX_PLACED_OBJECTS];
unsigned int editor_placed_count;
unsigned int editor_current_index;
unsigned int editor_current_angle;
vec3 editor_current_pos;
GLFWwindow* editor_window;

void editor_init();
void editor_next_piece();
void editor_rotate_piece();
void editor_move_piece(vec3 pos);
void editor_place_piece();
void editor_remove_piece();
object* editor_current_object();
void editor_free();

#endif
