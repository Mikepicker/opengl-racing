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

  // make aabb squared
  /*float w = aabb.max_x - aabb.min_x;
  float d = aabb.max_z - aabb.min_z;
  float diff_wd = fabsf(w - d);
  if (w > d) {
    aabb.max_z += diff_wd / 2;
    aabb.min_z -= diff_wd / 2;
  } else {
    aabb.max_x += diff_wd / 2;
    aabb.min_x -= diff_wd / 2;
  }

  // make aabb at least 0.1 high
  float h = aabb.max_y - aabb.min_y;
  if (h < 0.1f) {
    float diff_h = fabsf(h - 0.1f);
    aabb.min_y += diff_h / 2;
    aabb.max_y -= diff_h / 2;
  }*/

  return aabb;
}


int physics_objects_collide(object* a, object* b) {
  return a->position[0] + a->box.min_x < b->position[0] + b->box.max_x &&
    a->position[0] + a->box.max_x > b->position[0] + b->box.min_x &&
    a->position[1] + a->box.min_y < b->position[1] + b->box.max_y &&
    a->position[1] + a->box.max_y > b->position[1] + b->box.min_y &&
    a->position[2] + a->box.min_z < b->position[2] + b->box.max_z &&
    a->position[2] + a->box.max_z > b->position[2] + b->box.min_z;
}
