#ifndef importer_h
#define importer_h

#include "engine.h"
#include "data/material.h"
#include "data/mesh.h"
#include "vertex_hashtable.h"
#include "dict.h"

int importer_load_obj(const char *filename, mesh* out_meshes[], int* out_meshes_size);

#endif
