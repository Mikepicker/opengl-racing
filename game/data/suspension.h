#ifndef suspension_h
#define suspension_h

#include "../../engine/steve.h"

#define SUSPENSION_STIFFNESS 70
#define SUSPENSION_DAMPING 3

typedef struct {
  float x;
  float v;
} suspension;

#endif
