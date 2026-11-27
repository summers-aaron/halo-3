#ifndef MATH_GEOMETRY_H
#define MATH_GEOMETRY_H

// Functions defined in tu/blamlibxbox/geometry.cpp that other units call.

union real_point3d;

void geometry_compute_points_minimum_bounding_sphere(real_point3d *points, long point_count, real_point3d *sphere_center, float *sphere_radius);

#endif // MATH_GEOMETRY_H
