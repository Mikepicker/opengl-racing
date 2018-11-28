#ifndef renderer_h
#define renderer_h

#include "engine.h"

#include "object.h"

GLFWwindow* window;

int renderer_init(char* title, int width, int height, void* key_callback);
void renderer_cleanup();
int renderer_should_close();
void renderer_add_object(object* o);
void renderer_render_objects(object* objects[], int objects_length, GLFWwindow* window, GLuint shaderID);

#endif
