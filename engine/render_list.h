#ifndef render_list_h
#define render_list_h

#include "engine.h"
#include "data/object.h"

#define RENDER_LIST_SIZE 128

typedef struct {
  object* objects[RENDER_LIST_SIZE];
  int size;
} render_list;

render_list* render_list_new();
void render_list_add(render_list* rl, object* o);
void render_list_add_batch(render_list* rl, object** batch, int size);
void render_list_clear(render_list* rl);
void render_list_free(render_list* rl);

#endif
