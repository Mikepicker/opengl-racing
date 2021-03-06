#ifndef car_h
#define car_h

#include "../../engine/steve.h"

#define CAR_ACCEL 0.1f
#define CAR_TURN_RATE 2.0f
#define CAR_MAX_SPEED 0.7f
#define CAR_MAX_SPEED_GRASS 0.05f

typedef struct {
  object* obj;
  vec3 dir;
  float accel;
  float speed;
  int current_lap;
  int last_piece_index;
} car;

#endif
