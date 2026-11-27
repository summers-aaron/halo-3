#ifndef PHYSICS_COLLISION_TEST_VECTOR_RAW_H
#define PHYSICS_COLLISION_TEST_VECTOR_RAW_H
union real_point3d;
union real_vector3d;
struct collision_result;
// Legacy packed flag ABI used by the biped queries.
extern "C" bool collision_test_vector(unsigned __int64 flags, bool test_exit, const real_point3d *point, const real_vector3d *vector,
    long first_ignore_object_index, long second_ignore_object_index, long ignore_index, collision_result *collision);
#endif
