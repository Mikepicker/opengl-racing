#include "material.h"

void material_init(material* mat) {
  strcpy(mat->name, "\0");
  strcpy(mat->texture_path, "\0");
  strcpy(mat->normal_map_path, "\0");
  strcpy(mat->specular_map_path, "\0");
  mat->diffuse[0] = 1.0f;
  mat->diffuse[1] = 1.0f;
  mat->diffuse[2] = 1.0f;
  mat->specular[0] = 1.0f;
  mat->specular[1] = 1.0f;
  mat->specular[2] = 1.0f;
  mat->texture_subdivision = 1;
}
