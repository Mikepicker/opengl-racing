#ifndef physics_h
#define physics_h

#include "engine.h"
#include "data/object.h"
#include "data/aabb.h"

aabb physics_compute_aabb(object* object);
int physics_objects_collide(object* a, object* b);

#endif
