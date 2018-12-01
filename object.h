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
  int index;
  vertex v;
} vertex_indexed;

typedef struct {
  GLuint v_idx;
  GLuint vt_idx;
  GLuint vn_idx;
} face;

typedef struct {
 vec3 position;
 vec3 scale;
 quat rotation;

 vertex* vertices;
 GLuint* indices;
 GLfloat* normals;
 GLfloat* texcoords;

 GLuint num_faces;
 GLuint num_vertices;
 GLuint num_normals;
 GLuint num_texcoords;

 GLuint vao, vbo, ebo;
} object;

#endif
