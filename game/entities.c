#include "entities.h"

car* entities_new_car(vec3 pos) {
  car* c = malloc(sizeof(car));
  c->obj = importer_load_obj("assets/racing/raceCarRed.obj");
  c->speed = CAR_ROAD_SPEED;
  vec3_copy(c->obj->position, pos);
  c->obj->box = physics_compute_aabb(c->obj);
  renderer_add_object(c->obj);
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

void entities_update(car* cars[], int size_cars, object* track_pieces[], int size_track_pieces) {
  for (int i = 0; i < size_cars; i++) {
    car* car = cars[i];
    car->speed = CAR_ROAD_SPEED;
    for (int j = 0; j < size_track_pieces; j++) {
      object* track_piece = track_pieces[j];
      if (track_piece == NULL) continue;
      mesh* m = physics_ray_hit_mesh(compute_car_ray(car), track_piece);
      if (m != NULL) {
        if (strcmp(m->mat.name, "grass") == 0) {
          car->speed = CAR_GRASS_SPEED;
        } else {
          car->speed = CAR_ROAD_SPEED;
        }
        break;
      } else {
        car->speed = CAR_ROAD_SPEED;
      }
    }
  }
}
