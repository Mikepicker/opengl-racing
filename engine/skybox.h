#ifndef skybox_h
#define skybox_h

#include "engine.h"

typedef struct {
  unsigned int vao;
  unsigned int vbo;
  unsigned int texture_id;
  const char* faces[6];
} skybox;

void skybox_init(skybox* s, const char* faces[]);
void skybox_free(skybox* s);

#endif
