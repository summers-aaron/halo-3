#ifndef MATH_PLANE_INTERSECTIONS_H
#define MATH_PLANE_INTERSECTIONS_H

#include "math/real_math_types.h"

bool planes_try_and_get_intersection_point(const real_plane3d *plane0, const real_plane3d *plane1, const real_plane3d *plane2, real_point3d *point);
bool planes_try_and_get_intersection_line(const real_plane3d *plane0, const real_plane3d *plane1, real_point3d *line_point, real_vector3d *line_vector);

#endif // MATH_PLANE_INTERSECTIONS_H
