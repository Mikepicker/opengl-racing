#include "debug.h"

void print4x4Mat(mat4x4 m) {
  for (int i = 0; i < 4; i++) {
    printf("%f %f %f %f\n", m[0][i], m[1][i], m[2][i], m[3][i]);
  }
  printf("------------\n");
}
