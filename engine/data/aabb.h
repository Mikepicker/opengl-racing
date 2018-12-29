#ifndef aabb_h
#define aabb_h

#include "../engine.h"

typedef struct {
  vec3 offset;
  float w, h, d;
  
  GLuint vao, vbo, ebo;
} aabb;

#endif
