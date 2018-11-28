#ifndef renderer_h
#define renderer_h

#include "engine.h"

#include "object.h"

void renderer_add_object(object* o);
void renderer_render_objects(object* objects, GLFWwindow* window, GLuint shaderID);

#endif
