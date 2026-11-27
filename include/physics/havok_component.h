#ifndef PHYSICS_HAVOK_COMPONENT_H
#define PHYSICS_HAVOK_COMPONENT_H

// Functions defined in tu/blamlibxbox/havok_component.cpp.

class phys_world_object;
struct s_data_array;
extern s_data_array *g_havok_component_data;

long havok_component_new(long object_index);
long havok_entity_get_havok_component_index(class phys_world_object const * _sym_p0);
long havok_entity_get_havok_component_rigid_body_index(class phys_world_object const * _sym_p0);
long havok_entity_get_structure_bsp_index(class phys_world_object const * _sym_p0);
void havok_entity_set_havok_component_index(phys_world_object *world_object, long havok_component_index);
void havok_entity_set_structure_bsp_index(phys_world_object *world_object, long structure_bsp_index);
void havok_component_construct(long havok_component_index);
extern "C" void havok_muffin_update(long muffin_index, float time);
extern "C" bool entity_belongs_in_phantom(long havok_component_index, long body_havok_component_index, long body_rigid_body_index, long phantom_index);
extern "C" void s3tc_compute_dxt5a_palette(unsigned short *colors, unsigned char color_a, unsigned char color_b);

class phys_motor;
struct s_physics_model_instance;
struct s_physics_model_motor_reference;

phys_motor *build_motor_from_motor_reference(const s_physics_model_instance *instance, const s_physics_model_motor_reference *reference, float *power);

class c_havok_component;
class phys_mat3;
class phys_rigid_body;
class phys_shape;
class phys_transform;
class phys_vec4;
union real_point3d;
union real_vector3d;

extern "C" void havok_component_update_body_transforms(c_havok_component *component, long havok_component_index, bool unused, bool keep_rotation);
extern "C" void havok_component_get_point_velocity(c_havok_component *component, long rigid_body_index, real_vector3d *velocity, const real_point3d *point);
extern "C" phys_rigid_body *havok_component_create_rigid_body(c_havok_component *component, const phys_shape *shape, float deactivation_scale,
    bool keep_wrapped_shape, const phys_vec4 *center_of_mass, float friction, float restitution, const phys_mat3 *inertia_tensor,
    float mass, long motion_type, const phys_transform *transform, long havok_group, bool use_physics_model, bool unused_proxy_flag, float linear_damping,
    float angular_damping);
extern "C" void havok_component_update_phantom_state(c_havok_component *component, bool force);
extern "C" void havok_component_activate_rigid_bodies(c_havok_component *component, unsigned char restart);
extern "C" int havok_component_get_collision_group( c_havok_component *component, const s_physics_model_instance *instance, long size,
    long motion_type, long rigid_body_index);

extern "C" {
extern const float k_maximum_collector_distance;
}

#endif // PHYSICS_HAVOK_COMPONENT_H
