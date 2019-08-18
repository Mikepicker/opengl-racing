#ifndef mesh_h
#define mesh_h

#include "../engine.h"
#include "vertex.h"
#include "material.h"

typedef struct {
  char name[256];

  vertex* vertices;
  GLuint* indices;

  GLuint num_indices;
  GLuint num_vertices;

  GLuint vao, vbo, ebo;
  material mat;

  GLuint texture_id;
  GLuint normal_map_id;
  GLuint specular_map_id;
} mesh;

void mesh_compute_tangent(mesh* m);

#endif
