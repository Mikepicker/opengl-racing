#ifndef camera_h
#define camera_h

#include "../engine.h"

typedef struct
{
  vec3 front;
  vec3 up;
  vec3 pos;
  float speed;
} camera;

#endif
