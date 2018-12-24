#ifndef editor_h
#define editor_h

#include "../engine/steve.h"

static object** editor_objects;
static unsigned int editor_size = 0;
static unsigned int editor_current_index = 0;
static GLFWwindow* editor_window;

void editor_init(GLFWwindow* window, object* objects[], unsigned int size);
void editor_update();
object* editor_current_object();

#endif
