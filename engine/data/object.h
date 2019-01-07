#ifndef object_h
#define object_h

#include "../engine.h"
#include "mesh.h"
#include "aabb.h"

typedef struct {
 vec3 position;
 vec3 center;
 GLfloat scale;
 quat rotation;

 mesh* meshes;
 int num_meshes;

 int glowing;

 aabb box;
} object;

void object_free(object* o);

#endif
