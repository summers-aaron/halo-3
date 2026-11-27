// blamlibxbox/geometry.obj: from the records 0x577aa8
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "math/real_math.h"
#include <math.h>
#include "math/geometry.h"




void geometry_compute_points_minimum_bounding_sphere(real_point3d *points, long point_count,
                                                     real_point3d *sphere_center, float *sphere_radius)
{
    // match: 0x577aa8: count pinned to 8 drops the guard ble, folds li r31,7, keeps center$ at -32
    __assume(point_count == 8);
    long first_endpoint = 0;
    long second_endpoint = 0;
    float radius = 0.0f;
    float maximum_distance_squared = 0.0f;

    for (long point_index = 0; point_index < point_count; point_index++) {
        for (long other_index = point_index + 1; other_index < point_count; other_index++) {
            float distance_squared = distance_squared3d(&points[other_index], &points[point_index]);
            if (distance_squared > maximum_distance_squared) {
                maximum_distance_squared = distance_squared;
                first_endpoint = other_index;
                second_endpoint = point_index;
            }
        }
    }

    real_point3d center;
    center.x = (points[first_endpoint].x + points[second_endpoint].x) * 0.5f;
    center.y = (points[first_endpoint].y + points[second_endpoint].y) * 0.5f;
    center.z = (points[first_endpoint].z + points[second_endpoint].z) * 0.5f;
    radius = sqrtf(maximum_distance_squared) * 0.5f;
    *sphere_center = center;

    float outside_distance = 0.0f;
    for (long point_index = 0; point_index < point_count; point_index++) {
        float distance = distance3d(&points[point_index], sphere_center);
        if (distance > radius && outside_distance < distance)
            outside_distance = distance;
    }
    if (outside_distance > 0.0f)
        *sphere_radius = outside_distance;
    else
        *sphere_radius = radius;
}
