#ifndef dict_h
#define dict_h

#include "engine.h"

typedef struct {
    int size;
    char** keys;
    void** values;
} dict;
 
dict *dict_new(int size); 
int dict_index(dict* h, const char key[256]); 
void dict_insert(dict* h, const char key[256], void* value); 
void* dict_search(dict* h, const char key[256]);
void dict_free(dict* h);

#endif
