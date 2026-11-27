#ifndef PHYSICS_RASTERIZER_VERTEX_TRANSPARENT_TYPES_H
#define PHYSICS_RASTERIZER_VERTEX_TRANSPARENT_TYPES_H

#include "math/matrix_math_types.h"
#include "math/real_math_types.h"

struct rasterizer_vertex_transparent {
    real_point3d position;
    float u, v;
    unsigned long color;
};

#endif // PHYSICS_RASTERIZER_VERTEX_TRANSPARENT_TYPES_H
