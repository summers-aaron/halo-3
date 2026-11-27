#ifndef PHYSICS_COLLISIONS_H
#define PHYSICS_COLLISIONS_H

#include "physics/collision_test_flags.h"

// Functions defined in tu/blamlibxbox/collisions.cpp.

struct collision_feature_list;
struct collision_result;
union real_point3d;
union real_vector3d;
struct s_collision_bsp_test_context;
struct s_collision_test_flags;
enum e_collision_result_type;

bool collision_test_line(s_collision_test_flags flags, const real_point3d *point0, const real_point3d *point1,
    long first_ignore_object_index, long second_ignore_object_index, collision_result *collision);
bool collision_test_point(s_collision_test_flags flags, const real_point3d *point, long first_ignore_object_index,
    long second_ignore_object_index, e_collision_result_type *result_type);
unsigned short collision_compute_cull_flags(s_collision_test_flags flags);

bool instanced_geometry_test_vector_internal(const s_collision_bsp_test_context *context, long instance_index, unsigned long flags, unsigned long bsp_flags,
    const real_point3d *point, const real_vector3d *vector, collision_result *result);
unsigned short collision_compute_object_cull_flags(long object_index);
bool collision_find_good_projectile_point(const real_point3d *known_point, const real_point3d *desired_point,
    real_point3d *good_point, long ignore_object_index, long unused);
const real_vector3d *collision_structure_ground_plane_calculate(const real_point3d *point, const real_vector3d *vector, float radius, real_vector3d *normal);
bool collision_get_features_in_sphere(unsigned __int64 test_flags, const real_point3d *point, float radius, float feature_radius, float height,
    long first_ignore_object_index, long second_ignore_object_index, collision_feature_list *features);
extern "C" bool collision_test_instanced_geometry_instance(long structure_bsp_index, long instance_index, unsigned long flags,
    const real_point3d *point, const real_vector3d *vector, collision_result *result);
bool instanced_geometry_test_point(const s_collision_bsp_test_context *context, long instance_index, unsigned long flags, const real_point3d *point);
bool collision_get_unobstructed_point(unsigned __int64 test_flags, const real_point3d *center, float radius,
    long first_ignore_object_index, long second_ignore_object_index, real_point3d *new_center, float *new_radius);
extern "C" bool object_test_point_internal(long object_index, bool pretested, s_collision_test_flags flags, const real_point3d *point,
    long first_ignore_object_index, long second_ignore_object_index);
bool object_test_vector(long object_index, s_collision_test_flags flags, const real_point3d *point, const real_vector3d *vector,
    long first_ignore_object_index, long second_ignore_object_index, collision_result *collision);
bool object_test_vector_internal(long object_index, bool test_children, s_collision_test_flags flags, unsigned long collision_model_flags,
    const real_point3d *point, const real_vector3d *vector, long first_ignore_object_index, long second_ignore_object_index,
    long ignore_index, collision_result *collision);

struct collision_bsp;
struct collision_bsp_test_vector_result;
struct real_matrix4x3;

bool get_bsp_from_collision_result(collision_result *result, const collision_bsp **bsp, const real_matrix4x3 **matrix);
void build_collision_result_from_bsp_result(collision_result *result, const collision_bsp_test_vector_result *bsp_result,
    const real_matrix4x3 *matrix);
#endif // PHYSICS_COLLISIONS_H
