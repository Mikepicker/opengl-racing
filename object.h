#ifndef object_h
#define object_h

#include "engine.h"

typedef struct {
 vec3 position;
 vec3 scale;
 quat rotation;

 GLfloat* vertices;
 GLuint num_vertices;
 GLuint vao, vbo;
} object;

#endif
