#ifndef editor_h
#define editor_h

#include "../engine/steve.h"

#define EDITOR_OBJECTS_COUNT 4

object* editor_objects[EDITOR_OBJECTS_COUNT];
unsigned int editor_current_index;
GLFWwindow* editor_window;

void editor_init();
void editor_next_piece();
object* editor_current_object();

#endif
