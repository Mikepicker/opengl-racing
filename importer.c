#include "importer.h"

const int INIT_SIZE = 256;
const char* ASSETS_PATH = "assets/";

vec3* temp_vertices;
vec2* temp_uvs;
vec3* temp_normals;
GLuint* indices;

int vsize, vcount;
int vtsize, vtcount;
int vnsize, vncount;
int isize, icount;

int total_vertices_size, total_vertices;
vertex* vertices;

int meshes_size, meshes_count;
mesh* meshes;

static dict* import_mtl(const char* filename) {
  FILE* file = fopen(filename, "r");
  char line[256];
  dict* materials = dict_new(INIT_SIZE);

  material* current_mat = NULL;
  int first = 1;
  while (fgets(line, sizeof(line), file)) {

    // new material
    if (strstr(line, "newmtl") != NULL) {
      if (!first) {
        dict_insert(materials, current_mat->name, current_mat);
      }
      current_mat = (material*)malloc(sizeof(material));
      sscanf(line, "newmtl %s", current_mat->name);
    }
    // texture path
    else if (strstr(line, "map_Kd") != NULL) {
      sscanf(line, "map_Kd %s", current_mat->texture_path);
    }
  }

  if (current_mat != NULL) {
    dict_insert(materials, current_mat->name, current_mat);
  }
  printf("MAT1 %s %s\n", ((material*)dict_search(materials, current_mat->name))->name, ((material*)dict_search(materials, "Material"))->name);

  fclose(file);

  return materials;
}

static void push_index(vertex_hashtable* vh, const char* vkey) {
  // search face in the hashmap
  vertex_item* found = vertex_hashtable_search(vh, vkey);

  // get vertex indices (vertex, texcoords, normals)
  int v_index, vt_index, vn_index;
  sscanf(vkey, "%d/%d/%d", &v_index, &vt_index, &vn_index);
  v_index--; vt_index--; vn_index--;

  // get index from hashtable (or insert it if not present)
  if (found != NULL) {
    indices[icount] = found->data.index;
  } else {
    // push vertex
    vertices[total_vertices].x = temp_vertices[v_index][0];
    vertices[total_vertices].y = temp_vertices[v_index][1];
    vertices[total_vertices].z = temp_vertices[v_index][2];
    vertices[total_vertices].u = temp_uvs[vt_index][0];
    vertices[total_vertices].v = temp_uvs[vt_index][1];
    vertices[total_vertices].nx = temp_normals[vn_index][0];
    vertices[total_vertices].ny = temp_normals[vn_index][1];
    vertices[total_vertices].nz = temp_normals[vn_index][2];
    
    // update indices
    vertex_indexed vi = { total_vertices, vertices[total_vertices] };
    vertex_hashtable_insert(vh, vkey, vi);
    indices[icount] = total_vertices;

    total_vertices++;

    // Increase vertices size
    if (total_vertices >= total_vertices_size) {
      total_vertices_size *= 2;
      vertices = realloc(vertices, total_vertices_size * sizeof(vertex));
    }
  }
  icount++;
}

static void new_mesh() {
  // temp vertices
  vsize = INIT_SIZE;
  vcount = 0;
  temp_vertices = malloc(vsize * sizeof(vec3));

  // temp uvs
  vtsize = INIT_SIZE;
  vtcount = 0;
  temp_uvs = malloc(vtsize * sizeof(vec2));
  
  // temp normals
  vnsize = INIT_SIZE;
  vncount = 0;
  temp_normals = malloc(vnsize * sizeof(vec3));

  // temp indices
  isize = INIT_SIZE;
  icount = 0;
  indices = malloc(isize * sizeof(GLuint));

  // vertices to return
  total_vertices_size = INIT_SIZE;
  total_vertices = 0;
  vertices = malloc(total_vertices_size * sizeof(vertex));
}

