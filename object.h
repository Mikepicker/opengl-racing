#ifndef object_h
#define object_h

#include "engine.h"

typedef struct {
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat u;
  GLfloat v;
  GLfloat nx;
  GLfloat ny;
  GLfloat nz;
} vertex;

typedef struct {
 vec3 position;
 vec3 scale;
 quat rotation;

 vertex* vertices;
 GLuint* indices;
 GLfloat* normals;
 GLfloat* texcoords;

 GLuint num_indices;
 GLuint num_vertices;
 GLuint num_normals;
 GLuint num_texcoords;

 GLuint vao, vbo, ebo;
 GLuint texture_id;
} object;

#endif
