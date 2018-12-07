#ifndef material_h
#define material_h

#include "../engine.h"

typedef struct {
  char name[256];
  char texture_path[256]; 
  vec3 ambient;
} material;

#endif
