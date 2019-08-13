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
 vec3 glow_color;
 int receive_shadows;

 aabb box;

 ALuint audio_source;
} object;

object* object_create(vec3 position, GLfloat scale, mesh* meshes, int num_meshes, int compute_center);
void object_get_transform(const object* o, mat4x4 m);
void object_get_center(const object* o, vec3* out_center);
void object_set_center(object* o);
void object_vec3_to_object_space(const object* o, vec3 v);
aabb object_aabb_to_object_space(const object* o, aabb box);
void object_free(object* o);

#endif
