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
  char* key;
} vertex_item;

int vertex_hashtable_init(int size);
void vertex_hashtable_resize(int new_size);
void vertex_hashtable_insert(char* key, vertex_indexed data);
vertex_item* vertex_hashtable_search(char* key);
vertex_item* vertex_hashtable_delete(char* key);

#endif
