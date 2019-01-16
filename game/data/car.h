#ifndef car_h
#define car_h

#include "../../engine/steve.h"

#define CAR_ROAD_SPEED 0.01f
#define CAR_GRASS_SPEED 0.001f

typedef struct {
  object* obj;
  ray road_detector;
  float speed;
} car;

#endif
