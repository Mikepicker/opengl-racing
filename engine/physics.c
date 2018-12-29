#include "physics.h"

aabb physics_compute_aabb(object* object) {
  aabb aabb;

  mesh* first_mesh = &object->meshes[0];
  float min_x = first_mesh->vertices[0].x;
  float max_x = first_mesh->vertices[0].x;
  float min_y = first_mesh->vertices[0].y;
  float max_y = first_mesh->vertices[0].y;
  float min_z = first_mesh->vertices[0].z;
  float max_z = first_mesh->vertices[0].z;

  for (int i = 0; i < object->num_meshes; i++) {
    mesh* mesh = &object->meshes[i];
    for (int j = 0; j < mesh->num_vertices; j++) {
      vertex* v = &mesh->vertices[j];

      if (v->x < min_x) { min_x = v->x; } 
      if (v->x > max_x) { max_x = v->x; }

      if (v->y < min_y) { min_y = v->y; } 
      if (v->y > max_y) { max_y = v->y; }

      if (v->z < min_z) { min_z = v->z; } 
      if (v->z > max_z) { max_z = v->z; }
    }
  } 

  vec3 pos = {(min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2};
  vec3_copy(aabb.offset, pos);

  aabb.w = max_x - min_x;
  aabb.h = max_y - min_y;
  aabb.d = max_z - min_z;

  return aabb;
}
