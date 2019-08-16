#ifndef renderer_h
#define renderer_h

#include "engine.h"
#include "shader.h"
#include "skybox.h"
#include "data/object.h"
#include "data/light.h"
#include "data/camera.h"
#include "data/ray.h"

#define SHADOW_WIDTH 1024 * 2
#define SHADOW_HEIGHT 1024 * 2

GLFWwindow* window;

GLuint renderer_main_shader;
GLuint renderer_shadow_shader;
GLuint renderer_debug_shader;
GLuint renderer_skybox_shader;

GLuint renderer_depth_fbo;
GLuint renderer_depth_map;
GLuint renderer_debug_vao;
GLuint renderer_debug_vbo;

int renderer_debug_enabled;
float renderer_shadow_bias;
int renderer_shadow_pcf_enabled;
float renderer_shadow_near;
float renderer_shadow_far;

int renderer_init(char* title, int width, int height, GLFWwindow** out_window);
void renderer_cleanup();
void renderer_recompile_shader();
int renderer_should_close();
void renderer_init_object(object* o);
void renderer_render_objects(object* objects[], int objects_length, light* lights[], int lights_length, camera* camera, void (*ui_render_callback)(void), skybox* sky, int debug);

#endif
