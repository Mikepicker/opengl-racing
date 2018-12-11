#ifndef renderer_h
#define renderer_h

#include "engine.h"
#include "data/object.h"
#include "data/light.h"
#include "stb_image.h"
#include "debug.h"

GLFWwindow* window;

int renderer_init(char* title, int width, int height, void* key_callback, void* mouse_callback);
void renderer_cleanup();
int renderer_should_close();
void renderer_add_object(object* o);
void renderer_render_objects(object *objects[], int objects_length, light *lights[], int lights_length, GLuint shader_id, camera_obj *camera);

#endif
