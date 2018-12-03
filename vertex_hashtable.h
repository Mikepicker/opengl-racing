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

typedef struct {
  vertex_item** hash_array;
  unsigned int size;
} vertex_hashtable;

vertex_hashtable* vertex_hashtable_new(int size);
void vertex_hashtable_resize(vertex_hashtable* vh, int new_size);
void vertex_hashtable_insert(vertex_hashtable* vh, const char* key, vertex_indexed data);
vertex_item* vertex_hashtable_search(vertex_hashtable* vh, const char* key);
vertex_item* vertex_hashtable_delete(vertex_hashtable* vh, const char* key);
void vertex_hashtable_free(vertex_hashtable* vh);

#endif
