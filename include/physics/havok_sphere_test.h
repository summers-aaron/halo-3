#ifndef PHYSICS_HAVOK_SPHERE_TEST_H
#define PHYSICS_HAVOK_SPHERE_TEST_H
#include "physics/physics_constants_tls.h"

struct s_havok_sphere_query;
struct s_havok_sphere_test;
struct s_havok_sphere_test_cache;

// Legacy scalar view retained until the vehicle full-image operand-rank residue is resolved.

extern "C" void havok_calculate_features_in_simple_shape_phantom(long component_index, float radius,
    const s_havok_sphere_query *queries, s_havok_sphere_test *tests,
    long count, s_havok_sphere_test_cache *cache, bool replay);

#endif
