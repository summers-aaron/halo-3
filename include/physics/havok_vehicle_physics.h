#ifndef PHYSICS_HAVOK_VEHICLE_PHYSICS_H
#define PHYSICS_HAVOK_VEHICLE_PHYSICS_H

#include "math/matrix_math_types.h"
#include "game/materials.h"
#include "tag_files/tag_types.h"
#include <stddef.h>
#include "cseries/cseries_wrapped.h"

struct s_havok_vehicle_anti_gravity_point_definition
{
    long marker_name;
    unsigned long flags;
    float antigrav_strength;
    float antigrav_offset;
    float antigrav_height;
    float antigrav_damp_factor;
    float antigrav_normal_k1;
    float antigrav_normal_k0;
    float radius;
    // Reserved bytes in the serialized anti-gravity point definition.
    char pad24[0x32 - 0x24];
    short damage_source_region_index;
    long damage_source_region_name;
    float runtime_damage_state_errors[5];
};

enum e_friction_point_definition_flags
{
    _friction_point_definition_flag0,
    _friction_point_definition_flag1,
    _friction_point_definition_flag2,
    _friction_point_definition_flag3,
    _friction_point_definition_flag4,
    _friction_point_definition_flag5
};

struct s_havok_vehicle_friction_point_definition
{
    long marker_name;
    union
    {
        unsigned long flags;
        c_flags_no_init<e_friction_point_definition_flags, unsigned long, 6> m_flags;
    };
    float fraction_of_total_mass;
    float radius;
    float damaged_radius;
    short friction_type;
    float moving_friction_velocity_diff;
    float e_brake_moving_friction;
    float e_brake_friction;
    float e_brake_moving_friction_vel_diff;
    // Reserved bytes in the serialized friction point definition.
    char pad28[0x3c - 0x28];
    long collision_global_material_name;
    c_global_material_type collision_global_material_index;
    short destroyed_model_state;
    long region_name;
    long destroyed_runtime_region_index;
};

struct s_havok_vehicle_physics_definition
{
    long flags;
    float static_friction;
    float spring_constant;
    float spring_damping;
    float dynamic_friction;
    float maximum_k;
    float minimum_k;
    float maximum_friction_acceleration;
    float anti_gravity_modulation;
    float steering_scale;
    float gravity_scale;
    float radius;
    float maximum_update_distance;
    float maximum_update_period;
    float maximum_remote_update_period;
    s_tag_block anti_gravity_points;
    s_tag_block friction_points;
    s_tag_block shapes;
    signed char player_training_vehicle_type;
    signed char size;
    signed char complex_suspension_sample_count;
    union
    {
        struct { float minimum_flipping_angular_velocity; float maximum_flipping_angular_velocity; };
        real_bounds flipping_angular_velocity;
    };
    float crouch_transition_time;
};

struct s_havok_vehicle_anti_gravity_point
{
    real_matrix4x3 matrix;
    real_point3d marker_position;
    real_vector3d position_offset;
    real_vector3d velocity;
    real_vector3d force;
    real_vector3d torque;
    char pad70[0x80 - 0x70];
    long local_physics_object_index;
    bool active;
    bool is_root_node;
    real_point3d node_position;
    real_vector3d spring_force;
    float strength;
    float oscillation;
    float compression;
};

struct s_friction_vehicle_physics_point_datum
{
    real_matrix4x3 matrix;
    real_point3d marker_position;
    real_vector3d position_offset;
    real_vector3d velocity;
    real_vector3d force;
    real_vector3d torque;
    unsigned char flag;
    real_vector3d contact_velocity;
    long object_index;
    bool grounded;
    bool sliding;
    bool destroyed;
    bool braking;
    float compression;
    float current_radius;
    float normal_force;
    float forward_velocity;
    float lateral_speed;
    real_vector3d spring_force;
    real_vector3d normal;
    real_point3d position;
    long surface_index;
    c_global_material_type material;
    char padc6[1];
    bool use_surface_material;
    char padc8[0xec - 0xc8];
};

struct s_havok_vehicle_physics_instance
{
    long object_index;
    s_havok_vehicle_physics_definition *definition;
    real_matrix4x3 world_matrix;
    real_matrix3x3 inertia_tensor;
    real_point3d center_of_mass;
    float mass;
    float load_torque;
    real_vector3d throttle;
    float steering;
    bool destroyed;
    bool upside_down;
    unsigned char update_flag;
    long previous_active_anti_gravity_point_count;
    long previous_grounded_friction_point_count;
    s_havok_vehicle_anti_gravity_point anti_gravity_points[16];
    s_friction_vehicle_physics_point_datum friction_points[16];
};

