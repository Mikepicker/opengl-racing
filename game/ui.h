#ifndef ui_h
#define ui_h

#include "../engine/steve.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

camera ui_camera;

void ui_init();
void ui_render();
void ui_free();

void ui_set_camera(camera cam);

#endif
