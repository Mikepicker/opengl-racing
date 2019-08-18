#ifndef suspension_h
#define suspension_h

#include "../../engine/steve.h"

#define SUSPENSION_STIFFNESS 50
#define SUSPENSION_DAMPING 15

typedef struct {
  float x;
  float v;
} suspension;

#endif
