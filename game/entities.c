#include "entities.h"

car* entities_new_car(vec3 pos, char* filename) {
  car* c = malloc(sizeof(car));
  c->obj = importer_load_obj(filename);
  c->accel = 0.0f;
  c->speed = 0.0f;
  c->obj->receive_shadows = 0;
  vec3_copy(c->obj->position, pos);
  c->obj->box = physics_compute_aabb(c->obj);
  renderer_add_object(c->obj);

  ALuint s;
  audio_add_source(&s, microdrag.sound_car);
  audio_loop_source(s, AL_TRUE);
  audio_play_source(s);
  c->obj->audio_source = s;

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

void entities_update() {
  for (int i = 0; i < microdrag.num_cars; i++) {
    car* car = &microdrag.cars[i];

    vec3 y_axis = {0.0f, 1.0f, 0.0f};
    //quat_rotate(car->obj->rotation, (float)glfwGetTime(), y_axis);

    // rotation
    vec4 front = { 0.0f, 0.0f, -1.0f, 1.0f };
    vec4 vel;
    mat4x4 m;
    mat4x4_from_quat(m, car->obj->rotation);
    mat4x4_mul_vec4(vel, m, front);
    vec4_norm(vel, vel);

    // speed
    car->speed += car->accel * microdrag.delta_time;
    if (fabsf(car->speed) > CAR_MAX_SPEED) car->speed = (car->speed / fabsf(car->speed)) * CAR_MAX_SPEED;

    // damping
    if (car->accel == 0) {
      car->speed -= car->speed * 0.01f;
    }

    // collide with track
    int on_grass = 1;
    for (int j = 0; j < EDITOR_MAX_PLACED_OBJECTS; j++) {
      object* track_piece = game_editor.placed_objects[j];
      if (track_piece != NULL && physics_objects_collide(car->obj, track_piece)) {
        mesh* m = physics_ray_hit_mesh(compute_car_ray(car), track_piece);
        if (m != NULL) {
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

  }
}
