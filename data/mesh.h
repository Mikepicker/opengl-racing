#ifndef mesh_h
#define mesh_h

#include "../engine.h"
#include "material.h"
#include "vertex.h"

typedef struct {
  char name[256];

  vertex* vertices;
  GLuint* indices;

  GLuint num_indices;
  GLuint num_vertices;

  GLuint vao, vbo, ebo;
  material mat;

  GLuint texture_id;
} mesh;

void mesh_free(mesh* m);

#endif
