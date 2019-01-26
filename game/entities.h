#ifndef entities_h
#define entities_h

#include "../engine/steve.h"
#include "game.h"
#include "editor.h"
#include "data/car.h"

car* entities_new_car(vec3 pos, char* filename);
void entities_update();

#endif
