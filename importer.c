#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "importer.h"

static const char* mmap_file(size_t* len, const char* filename) {
  FILE* f;
  long file_size;
  struct stat sb;
  char* p;
  int fd;

  (*len) = 0;

  f = fopen(filename, "r");
  fseek(f, 0, SEEK_END);
  file_size = ftell(f);
  fclose(f);

  fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return NULL;
  }

  if (fstat(fd, &sb) == -1) {
    perror("fstat");
    return NULL;
  }

  if (!S_ISREG(sb.st_mode)) {
    fprintf(stderr, "%s is not a file\n", "lineitem.tbl");
    return NULL;
  }

  p = (char*)mmap(0, (size_t)file_size, PROT_READ, MAP_SHARED, fd, 0);

  if (p == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  if (close(fd) == -1) {
    perror("close");
    return NULL;
  }

  (*len) = (size_t)file_size;

  return p;
}

static const char* get_file_data(size_t* len, const char* filename) {
  const char* ext = strrchr(filename, '.');

  size_t data_len = 0;
  const char* data = NULL;

  data = mmap_file(&data_len, filename);

  (*len) = data_len;
  return data;
}

void importer_load_obj(const char *filename, tinyobj_attrib_t *model_data)
{
  tinyobj_shape_t* shapes = NULL;
  size_t num_shapes;
  tinyobj_material_t* materials = NULL;
  size_t num_materials;

  size_t data_len= 0;
  const char* data = get_file_data(&data_len, filename);

  unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
  tinyobj_parse_obj(model_data, &shapes, &num_shapes, &materials, &num_materials, data, data_len, flags);
}
