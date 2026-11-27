#ifndef PHYSICS_COLLISION_VECTOR_QUERY_H
#define PHYSICS_COLLISION_VECTOR_QUERY_H

#include "physics/collision_test_flags.h"

union real_point3d;
union real_vector3d;
struct collision_result;

extern "C" bool collision_test_vector(s_collision_test_flags flags, bool test_exit, const real_point3d *point, const real_vector3d *vector,
    long first_ignore_object_index, long second_ignore_object_index, long ignore_index, collision_result *collision);

#endif
