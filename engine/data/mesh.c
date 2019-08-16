#include "mesh.h"

void mesh_free(mesh* m) {
  // glDeleteVertexArrays(1, &(m->vao));
  // glDeleteBuffers(1, &(m->vbo));
  // glDeleteBuffers(1, &(m->ebo));
  free(m->vertices);
}
