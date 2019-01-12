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
  float w = aabb.max_x - aabb.min_x;
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
  }

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

int physics_point_in_triangle(vec3 p, vec3 v0, vec3 v1, vec3 v2) {
  vec3 d0, d1, d2;
  vec3_sub(d0, v2, v0);
  vec3_sub(d1, v1, v0);
  vec3_sub(d2, p, v0);

  float dot00 = vec3_dot(d0, d0);
  float dot01 = vec3_dot(d0, d1);
  float dot02 = vec3_dot(d0, d2);
  float dot11 = vec3_dot(d1, d1);
  float dot12 = vec3_dot(d1, d2);

  float inv_dom = 1.0f / (dot00 * dot11 - dot01 * dot01);
  float u = (dot11 * dot02 - dot01 * dot12) * inv_dom;
  float v = (dot00 * dot12 - dot01 * dot02) * inv_dom;

  return (u >= 0) && (v >= 0) && (u + v < 1);
}

int physics_point_in_mesh(vec3 p, mesh* m) {
  for (int i = 0; i < m->num_indices; i += 3) {
    vertex vt1 = m->vertices[m->indices[i + 0]];
    vertex vt2 = m->vertices[m->indices[i + 1]];
    vertex vt3 = m->vertices[m->indices[i + 2]];

    vec3 v1 = { vt1.x, vt1.y, vt1.z };
    vec3 v2 = { vt2.x, vt2.y, vt2.z };
    vec3 v3 = { vt3.x, vt3.y, vt3.z };
    if (physics_point_in_triangle(p, v1, v2, v3)) {
      return 1;
    }
  } 
  return 0;
}

mesh* physics_ray_hit_mesh(const ray ray, const object* o) {
  for (int j = 0; j < o->num_meshes; j++) {
    mesh* m = &o->meshes[j];

    for (int i = 0; i < m->num_indices; i += 3) {
      vertex vt1 = m->vertices[m->indices[i + 0]];
      vertex vt2 = m->vertices[m->indices[i + 1]];
      vertex vt3 = m->vertices[m->indices[i + 2]];

      vec3 v1 = { vt1.x, vt1.y, vt1.z };
      vec3 v2 = { vt2.x, vt2.y, vt2.z };
      vec3 v3 = { vt3.x, vt3.y, vt3.z };

      // transform vertices wrt object space
      object_vec3_to_object_space(o, v1);
      object_vec3_to_object_space(o, v2);
      object_vec3_to_object_space(o, v3);

      // compute plane's normal
      vec3 v1v2, v1v3, n;
      vec3_sub(v1v2, v1, v2);
      vec3_sub(v1v3, v1, v3);
      vec3_mul_cross(n, v1v2, v1v3);

      // 1. find p

      // check parallel
      float dot = vec3_dot(n, ray.dir);
      if (fabs(dot) < 0.001) continue;

      // compute d
      float d = vec3_dot(n, v1);

      // compute t
      float t = -(vec3_dot(n, ray.o) + d) / dot;

      // check triangle behind the ray
      if (t < 0) continue;

      // compute intersection point
      vec3 p, tdir;
      vec3_scale(tdir, ray.dir, t);
      vec3_add(p, ray.o, tdir);

      // 2. inside-outside test

      // compute vec perpendicular to triangle's plane
      vec3 c;

      // edge 0
      vec3 edge1, vp1;
      vec3_sub(edge1, v2, v1);
      vec3_sub(vp1, p, v1);
      vec3_mul_cross(c, edge1, vp1); 
      if (vec3_dot(n, c) < 0) continue;

      // edge 1
      vec3 edge2, vp2;
      vec3_sub(edge2, v3, v2);
      vec3_sub(vp2, p, v2);
      vec3_mul_cross(c, edge2, vp2); 
      if (vec3_dot(n, c) < 0) continue;

      // edge 2
      vec3 edge3, vp3;
      vec3_sub(edge3, v1, v3);
      vec3_sub(vp3, p, v3);
      vec3_mul_cross(c, edge3, vp3); 
      if (vec3_dot(n, c) < 0) continue;

      return m;
    } 
  }
  return NULL;
}
