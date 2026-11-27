#ifndef PHYSICS_HAVOK_UTILITIES_H
#define PHYSICS_HAVOK_UTILITIES_H

// Functions defined in tu/blamlibxbox/havok_utilities.cpp that other units call.

class c_global_material_type;
class phys_box_volume;
class phys_mat3;
class phys_rigid_body;
class phys_shape;
class phys_transform;
class phys_vec4;
struct real_matrix3x3;
struct real_matrix4x3;
union real_point3d;
union real_vector3d;
struct s_havok_ray_test;
struct phys_hit_point;
struct s_location;
enum e_havok_group;

bool calculate_shape_fixed_position(const phys_shape *shape, long object_index, e_havok_group group,
    const real_matrix4x3 *matrix, const real_vector3d *velocity, real_point3d *position,
    s_location *location, float minimum_search_radius, float maximum_search_radius, bool search_everywhere,
    const real_point3d *raycast_origin, long raycast_object_index);

void calculate_model_region_and_permutation_from_physics_model_region_and_permutation(
    long object_index, long model_index, long physics_region, long physics_permutation, long *model_region, long *model_permutation);

// match: 0x25b608: noalias preserves collector output stores across this call.
extern "C" __declspec(noalias) bool havok_try_and_get_cd_point_hit(bool ignore_surface_side, bool calculate_material_type,
    bool collide_with_structure, long ignore_component_index, const phys_hit_point &point, float maximum_distance,
    float *out_distance, unsigned char *out_conveyor, real_vector3d *out_velocity, real_point3d *out_point,
    real_vector3d *out_normal, c_global_material_type *out_material_type, long *out_component_index,
    long *out_rigid_body_index, long *out_structure_bsp_index, long *out_surface_flags);

void phys_mat3_from_matrix3x3(class phys_mat3 * _sym_p0, struct real_matrix3x3 const * matrix);
bool havok_ray_cast_in_world(s_havok_ray_test *ray_test, long filter_info, long context, long hit_index);
bool havok_ray_cast_in_aabb_phantom(long component_index, s_havok_ray_test *ray_test);
bool havok_ray_cast_in_aabb_phantom(phys_box_volume *aabb_phantom, s_havok_ray_test *ray_test, long context, long unused_context);
void havok_object_halt_keyframed_rigid_bodies(long object_index);
void phys_transform_from_matrix4x3(phys_transform *transform, const real_matrix4x3 *matrix);
long havok_object_compute_rigid_body_index_of_node(long object_index, long node_index);
float calculate_acceleration_magnitude(bool clamp_to_infinity, bool scale_by_time, float time, float target,
    float acceleration, float velocity, float max_velocity, float friction, float exponent, bool instant, float *scale);
bool calculate_closest_point_and_normal_on_havok_component(long component_index, const real_point3d *origin,
    real_point3d *closest_point, real_vector3d *normal);
bool havok_calculate_sphere_vs_triangle_collision(const real_point3d *center, float radius,
    const real_point3d *a, const real_point3d *b, const real_point3d *c, real_vector3d *normal, float *distance);
float calculate_havok_shape_bounding_sphere_radius(const phys_shape *shape, float *minimum_dimension,
    float *largest_shape_dimension, long *convex_shape_count, long *phantom_shape_count);
void havok_object_keyframed_rigid_bodies_to_node_transforms(long object_index, bool reset);
bool calculate_havok_shape_properties_recursive(const phys_shape *shape, const phys_rigid_body *rigid_body,
    const real_point3d *point, unsigned int key, float *friction, float *restitution, c_global_material_type *material);
extern "C" int havok_get_object_collision_group(long object_index, long state);
extern "C" void calculate_havok_shape_properties(const phys_shape *shape, const phys_rigid_body *rigid_body,
    const real_point3d *position, unsigned long packed_surface, float *friction, float *restitution,
    c_global_material_type *material);


// An hkVector4 holds a point in its first three lanes.
inline real_point3d *real_point3d_from_hkVector4_in_place(phys_vec4 *v)
{
    return reinterpret_cast<real_point3d *>(v);
}

// Data defined in tu/blamlibxbox/havok_utilities.cpp that other units use.

class c_havok_component;

extern "C" bool havok_ray_cast(c_havok_component *, long, long, const real_matrix4x3 *, const real_vector3d *, long, long *, real_matrix4x3 *);
extern "C" const real_vector3d k_fixed_position_search_directions[];

bool simple_fix_havok_object_position(long object_index, long raycast_object_index, float minimum_distance, e_havok_group group);

extern "C" bool havok_try_and_get_sphere_fixed_position(float radius, float height, e_havok_group group,
    long collision_flags, long ignore_object_index,
    const real_point3d *point, const real_point3d *direction, long ignore_index, real_point3d *fixed_point, bool test_only);

#endif // PHYSICS_HAVOK_UTILITIES_H
