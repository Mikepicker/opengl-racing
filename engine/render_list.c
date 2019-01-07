#include "render_list.h"

render_list* render_list_new() {
  render_list* rl = malloc(sizeof(render_list));
  for (int i = 0; i < RENDER_LIST_SIZE; i++)
    rl->objects[i] = NULL;
  rl->size = 0;
  return rl;
}

void render_list_add(render_list* rl, object* o) {
  rl->objects[rl->size++] = o; 
}

void render_list_add_batch(render_list* rl, object** batch, int size) {
  for (int i = 0; i < size; i++)
    rl->objects[rl->size++] = batch[i];
}

void render_list_clear(render_list* rl) {
  for (int i = 0; i < rl->size; i++)
    rl->objects[i] = NULL;
  rl->size = 0;
}

void render_list_free(render_list* rl) {
  free(rl);
}
