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

void object_get_transform(const object* o, mat4x4 m);
void object_vec3_to_object_space(const object* o, vec3 v);
void object_free(object* o);

#endif
