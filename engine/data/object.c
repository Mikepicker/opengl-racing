#include "object.h"

object* object_create(vec3 position, GLfloat scale, mesh* meshes, int num_meshes, int compute_center) {
  object* obj = (object*)malloc(sizeof(object));

  vec3 zero_vec = { 0, 0, 0 };

  // meshes
  obj->meshes = meshes;
  obj->num_meshes = num_meshes;

  // transform
  quat_identity(obj->rotation);
  obj->scale = 1.0f;
  if (position) { vec3_copy(obj->position, position); }
  else { vec3_copy(obj->position, zero_vec); }
  
  // shader
  obj->receive_shadows = 0;
  obj->glowing = 0;
  vec3_copy(obj->glow_color, zero_vec);

  // center
  if (compute_center) { object_set_center(obj); }

  return obj;
}

void object_get_transform(const object* o, mat4x4 m) {
  mat4x4_identity(m);

  // scale
  mat4x4_identity(m);
  mat4x4_scale(m, m, o->scale);

  // translate
  mat4x4 translation;
  mat4x4_translate(translation, o->position[0], o->position[1], o->position[2]);
  mat4x4_mul(m, m, translation);

  // compute rotation matrix from quaternion
  mat4x4 mat_rot;
  mat4x4_from_quat(mat_rot, o->rotation);

  // rotate around center
  mat4x4 t1;
  mat4x4_translate(t1, o->center[0], o->center[1], o->center[2]);
  mat4x4_mul(m, m, t1);
  mat4x4_mul(m, m, mat_rot);
  mat4x4 t2;
  mat4x4_translate(t2, -o->center[0], -o->center[1], -o->center[2]);
  mat4x4_mul(m, m, t2);
}

void object_get_center(const object* o, vec3* out_center) {
  mesh* first_mesh = &o->meshes[0];
  float min_x = first_mesh->vertices[0].x;
  float max_x = first_mesh->vertices[0].x;
  float min_y = first_mesh->vertices[0].y;
  float max_y = first_mesh->vertices[0].y;
  float min_z = first_mesh->vertices[0].z;
  float max_z = first_mesh->vertices[0].z;

  for (int i = 0; i < o->num_meshes; i++) {
    mesh* mesh = &o->meshes[i];
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
  vec3 center = {(min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2};
  vec3_copy(*out_center, center);
}

void object_set_center(object* o) {
  vec3 center;
  object_get_center(o, &center);
  vec3_copy(o->center, center);
}

void object_vec3_to_object_space(const object* o, vec3 v) {
  vec4 u = { v[0], v[1], v[2], 1.0f };
  vec4 r;
  mat4x4 m;
  object_get_transform(o, m);
  mat4x4_mul_vec4(r, m, u);
  v[0] = r[0];
  v[1] = r[1];
  v[2] = r[2];
}

aabb object_aabb_to_object_space(const object* o, aabb box) {
  aabb res = box;
  res.min_x += o->position[0] * o->scale;
  res.max_x += o->position[0] * o->scale;
  res.min_y += o->position[1] * o->scale;
  res.max_y += o->position[1] * o->scale;
  res.min_z += o->position[2] * o->scale;
  res.max_z += o->position[2] * o->scale;
  return res;
}

void object_free(object* o) {
  mesh_free(o->meshes);
  alDeleteSources(1, &o->audio_source);
}
