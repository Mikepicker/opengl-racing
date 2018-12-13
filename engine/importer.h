#ifndef importer_h
#define importer_h

#include "engine.h"

int importer_load_obj(const char *filename, mesh* out_meshes[], int* out_meshes_size);

#endif
