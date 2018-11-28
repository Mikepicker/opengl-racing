#ifndef importer_h
#define importer_h

#include "engine.h"

#include "tinyobj_loader_c.h"

void importer_load_obj(const char* filename, GLfloat* vertices[]);

#endif
