#ifndef PHYSICS_SOFT_CEILING_TRIANGLE_TYPES_H
#define PHYSICS_SOFT_CEILING_TRIANGLE_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

struct s_soft_ceiling_triangle {
    real_plane3d plane;
    real_point3d center;
    float radius;
    real_point3d vertices[3];
};

// Sphere queries use the center and radius between the plane and vertices.
typedef char soft_ceiling_triangle_center_offset[(offsetof(s_soft_ceiling_triangle, center) == 0x10) ? 1 : -1];
typedef char soft_ceiling_triangle_radius_offset[(offsetof(s_soft_ceiling_triangle, radius) == 0x1c) ? 1 : -1];
typedef char soft_ceiling_triangle_size[(sizeof(s_soft_ceiling_triangle) == 0x44) ? 1 : -1];

#endif // PHYSICS_SOFT_CEILING_TRIANGLE_TYPES_H
