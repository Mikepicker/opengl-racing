#ifndef object_h
#define object_h

#include "../engine.h"
#include "mesh.h"

typedef struct {
 vec3 position;
 vec3 scale;
 quat rotation;

 mesh* meshes;
 int num_meshes;
} object;

#endif
