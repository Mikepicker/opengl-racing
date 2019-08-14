#include "factory.h"

object* factory_create_sphere(float radius, int sector_count, int stack_count) {
  int num_vertices = sector_count * stack_count;

  vertex* vertices = (vertex*)malloc(3 * num_vertices * sizeof(vertex));
  int vcount = 0;

  float x, y, z, xy;
  float nx, ny, nz, lengthInv = 1.0f / radius;
  float s, t;

  float sector_step = 2 * M_PI / sector_count;
  float stack_step = M_PI / stack_count;
  float sector_angle, stack_angle;

  // generate vertices
  for (int i = 0; i <= stack_count; i++) {
    stack_angle = M_PI / 2 - i * stack_step;  // starting from pi/2 to -pi/2
    xy = radius * cosf(stack_angle);          // r * cos(u)
    z = radius * sinf(stack_angle);           // r * sin(u)

    for (int j = 0; j <= sector_count; j++) {
      sector_angle = j * sector_step;         // starting from 0 to 2pi

      // vertex position (x, y, z)
      x = xy * cosf(sector_angle);
      y = xy * sinf(sector_angle);
      vertices[vcount].x = x;
      vertices[vcount].y = y;
      vertices[vcount].z = z;

      // normalized vertex normal (nx, ny, nz)
      nx = x * lengthInv;
      ny = y * lengthInv;
      nz = z * lengthInv;
      vertices[vcount].nx = nx;
      vertices[vcount].ny = ny;
      vertices[vcount].nz = nz;

      // vertex tex coord (s, t) range between [0, 1]
      s = (float)j / sector_count;
      t = (float)i / stack_count;
      vertices[vcount].u = s;
      vertices[vcount].v = t;
      vcount++;
    }
  }

  // generate indices
  GLuint* indices = (GLuint*)malloc(6 * num_vertices * sizeof(GLuint));
  int icount = 0;
  int k1, k2;
  for (int i = 0; i < stack_count; i++) {
    k1 = i * (sector_count + 1);  // beginning of current stack
    k2 = k1 + sector_count + 1;   // beginning of next stack

    for (int j = 0; j < sector_count; j++, k1++, k2++) {
      // 2 triangles per sector excluding first and last stacks
      // k1 => k2 => k1+1
      if (i != 0) {
        indices[icount++] = k1;
        indices[icount++] = k2;
        indices[icount++] = k1 + 1;
      }

      // k1+1 => k2 => k2+1
      if (i != (stack_count - 1)) {
        indices[icount++] = k1 + 1;
        indices[icount++] = k2;
        indices[icount++] = k2 + 1;
      }
    }
  }

  // prepare and return object
  mesh* m = (mesh*)malloc(sizeof(mesh));
  m->vertices = vertices;
  m->num_vertices = vcount;
  m->indices = indices;
  m->num_indices = icount;

  object* obj = object_create(NULL, 1.0f, m, 1, 1);
  return obj;
}

object* factory_create_plane(float width, float height) {
  vertex* vertices = (vertex*)malloc(4 * sizeof(vertex));
  vertex v1 = { -width/2, 0,-height/2, 0, 0, 0, 1, 0 };
  vertex v2 = { width/2, 0, -height/2, 0, 1, 0, 1, 0 };
  vertex v3 = { width/2, 0, height/2, 1, 1, 0, 1, 0 };
  vertex v4 = { -width/2, 0, height/2, 1, 0, 0, 1, 0 };
  vertices[0] = v1;
  vertices[1] = v2;
  vertices[2] = v3;
  vertices[3] = v4;

  GLuint* indices = (GLuint*)malloc(4 * sizeof(GLuint));
  indices[0] = 0;
  indices[1] = 3;
  indices[2] = 1;
  indices[3] = 3;
  indices[4] = 2;
  indices[5] = 1;

  mesh* m = (mesh*)malloc(sizeof(mesh));
  m->vertices = vertices;
  m->num_vertices = 4;
  m->indices = indices;
  m->num_indices = 6;

  object* obj = object_create(NULL, 1.0f, m, 1, 1);
  return obj;
}
