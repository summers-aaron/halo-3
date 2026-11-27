#ifndef OBJECTS_LIGHT_INTERMEDIATE_GEOMETRY_TYPES_H
#define OBJECTS_LIGHT_INTERMEDIATE_GEOMETRY_TYPES_H

#include "math/real_math_types.h"

struct s_light_intermediate_geometry {
    // Unreferenced gap: March names intermediate.valid (+0x18) and intermediate_frustum->length
    // but nothing at +0x38, and 0x17c6b0 never loads or stores it.
    long pad;
    union {
        struct {
            float near_radius;
            float far_radius;
            float radius;
        } sphere;
        struct {
            real_vector3d direction;
            float length;
            real_vector2d slope;
            float distance;
            real_point2d near_extent;
            float near_offset;
            float far_offset;
            real_point2d far_extent;
            real_point3d far_point;
        } frustum;
    };
};

#endif // OBJECTS_LIGHT_INTERMEDIATE_GEOMETRY_TYPES_H