int importer_load_obj(const char* filename, mesh* out_meshes[], int* out_meshes_size)
{
  /* parse vertices */
  FILE* file = fopen(filename, "r");
  char line[256];

  new_mesh();

  // meshes
  meshes_count = 0;
  meshes_size = INIT_SIZE;
  meshes = malloc(meshes_size * sizeof(mesh));

  // init hastable (it will be resized if needed)
  vertex_hashtable* vh = vertex_hashtable_new(INIT_SIZE);

  // materials dictionary
  dict* materials;

  int first_mesh = 1;
  while (fgets(line, sizeof(line), file)) {

    // realloc indices list, vertex hashmap
    if (icount + 3 >= isize) {
      isize = isize * 2;
      indices = realloc(indices, isize * sizeof(GLuint));
      vertex_hashtable_resize(vh, isize);
    }

    // new mesh
    if (strstr(line, "o ") != NULL) {
      if (first_mesh) {
        char mesh_name[256];
        sscanf(line, "o %s", mesh_name);
        strncpy(meshes[meshes_count].name, mesh_name, sizeof(mesh_name)-1);
        
        first_mesh = 0;
      } else {
        meshes[meshes_count].vertices = vertices;
        meshes[meshes_count].indices = indices;
        meshes[meshes_count].num_vertices = total_vertices;
        meshes[meshes_count].num_indices = icount;
        meshes_count++;

        new_mesh();
      }
    }

    // new vertex
    if (strstr(line, "v ") != NULL) {
      sscanf(line, "v %f %f %f", &temp_vertices[vcount][0], &temp_vertices[vcount][1], &temp_vertices[vcount][2]);
      vcount++;

      // realloc vertices list
      if (vcount >= vsize) {
        vsize *= 2;
        temp_vertices = realloc(temp_vertices, vsize * sizeof(vec3));
      }
    }

    // new texcoords
    if (strstr(line, "vt ") != NULL) {
      sscanf(line, "vt %f %f", &temp_uvs[vtcount][0], &temp_uvs[vtcount][1]);
      vtcount++;

      // realloc uvs list
      if (vtcount >= vtsize) {
        vtsize *= 2;
        temp_uvs = realloc(temp_uvs, vtsize * sizeof(vec2));
      }
    }

    // new normal
    if (strstr(line, "vn ") != NULL) {
      sscanf(line, "vn %f %f %f", &temp_normals[vncount][0], &temp_normals[vncount][1], &temp_normals[vncount][2]);
      vncount++;

      // realloc vertices list
      if (vncount >= vnsize) {
        vnsize *= 2;
        temp_normals = realloc(temp_normals, vnsize * sizeof(vec3));
      }
    }

    // new face
    if (strstr(line, "f ") != NULL) {

      // parse vertices of current face
      char v1[128];
      char v2[128];
      char v3[128];
      sscanf(line, "f %s %s %s", v1, v2, v3);

      // update indices
      push_index(vh, v1);
      push_index(vh, v2);
      push_index(vh, v3);
    }

    // load mtl
    if (strstr(line, "mtllib ") != NULL) {
      char mtl_name[128];
      sscanf(line, "mtllib %s\n", mtl_name);
      char mtl_path[128] = "assets/";
      strncat(mtl_path, mtl_name, 128);
      materials = import_mtl(mtl_path);
    }

    // use mtl
    if (strstr(line, "usemtl ") != NULL) {
      char mtl_name[128];
      sscanf(line, "usemtl %s\n", mtl_name);
      printf("MAT %s\n", ((material*)dict_search(materials, mtl_name))->name);
      meshes[meshes_count].mat = *((material*)dict_search(materials, mtl_name));
    }
  }

  // push last mesh
  meshes[meshes_count].vertices = vertices;
  meshes[meshes_count].indices = indices;
  meshes[meshes_count].num_vertices = total_vertices;
  meshes[meshes_count].num_indices = icount;
  meshes_count++;

  // return values
  *out_meshes = meshes;
  *out_meshes_size = meshes_count;

  fclose(file);

  vertex_hashtable_free(vh);
  free(temp_vertices);
  free(temp_uvs);
  free(temp_normals);

  return 1;
}
