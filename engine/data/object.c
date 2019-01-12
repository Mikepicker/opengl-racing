#include "object.h"

void object_get_transform(const object* o, mat4x4 m) {
  mat4x4_identity(m);

  // scale
  mat4x4_identity(m);
  //mat4x4_scale(m, m, o->scale);

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

void object_free(object* o) {
  mesh_free(o->meshes);
}
