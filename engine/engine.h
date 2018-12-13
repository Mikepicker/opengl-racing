#ifndef engine_h
#define engine_h

/* standard includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <math.h>

/* importer */
#include <limits.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

/* math */
#include "linmath.h"

/* include data structures */
#include "dict.h"

/* include data */
#include "data/object.h"
#include "data/material.h"
#include "data/camera.h"
#include "data/mesh.h"

#endif
