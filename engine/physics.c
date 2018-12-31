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

  aabb.min_x = min_x;
  aabb.max_x = max_x;
  aabb.min_y = min_y;
  aabb.max_y = max_y;
  aabb.min_z = min_z;
  aabb.max_z = max_z;

  vec3 center = {(min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2};
  vec3_copy(aabb.offset, center);

  aabb.w = max_x - min_x;
  aabb.h = max_y - min_y;
  aabb.d = max_z - min_z;

  if (aabb.w > aabb.d) {
    aabb.d = aabb.w;
  } else {
    aabb.w = aabb.d;
  }

  if (aabb.h < 0.1f) {
    aabb.h = 0.1f;
  }

  return aabb;
}


int physics_objects_collide(object* a, object* b) {
    return a->position[0] + a->aabb.min_x < b->position[0] + b->aabb.max_x &&
    a->position[0] + a->aabb.max_x > b->position[0] + b->aabb.min_x &&
    a->position[1] + a->aabb.min_y < b->position[1] + b->aabb.max_y &&
    a->position[1] + a->aabb.max_y > b->position[1] + b->aabb.min_y &&
    a->position[2] + a->aabb.min_z < b->position[2] + b->aabb.max_z &&
    a->position[2] + a->aabb.max_z > b->position[2] + b->aabb.min_z;
}
