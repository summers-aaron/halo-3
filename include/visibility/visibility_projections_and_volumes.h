#ifndef VISIBILITY_VISIBILITY_PROJECTIONS_AND_VOLUMES_H
#define VISIBILITY_VISIBILITY_PROJECTIONS_AND_VOLUMES_H

// Functions defined in tu/blamlibxbox/visibility_projections_and_volumes.cpp that other units call.

union real_point3d;
union real_rectangle2d;
union real_rectangle3d;
struct s_visibility_region;
struct visibility_projection;
struct visibility_volume;

bool visibility_volume_build(const visibility_projection *projection, long index, const real_rectangle2d *rectangle, visibility_volume *volume);
bool visibility_region_test_sphere(const s_visibility_region *region, long cluster_index, const real_point3d *center, float radius, bool *inside_out);
bool visibility_project_connected_points_clipped(const visibility_projection *projection, float margin,
    const real_point3d *world_points, long world_point_count, real_point3d *points_out, short *count_out,
    long maximum_points, real_rectangle3d *bounds, bool clip_near, bool clip_far, bool unused);

#include <vectorintrinsics.h>

extern "C" const float visibility_projections_and_volumes_minus_one;
extern const float visibility_projections_and_volumes_zero;
// Tests a sphere against six planes using VMX vector intrinsics.
extern "C" long visibility_volume_test_sphere_planes(const __vector4 *planes, const real_point3d *center, float radius);

#endif // VISIBILITY_VISIBILITY_PROJECTIONS_AND_VOLUMES_H
