#include "vertex_hashtable.h"

static unsigned long hash(const char *str)
{
  unsigned long hash = 5381;
  unsigned int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

static unsigned int hash_code(vertex_hashtable* vh, const char* key) {
  return hash(key) % vh->size;
}

vertex_hashtable* vertex_hashtable_new(int size) {
  vertex_hashtable* vh = malloc(sizeof(vertex_hashtable));
  vh->hash_array = malloc(size * sizeof(vertex_item*));
  for (int i = 0; i < size; i++) {
    vh->hash_array[i] = NULL;
  }
  vh->size = size;
  return vh;
}

void vertex_hashtable_resize(vertex_hashtable* vh, int new_size) {
  vh->hash_array = realloc(vh->hash_array, new_size * sizeof(vertex_item*));
  for (int i = vh->size; i < new_size; i++) {
    vh->hash_array[i] = NULL;
  }
  vh->size = new_size;
}

vertex_item* vertex_hashtable_search(vertex_hashtable* vh, const char* key) {
  //get the hash
  unsigned int hash_index = hash_code(vh, key);

  //move in array until an empty
  while (vh->hash_array[hash_index] != NULL) {
    if (strcmp(vh->hash_array[hash_index]->key, key) == 0) {
      return vh->hash_array[hash_index];
    }

    //go to next cell
    ++hash_index;

    //wrap around the table
    hash_index %= vh->size;
  }

  return NULL;
}

void vertex_hashtable_insert(vertex_hashtable* vh, const char* key, vertex_indexed data)
{
  vertex_item *item = (vertex_item *)malloc(sizeof(vertex_item));
  item->data = data;
  strncpy(item->key, key, sizeof(item->key));

  //get the hash
  unsigned int hash_index = hash_code(vh, key);

  // TODO: check if removed
  //move in array until an empty or different key
  while (vh->hash_array[hash_index] != NULL && strcmp(key, vh->hash_array[hash_index]->key) != 0)
  {
    //go to next cell
    ++hash_index;

    //wrap around the table
    hash_index %= vh->size;
  }

  vh->hash_array[hash_index] = item;
}

vertex_item* vertex_hashtable_delete(vertex_hashtable* vh, const char* key)
{
  //get the hash
  unsigned int hash_index = hash_code(vh, key);

  //move in array until an empty
  while (vh->hash_array[hash_index] != NULL)
  {
    if (strcmp(vh->hash_array[hash_index]->key, key) == 0)
    {
      vertex_item* temp = vh->hash_array[hash_index];

      //assign a dummy item at deleted position
      vh->hash_array[hash_index] = NULL;
      return temp;
    }

    //go to next cell
    ++hash_index;

    //wrap around the table
    hash_index %= vh->size;
  }

  return NULL;
}

void vertex_hashtable_free(vertex_hashtable* vh) {
  for (unsigned int i = 0; i < vh->size; i++) {
    free(vh->hash_array[i]);
  }
  free(vh->hash_array);
  free(vh);
}
