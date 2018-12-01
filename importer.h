#ifndef importer_h
#define importer_h

#include "engine.h"

#include "tinyobj_loader_c.h"
#include "material.h"

void importer_load_obj(const char *filename, tinyobj_attrib_t *model_data);

#endif
