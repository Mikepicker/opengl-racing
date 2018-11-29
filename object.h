#ifndef object_h
#define object_h

#include "engine.h"

typedef struct {
 vec3 position;
 vec3 scale;
 quat rotation;

 GLfloat* vertices;
 GLuint num_vertices;
 GLuint* faces;
 GLuint num_faces;
 GLuint vao, vbo, ebo;
} object;

#endif
