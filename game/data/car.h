#ifndef car_h
#define car_h

#include "../../engine/steve.h"
#include "suspension.h"

#define CAR_ACCEL 0.1f
#define CAR_TURN_RATE 2.0f
#define CAR_MAX_SPEED 0.7f
#define CAR_MAX_SPEED_GRASS 0.05f
#define CAR_FRAME_LONGITUDINAL_LENGTH 1.0
#define CAR_FRAME_TRANSVERSAL_LENGTH 1.0
#define CAR_CG_TO_REAR_AXLE_DISTANCE 0.45
#define CAR_CG_TO_FRONT_AXLE_DISTANCE 0.55
#define WEIGHT_TRANSFER_FACTOR 1.0
#define CAR_CG_HEIGHT 0.4
#define CAR_MASS 100.0

#define GRAVITY_ACC 0.1

typedef struct {
  object* obj;
  vec3 dir;
  float accel;
  float speed;
  float steering_wheel_angle;
  float steering_command;
  float roll;
  float pitch;
  float yaw;
  suspension suspension_fl;
  suspension suspension_fr;
  suspension suspension_rl;
  suspension suspension_rr;
  float weight_transfer_front;
  float weight_transfer_rear;
  float weight_transfer_left;
  float weight_transfer_right;
  int current_lap;
  int last_piece_index;
} car;

#endif
