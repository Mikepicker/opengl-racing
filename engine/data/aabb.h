#ifndef aabb_h
#define aabb_h

#include "../engine.h"

typedef struct {
  float min_x, max_x;
  float min_y, max_y;
  float min_z, max_z;
  
  GLuint vao, vbo, ebo;
} aabb;

#endif
