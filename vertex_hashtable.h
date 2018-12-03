#ifndef vertex_hashtable_h
#define vertex_hashtable_h

#include "engine.h"
#include "object.h"

typedef struct {
  int index;
  vertex v;
} vertex_indexed;

typedef struct {
  vertex_indexed data;
  char key[256];
} vertex_item;

int vertex_hashtable_init(int size);
void vertex_hashtable_resize(int new_size);
void vertex_hashtable_insert(const char* key, vertex_indexed data);
vertex_item* vertex_hashtable_search(const char* key);
vertex_item* vertex_hashtable_delete(const char* key);

#endif
