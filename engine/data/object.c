#include "object.h"

void object_free(object* o) {
  mesh_free(o->meshes);
}
