#include "debug.h"

void debug_print_mat4x4(mat4x4 m) {
  for (int i = 0; i < 4; i++) {
    printf("%f %f %f %f\n", m[0][i], m[1][i], m[2][i], m[3][i]);
  }
  printf("------------\n");
}

void debug_print_vec3(vec3 v) {
  printf("%f %f %f\n", v[0], v[1], v[2]);
  printf("------------\n");
}

void debug_print_vec4(vec4 v) {
  printf("%f %f %f %f\n", v[0], v[1], v[2], v[3]);
  printf("------------\n");
}
