#ifndef object_h
#define object_h

#include "../engine.h"
#include "mesh.h"

typedef struct {
 vec3 position;
 GLfloat scale;
 quat rotation;

 mesh* meshes;
 int num_meshes;
} object;

#endif