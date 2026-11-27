#ifndef PHYSICS_COLLISION_TEST_VECTOR_H
#define PHYSICS_COLLISION_TEST_VECTOR_H

#include "physics/collision_test_flags.h"

union real_point3d;
union real_vector3d;
struct collision_result;

#include "physics/collision_vector_query.h"

struct collision_plane;
extern "C" short collision_move_point_with_features(s_collision_test_flags flags, const real_point3d *point, const real_vector3d *delta,
    float radius, float margin, long first_ignore, long second_ignore, real_point3d *out_point, real_vector3d *out_delta,
    short maximum_planes, collision_plane *planes);

#endif // PHYSICS_COLLISION_TEST_VECTOR_H
