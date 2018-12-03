#include "vertex_hashtable.h"

vertex_item** hash_array;
int hash_size;
// vertex_item *dummy_item;

static unsigned long hash(const char *str)
{
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

static int hash_code(const char* key) {
  return hash(key) % hash_size;
}

int vertex_hashtable_init(int size) {
  if (size < 0) {
    return -1;
  }
  hash_array = malloc(size * sizeof(vertex_item*));
  for (int i = 0; i < size; i++) {
    hash_array[i] = NULL;
  }
  hash_size = size;
  return 0;
}

void vertex_hashtable_resize(int new_size) {
  hash_array = realloc(hash_array, new_size * sizeof(vertex_item*));
  for (int i = hash_size; i < new_size; i++) {
    hash_array[i] = NULL;
  }
  hash_size = new_size;
}

vertex_item* vertex_hashtable_search(const char* key) {
  //get the hash
  int hash_index = hash_code(key);

  //move in array until an empty
  while (hash_array[hash_index] != NULL) {
    if (strcmp(hash_array[hash_index]->key, key) == 0) {
      return hash_array[hash_index];
    }

    //go to next cell
    ++hash_index;

    //wrap around the table
    hash_index %= hash_size;
  }

  return NULL;
}

void vertex_hashtable_insert(const char* key, vertex_indexed data)
{
  vertex_item *item = (vertex_item *)malloc(sizeof(vertex_item));
  item->data = data;
  strncpy(item->key, key, sizeof(item->key));

  //get the hash
  int hash_index = hash_code(key);

  // TODO: check if removed
  //move in array until an empty or different key
  while (hash_array[hash_index] != NULL && strcmp(key, hash_array[hash_index]->key) != 0)
  {
    //go to next cell
    ++hash_index;

    //wrap around the table
    hash_index %= hash_size;
  }

  hash_array[hash_index] = item;
}

vertex_item* vertex_hashtable_delete(const char* key)
{
  //get the hash
  int hash_index = hash_code(key);

  //move in array until an empty
  while (hash_array[hash_index] != NULL)
  {
    if (strcmp(hash_array[hash_index]->key, key) == 0)
    {
      vertex_item* temp = hash_array[hash_index];

      //assign a dummy item at deleted position
      hash_array[hash_index] = NULL;
      return temp;
    }

    //go to next cell
    ++hash_index;

    //wrap around the table
    hash_index %= hash_size;
  }

  return NULL;
}

/* int main()
{
  vertex_hashtable_init(200);

  vertex v1 = {
    1.0f,
    2.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f
  };

  vertex v2 = {
      1.0f,
      2.0f,
      2.0f,
      0.0f,
      0.0f,
      2.0f,
      0.0f,
      0.0f};

  vertex_indexed vi1 = {
    0,
    v1
  };

  vertex_indexed vi2 = {
    1,
    v2
  };
  vertex_hashtable_insert("ciao", vi1);
  vertex_hashtable_insert("ciccio", vi2);

  vertex_item* item = vertex_hashtable_search("ciao");

  if (item != NULL)
  {
    printf("Element found: %f %f %f\n", item->data.v.x, item->data.v.y, item->data.v.z);
  }
  else
  {
    printf("Element not found\n");
  }

  item = vertex_hashtable_search("ciccio");

  if (item != NULL)
  {
    printf("Element found: %f %f %f\n", item->data.v.x, item->data.v.y, item->data.v.z);
  }
  else
  {
    printf("Element not found\n");
  }

  vertex_hashtable_delete("ciccio");
  item = vertex_hashtable_search("ciccio");

  if (item != NULL)
  {
    printf("Element found: %f %f %f\n", item->data.v.x, item->data.v.y, item->data.v.z);
  }
  else
  {
    printf("Element not found\n");
  }
} */
