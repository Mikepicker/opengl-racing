#include "mesh.h"

void mesh_compute_tangent(mesh* m) {
  for (unsigned int i = 0; i < m->num_indices; i += 3) {
    vertex v1, v2, v3;
    v1 = m->vertices[m->indices[i]];
    v2 = m->vertices[m->indices[i + 1]];
    v3 = m->vertices[m->indices[i + 2]];

    vec2 uv1 = { v1.u, v1.v };
    vec2 uv2 = { v2.u, v2.v };
    vec2 uv3 = { v3.u, v3.v };

    vec3 v1_pos = { v1.x, v1.y, v1.z };
    vec3 v2_pos = { v2.x, v2.y, v2.z };
    vec3 v3_pos = { v3.x, v3.y, v3.z };

    vec3 edge1;
    vec3 edge2;
    vec3_sub(edge1, v2_pos, v1_pos);
    vec3_sub(edge2, v3_pos, v1_pos);

    vec2 delta_uv1;
    vec2 delta_uv2;
    vec2_sub(delta_uv1, uv2, uv1);
    vec2_sub(delta_uv2, uv3, uv1);

    GLfloat f = 1.0f / (delta_uv1[0] * delta_uv2[1] - delta_uv2[0] * delta_uv1[1]);

    vec3 tangent;
    tangent[0] = f * (delta_uv2[1] * edge1[0] - delta_uv1[1] * edge2[0]);
    tangent[1] = f * (delta_uv2[1] * edge1[1] - delta_uv1[1] * edge2[1]);
    tangent[2] = f * (delta_uv2[1] * edge1[2] - delta_uv1[1] * edge2[2]);
    vec3_norm(tangent, tangent);

    v1.tx = tangent[0];
    v1.ty = tangent[1];
    v1.tz = tangent[2];
    v2.tx = tangent[0];
    v2.ty = tangent[1];
    v2.tz = tangent[2];
    v3.tx = tangent[0];
    v3.ty = tangent[1];
    v3.tz = tangent[2];

    m->vertices[m->indices[i]] = v1;
    m->vertices[m->indices[i + 1]] = v2;
    m->vertices[m->indices[i + 2]] = v3;
  }

  // Normalize
  for (unsigned int i = 0; i < m->num_indices; i++) {
    vertex v = m->vertices[m->indices[i]];
    vec3 tnorm = { v.tx, v.ty, v.tz };
    vec3_norm(tnorm, tnorm);
    m->vertices[m->indices[i]].tx = tnorm[0];
    m->vertices[m->indices[i]].ty = tnorm[1];
    m->vertices[m->indices[i]].tz = tnorm[2];
  }
}

void mesh_free(mesh* m) {
  // glDeleteVertexArrays(1, &(m->vao));
  // glDeleteBuffers(1, &(m->vbo));
  // glDeleteBuffers(1, &(m->ebo));
  free(m->vertices);
}
