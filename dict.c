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

dict* dict_new(int size) {
  dict *h = calloc(1, sizeof(dict));
  h->keys = calloc(size, sizeof(char*));
  for (int i = 0; i < size; i++) {
    h->keys[i] = NULL;
  }
  h->values = calloc(size, sizeof(void*));
  h->size = size;
  return h;
}

int dict_index(dict* h, const char key[256]) {
  unsigned int i = hash(key) % h->size;
  while (h->keys[i] != NULL && strcmp(h->keys[i], key) != 0) {
    i = (i + 1) % h->size;
  }
  return i;
}

void dict_insert(dict* h, const char key[256], void* value) {
  int i = dict_index(h, key);
  h->keys[i] = malloc(256 * sizeof(char));
  strncpy(h->keys[i], key, 256 * sizeof(char));
  h->values[i] = value;
}

void* dict_search(dict* h, const char key[256]) {
  int i = dict_index(h, key);
  return h->values[i];
}

void dict_free(dict* h) {
  for (int i = 0; i < h->size; i++) {
    free(h->keys[i]);
    free(h->values[i]);
  }
  free(h);
}
