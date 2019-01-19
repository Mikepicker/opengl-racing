#include "entities.h"

car* entities_new_car(vec3 pos) {
  car* c = malloc(sizeof(car));
  c->obj = importer_load_obj("assets/racing/raceCarRed.obj");
  c->obj->scale = 0.25;
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

void entities_update() {
  for (int i = 0; i < microdrag.num_cars; i++) {
    car* car = &microdrag.cars[i];
    car->speed = CAR_ROAD_SPEED;

    car->obj->position[0] += car->speed;
    vec3 y_axis = {0.0f, 1.0f, 0.0f};
    quat_rotate(car->obj->rotation, (float)glfwGetTime(), y_axis);

    for (int j = 0; j < EDITOR_MAX_PLACED_OBJECTS; j++) {
      object* track_piece = game_editor.placed_objects[j];
      if (track_piece != NULL && physics_objects_collide(car->obj, track_piece)) {
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
}
