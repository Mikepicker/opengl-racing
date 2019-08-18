#include <math.h>
#include "entities.h"

car* entities_new_car(vec3 pos, char* filename) {
  car* c = malloc(sizeof(car));
  c->obj = importer_load_obj(filename);
  c->accel = 0.0f;
  c->speed = 0.0f;
  vec3_copy(c->obj->position, pos);
  c->current_lap = 0;
  c->last_piece_index = 0;

  physics_compute_aabb(c->obj);
  renderer_init_object(c->obj);
  audio_init_object(c->obj, microdrag.sound_car, 1);

  return c;
}

static ray compute_car_ray(car* c) {
  vec3 p = { c->obj->center[0], c->obj->center[1], c->obj->center[2] };
  object_vec3_to_object_space(c->obj, p);
  ray r;
  vec3 ray_dir = { 0, -1.0f, 0 };
  vec3_copy(r.o, p);
  vec3_copy(r.dir, ray_dir);
  r.length = 10;
  return r;
}

void suspension_update(suspension* s, float force) {
    // input
    s.x += s.v * microdrag.delta_time;
    s.v += (-SUSPENSION_DAMPING*s.v-SUSPENSION_STIFFNESS*s.x+force) * microdrag.delta_time;
}

// we use the rear-wheel driving kinematic model
void car_update(car* car) {
    // forward speed: keyboard input and nonlinear damping
    car->speed += ( car->accel - fabsf((car->speed)*(car->speed)) * 0.001f ) * microdrag.delta_time;
    if (car->accel == 0) { // this might not be necessary at all
      car->speed -= (car->speed * 0.005f) * microdrag.delta_time;
    }

    // steering wheel angle integration from keyboard input
    car->steering_wheel_angle += 0.1f * ( car->steering_command - * 0.005f * car->steering_wheel_angle ) * microdrag.delta_time;

    // car orientation in the plane (yaw)
    car->yaw += ( tanf(car->steering_wheel_angle)/CAR_FRAME_LONGITUDINAL_LENGTH ) * microdrag.delta_time;

    // car attitude from suspension displacement
    car->pitch=asinf((car->suspension_fl.x + car->suspension_fr.x - car->suspension_rl.x - car->suspension_rr.x)/2.0);
    car->roll=asinf((car->suspension_fr.x + car->suspension_rr.x - car->suspension_fl.x - car->suspension_rl.x)/2.0);

}

// weight transfer: formulas online from Car Physics for Games by Marco Monster
void weight_transfer_on_suspensions(car *car) {

    // forward motion
    car->weight_transfer_front = (CAR_CG_TO_FRONT_AXLE_DISTANCE/CAR_FRAME_LONGITUDINAL_LENGTH)*CAR_MASS*0.1 - (CAR_CG_HEIGHT/CAR_FRAME_LONGITUDINAL_LENGTH)*CAR_MASS*car->accel;
    car->weight_transfer_rear = (CAR_CG_TO_REAR_AXLE_DISTANCE/CAR_FRAME_LONGITUDINAL_LENGTH)*CAR_MASS*0.1 + (CAR_CG_HEIGHT/CAR_FRAME_LONGITUDINAL_LENGTH)*CAR_MASS*car->accel;

    // rotational motion
    float lateral_acceleration = car->speed * tanf(car->steering_wheel_angle)/CAR_FRAME_LONGITUDINAL_LENGTH;
    car->weight_transfer_left = - (CAR_CG_HEIGHT/CAR_FRAME_LONGITUDINAL_LENGTH)*CAR_MASS*lateral_acceleration;
    car->weight_transfer_right = - (CAR_CG_HEIGHT/CAR_FRAME_LONGITUDINAL_LENGTH)*CAR_MASS*lateral_acceleration;
}
 
void entities_update() {
  for (int i = 0; i < microdrag.num_cars; i++) {
    car* car = &microdrag.cars[i];

    vec3 y_axis = {0.0f, 1.0f, 0.0f};
    //quat_rotate(car->obj->rotation, (float)glfwGetTime(), y_axis);

    // rotation
    vec4 front = { 0.0f, 0.0f, -1.0f, 1.0f };
    vec4 vel;
    mat4x4 m;

    quat_from_rpy(car->obj->rotation,cat->roll,cat->pitch,cat->yaw);
    mat4x4_from_quat(m, car->obj->rotation);
    mat4x4_mul_vec4(vel, m, front);
    vec4_norm(vel, vel);

    // weight transfer on suspesions
    weight_transfer_on_suspensions(car* car);

    //suspensions
    suspension_update(car->suspension_fl,car->weight_transfer_front + car->weight_transfer_left);
    suspension_update(car->suspension_fr,car->weight_transfer_front + car->weight_transfer_right);
    suspension_update(car->suspension_rl,car->weight_transfer_rear + car->weight_transfer_left);
    suspension_update(car->suspension_rr,car->weight_transfer_rear + car->weight_transfer_right);

    // forward speed saturation   
    if (fabsf(car->speed) > CAR_MAX_SPEED) car->speed = (car->speed / fabsf(car->speed)) * CAR_MAX_SPEED;
    
    // car kinematic model
    update_car(car);

    // collide with track
    int on_grass = 1;
    for (int j = 0; j < EDITOR_MAX_PLACED_OBJECTS; j++) {
      object* track_piece = game_editor.placed_objects[j];
      if (track_piece != NULL && physics_objects_collide(car->obj, track_piece)) {
        mesh* m = physics_ray_hit_mesh(compute_car_ray(car), track_piece);
        if (m != NULL) {

          // advance piece
          if ((car->last_piece_index + 1) % game_editor.placed_count == j) {
            car->last_piece_index = j;
          }

          if (strcmp(m->mat.name, "grass") != 0) {
            on_grass = 0;
          }
          break;
        }
      }
    }
    
    if (on_grass) {
      if (fabsf(car->speed) > CAR_MAX_SPEED_GRASS)
        car->speed = (car->speed / fabsf(car->speed)) * CAR_MAX_SPEED_GRASS;
    }

    // apply velocity
    vec4_scale(vel, vel, car->speed);
    vec3_add(car->obj->position, car->obj->position, vel);

    // audio
    audio_move_source(car->obj->audio_source, car->obj->position);
    audio_source_playing(car->obj->audio_source);

    // color track pieces
    editor_color_car_pieces(&microdrag.cars[0], &microdrag.cars[1]);
  }
}
