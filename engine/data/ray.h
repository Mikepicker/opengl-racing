#ifndef ray_h
#define ray_h

#include "../engine.h"

typedef struct {
  vec3 o;
  vec3 dir;
  float length;
} ray;

#endif