typedef char havok_vehicle_anti_gravity_point_definition_size[
    (sizeof(s_havok_vehicle_anti_gravity_point_definition) == 0x4c) ? 1 : -1];
typedef char havok_vehicle_anti_gravity_point_region_offset[
    (offsetof(s_havok_vehicle_anti_gravity_point_definition, damage_source_region_index) == 0x32) ? 1 : -1];
typedef char havok_vehicle_friction_point_definition_size[
    (sizeof(s_havok_vehicle_friction_point_definition) == 0x4c) ? 1 : -1];
typedef char havok_vehicle_physics_definition_size[
    (sizeof(s_havok_vehicle_physics_definition) == 0x70) ? 1 : -1];
typedef char havok_vehicle_anti_gravity_point_size[
    (sizeof(s_havok_vehicle_anti_gravity_point) == 0xac) ? 1 : -1];
typedef char havok_vehicle_friction_point_size[
    (sizeof(s_friction_vehicle_physics_point_datum) == 0xec) ? 1 : -1];
typedef char havok_vehicle_physics_instance_size[
    (sizeof(void *) != 4 || sizeof(s_havok_vehicle_physics_instance) == 0x1a10) ? 1 : -1];
typedef char havok_vehicle_physics_world_matrix_offset[
    (sizeof(void *) != 4 || offsetof(s_havok_vehicle_physics_instance, world_matrix) == 0x08) ? 1 : -1];
typedef char havok_vehicle_physics_inertia_tensor_offset[
    (sizeof(void *) != 4 || offsetof(s_havok_vehicle_physics_instance, inertia_tensor) == 0x3c) ? 1 : -1];
typedef char havok_vehicle_physics_center_of_mass_offset[
    (sizeof(void *) != 4 || offsetof(s_havok_vehicle_physics_instance, center_of_mass) == 0x60) ? 1 : -1];
typedef char havok_vehicle_physics_mass_offset[
    (sizeof(void *) != 4 || offsetof(s_havok_vehicle_physics_instance, mass) == 0x6c) ? 1 : -1];
typedef char havok_vehicle_physics_load_torque_offset[
    (sizeof(void *) != 4 || offsetof(s_havok_vehicle_physics_instance, load_torque) == 0x70) ? 1 : -1];
typedef char havok_vehicle_physics_throttle_offset[
    (sizeof(void *) != 4 || offsetof(s_havok_vehicle_physics_instance, throttle) == 0x74) ? 1 : -1];
typedef char havok_vehicle_physics_anti_gravity_offset[
    (sizeof(void *) != 4 || offsetof(s_havok_vehicle_physics_instance, anti_gravity_points) == 0x90) ? 1 : -1];
typedef char havok_vehicle_physics_friction_offset[
    (sizeof(void *) != 4 || offsetof(s_havok_vehicle_physics_instance, friction_points) == 0xb50) ? 1 : -1];
typedef char havok_vehicle_friction_material_offset[
    (offsetof(s_friction_vehicle_physics_point_datum, material) == 0xc4) ? 1 : -1];
typedef char havok_vehicle_friction_surface_material_offset[
    (offsetof(s_friction_vehicle_physics_point_datum, use_surface_material) == 0xc7) ? 1 : -1];
typedef char havok_vehicle_friction_reserved_tail_offset[
    (offsetof(s_friction_vehicle_physics_point_datum, padc8) == 0xc8) ? 1 : -1];
typedef char havok_vehicle_anti_gravity_local_physics_object_offset[
    (offsetof(s_havok_vehicle_anti_gravity_point, local_physics_object_index) == 0x80) ? 1 : -1];
typedef char havok_vehicle_anti_gravity_node_position_offset[
    (offsetof(s_havok_vehicle_anti_gravity_point, node_position) == 0x88) ? 1 : -1];
typedef char havok_vehicle_friction_contact_velocity_offset[
    (offsetof(s_friction_vehicle_physics_point_datum, contact_velocity) == 0x74) ? 1 : -1];
typedef char havok_vehicle_physics_previous_active_count_offset[
    (sizeof(void *) != 4 || offsetof(s_havok_vehicle_physics_instance, previous_active_anti_gravity_point_count) == 0x88)
        ? 1 : -1];

bool havok_vehicle_physics_instance_new(
    s_havok_vehicle_physics_instance *instance, long object_index, bool destroyed, bool force_update);
void havok_vehicle_physics_update(long object_index, s_havok_vehicle_physics_instance *instance, float damping,
    bool disable_slip_friction, const real_vector3d *magic_force, const real_vector3d *magic_torque,
    const real_point3d *force_position);

#endif // PHYSICS_HAVOK_VEHICLE_PHYSICS_H
