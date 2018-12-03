#ifndef importer_h
#define importer_h

#include "engine.h"

#include "material.h"
#include "vertex_hashtable.h"

int importer_load_obj(const char *filename, vertex* out_vertices[], GLuint* out_indices[], GLuint* vertices_size, GLuint* indices_size);

#endif
