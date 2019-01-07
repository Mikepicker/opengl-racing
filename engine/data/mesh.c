#include "mesh.h"

void mesh_free(mesh* m) {
  free(m->vertices);
}
