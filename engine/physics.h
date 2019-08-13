#ifndef physics_h
#define physics_h

#include "engine.h"
#include "data/object.h"
#include "data/mesh.h"
#include "data/aabb.h"
#include "data/ray.h"

void physics_compute_aabb(object* object);
int physics_objects_collide(object* a, object* b);
int physics_point_in_triangle(vec3 p, vec3 v0, vec3 v1, vec3 v2);
int physics_point_in_mesh(vec3 p, mesh* m);
mesh* physics_ray_hit_mesh(const ray ray, const object* o);

#endif
