#ifndef editor_h
#define editor_h

#include "../engine/steve.h"

#define EDITOR_OBJECTS_COUNT 5

object* editor_objects[EDITOR_OBJECTS_COUNT];
unsigned int editor_current_index;
unsigned int editor_current_angle;
GLFWwindow* editor_window;

void editor_init();
void editor_next_piece();
void editor_rotate_piece();
void editor_move_piece(vec3 pos);
object* editor_current_object();

#endif
