#include "dict.h"

static unsigned long hash(const char str[256])
{
  unsigned long hash = 5381;
  unsigned int c;

  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return hash;
}

dict* dict_new(int capacity) {
  dict *h = calloc(1, sizeof(dict));
  h->keys = calloc(capacity, sizeof(char*));
  h->values = calloc(capacity, sizeof(void*));
  for (int i = 0; i < capacity; i++) {
    h->keys[i] = NULL;
    h->values[i] = NULL;
  }
  h->capacity = capacity;
  h->size = 0;
  return h;
}

int dict_index(dict* h, const char key[256]) {
  unsigned int i = hash(key) % h->capacity;
  while (h->keys[i] != NULL && strcmp(h->keys[i], key) != 0) {
    i = (i + 1) % h->capacity;
  }
  return i;
}

void dict_insert(dict* h, const char key[256], void* value) {
  // insert element
  int i = dict_index(h, key);
  h->keys[i] = malloc(256 * sizeof(char));
  strncpy(h->keys[i], key, 256 * sizeof(char));
  h->values[i] = value;
  h->size++;

  // resize
  if (h->size >= h->capacity) {
    int new_capacity = (int)(h->capacity * 1.5f);
    h->keys = realloc(h->keys, new_capacity * sizeof(char*));
    h->values = realloc(h->values, new_capacity * sizeof(void*));
    for (int i = h->capacity; i < new_capacity; i++) {
      h->keys[i] = NULL; 
      h->values[i] = NULL;
    }
    h->capacity = new_capacity;
  }
}

void* dict_search(dict* h, const char key[256]) {
  int i = dict_index(h, key);
  return h->values[i];
}

void dict_free(dict* h) {
  for (int i = 0; i < h->capacity; i++) {
    free(h->keys[i]);
    free(h->values[i]);
  }
  free(h->keys);
  free(h->values);
  free(h);
}
