#include "importer.h"

static material** parse_mtl(const char* filename) {
  FILE* file = fopen(filename, "r");
  char line[256];
  material** materials = (material**)malloc(256 * sizeof(material*));

  material* current_mat;
  int index = 0;
  int first = 1;
  while (fgets(line, sizeof(line), file)) {

    // new material
    if (strstr(line, "newmtl") != NULL) {
      if (!first) {
        materials[index++] = current_mat;
      }
      current_mat = (material*)malloc(sizeof(material));
      sscanf(line, "newmtl %s", current_mat->name);
    }
    // texture path
    else if (strstr(line, "map_Kd") != NULL) {
      sscanf(line, "map_Kd %s", current_mat->texture_path);
    }
  }

  materials[index] = current_mat;
  printf("Mat %s %s\n", materials[0]->name, materials[0]->texture_path);
  fclose(file);

  return materials;
}

static void push_index(vertex_hashtable* vh, int* indices, int icount, vertex* vertices, const char* vkey) {
  // search face in the hashmap
  vertex_item* found = vertex_hashtable_search(vh, vkey);

  // get vertex indices (vertex, texcoords, normals)
  int v_index, vt_index, vn_index;
  sscanf(vkey, "%d/%d/%d", &v_index, &vt_index, &vn_index);
  v_index--; vt_index--; vn_index--;

  vertex_indexed vi = { v_index, vertices[v_index] };
  if (found != NULL) {
    indices[icount] = found->data.index;
  } else {
    vertex_hashtable_insert(vh, vkey, vi);
    indices[icount] = v_index;
  }
}

int importer_load_obj(const char *filename, vertex* out_vertices[], GLuint* out_indices[], GLuint* vertices_size, GLuint* indices_size)
{
  /* parse vertices */
  FILE* file = fopen(filename, "r");
  char line[256];

  int vsize = 256;
  vertex* vertices = malloc(vsize * sizeof(vertex));
  int isize = 256;
  int* indices = malloc(isize * sizeof(int));
  int vcount = 0;
  int icount = 0;
  int fcount = 0;

  // init hastable (it will be resized if needed)
  vertex_hashtable* vh = vertex_hashtable_new(isize);

  while (fgets(line, sizeof(line), file)) {

    // realloc vertices list
    if (vcount >= vsize) {
      vsize = vsize * 2;
      vertices = realloc(vertices, vsize * sizeof(vertex));
    }

    // realloc indices list & vertex hashmap
    if (icount + 3 >= isize) {
      isize = isize * 2;
      indices = realloc(indices, isize * sizeof(int));
      vertex_hashtable_resize(vh, isize);
    }

    // new vertex
    if (strstr(line, "v ") != NULL) {
      vertex* v = &vertices[vcount]; 
      sscanf(line, "v %f %f %f", &v->x, &v->y, &v->z);
      vcount++;
    }

    // new face
    if (strstr(line, "f ") != NULL) {

      // parse vertices of current face
      char v1[128];
      char v2[128];
      char v3[128];
      sscanf(line, "f %s %s %s", v1, v2, v3);

      // update indices
      push_index(vh, indices, icount++, vertices, v1);
      push_index(vh, indices, icount++, vertices, v2);
      push_index(vh, indices, icount++, vertices, v3);

      // just parsed 1 face
      fcount++;
    }
  }

  // return values
  *out_vertices = vertices;
  *out_indices = indices;
  *indices_size = icount;
  *vertices_size = vcount;

  fclose(file);
  
  vertex_hashtable_free(vh);

  return 1;
}
