#ifndef entities_h
#define entities_h

#include "../engine/steve.h"
#include "data/car.h"

car* entities_new_car(vec3 pos);
void entities_update(car* cars[], int size_cars, object* track_pieces[], int size_track_pieces);

#endif
