#ifndef OBJECTS_LIGHT_GEOMETRY_TYPES_H
#define OBJECTS_LIGHT_GEOMETRY_TYPES_H

#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "objects/light_intermediate_geometry_types.h"

struct light_geometry {
    long type;
    float near_distance;
    float far_distance;
    float field_of_view;
    float near_width;
    float far_width;
    bool valid;
    real_vector3d left;
    float origin_offset;
    real_point3d origin;
    s_light_intermediate_geometry intermediate;
};

#endif // OBJECTS_LIGHT_GEOMETRY_TYPES_H
