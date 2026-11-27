// blamlibxbox/havok_vehicle_physics.obj: from the records 0xf1308 0xf2778 0x577fe8 0x578140 0x579700 0x5798c8 0x5799c0
// @flags /TP /O2 /Oc
#include "physics/physics_constants_tls.h"
#include "physics/havok_ray_test_types.h"
#include "tag_files/tag_groups.h"
#include <string.h>
#include "memory/tls_globals.h"
#include "math/matrix_math.h"
#include "game/game_globals_storage.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "objects/object_placement.h"
#include "phys/phys_dynamics.h"
#include "objects/objects.h"
#include "math/periodic_functions.h"
#include "physics/havok_vehicle_physics.h"
#include "physics/havok_vehicle_physics_definitions.h"
#include "physics/physics_constants.h"
#include "game/game_time.h"
#include "objects/object_early_movers.h"
#include "physics/impact_array_types.h"
#include "game/materials.h"
#include "physics/havok_component.h"

#include "units/vehicle_datum.h"
#include "units/vehicle_definitions.h"
#include "physics/havok_component_class.h"
#include "memory/data_typed_access.h"
#include "memory/data_types.h"
#include "physics/impact_contact.h"
#include "game/player_machine.h"
#include "physics/havok_utilities.h"
#include "physics/havok_vehicle_physics_internals.h"
#include "physics/havok_sphere_test.h"
#include "physics/havok_math.h"
#include "memory/datum_index.h"

// match: 0xf2778: retain the validated-byte ternary while objects.cpp owns the shared out-of-line getter.
static __forceinline short havok_vehicle_physics_controlling_body_index(const c_havok_component *component)
{
    return VALID_INDEX(component->m_object_postion_controlling_rigid_body_index, component->get_rigid_body_count())
        ? component->m_object_postion_controlling_rigid_body_index : -1;
}

__forceinline void c_impact::update_vehicle_impact(
    long impact_index, const s_contact_description *contact_description, const real_vector3d *effect_direction, float volume, bool vehicle)
{
    if (!contact_description->disconnected_from_physics)
    {
        object_position = contact_description->normal;
        origin = contact_description->position;
        vehicle_sound_volume_a = 0.0f;
        vehicle_sound_volume_b = volume;
        velocity_direction = *effect_direction;
        vehicle_impact = vehicle;
    }
}
inline void c_havok_component::get_rigid_body_inertia_tensor(long rigid_body_index, real_matrix3x3 *matrix)
{
    phys_rigid_body *body = get_rigid_body(rigid_body_index);
    phys_rotation inertia;
    phys_motion *motion = body->get_rigid_motion();
    motion->get_inertia_local(inertia);
    real_vector3d_from_hkVector4(&matrix->forward, &inertia.get_column(0));
    real_vector3d_from_hkVector4(&matrix->left, &inertia.get_column(1));
    real_vector3d_from_hkVector4(&matrix->up, &inertia.get_column(2));
}
inline void c_havok_component::get_rigid_body_inverse_inertia_tensor(long rigid_body_index, real_matrix3x3 *matrix)
{
    phys_rigid_body *body = get_rigid_body(rigid_body_index);
    phys_rotation inertia;
    phys_motion *motion = body->get_rigid_motion();
    motion->get_inv_inertia_local(inertia);
    real_vector3d_from_hkVector4(&matrix->forward, &inertia.get_column(0));
    real_vector3d_from_hkVector4(&matrix->left, &inertia.get_column(1));
    real_vector3d_from_hkVector4(&matrix->up, &inertia.get_column(2));
}



static __forceinline c_havok_component *havok_vehicle_physics_component_datum_get(long index)
{
    return (c_havok_component *)(g_havok_component_data->data + (unsigned short)index * sizeof(c_havok_component));
}

static __forceinline c_havok_component *havok_vehicle_physics_contact_component_datum_get(long index)
{
    return (c_havok_component *)(g_havok_component_data->data + (index & 0xffff) * sizeof(c_havok_component));
}

static __forceinline c_impact *havok_vehicle_physics_impact_datum_get(long impact_index)
{
    return (c_impact *)(TLS_IMPACT_DATA->data + (impact_index & 0xffff) * 180);
}

// @address 0x5798c8 ?havok_vehicle_physics_remove_impacts_for_friction_point_index@@YAXPBUs_havok_vehicle_physics_instance@@J_N@Z
void havok_vehicle_physics_remove_impacts_for_friction_point_index(
    const s_havok_vehicle_physics_instance *havok_vehicle_physics_instance, long friction_point_index, bool disconnected_from_physics)
{
    object_datum *object = object_get(havok_vehicle_physics_instance->object_index);
    c_havok_component *component = havok_vehicle_physics_component_datum_get(object->havok_component);
    if (!disconnected_from_physics) {
        for (long index = 0; index < component->get_impact_count(); ++index) {
            long impact_index = datum_get_typed<s_impact_array>(TLS_IMPACT_ARRAY_DATA, component->m_impact_array_index)->impact_indices[index];
            c_impact *impact = havok_vehicle_physics_impact_datum_get(impact_index);
            if (impact->physics_reference_count > 0 && impact->vehicle_friction_point_index == friction_point_index)
                impact->remove_reference(impact_index, false);
        }
    }
}

// @address 0x5799c0 ?compute_ground_plane@@YAXPAUs_havok_vehicle_physics_instance@@PBUs_havok_sphere_test@@J@Z
void compute_ground_plane(s_havok_vehicle_physics_instance *instance, const s_havok_sphere_test *test, long friction_point_index)
{
    s_friction_vehicle_physics_point_datum *point = &instance->friction_points[friction_point_index];
    point->normal = *global_up3d;
    point->position = *global_origin3d;
    point->material = k_global_material_type_default;
    point->compression = 0.0f;
    if (test->hit)
    {
        point->material = test->material;
        float radius = point->current_radius;
        point->normal = test->normal;
        float depth = radius - test->distance;
        point->compression = MIN(MAX(depth, 0.001f), radius);
        point->surface_index = -1;
        point->flag = test->flag;
        point->contact_velocity = test->contact_velocity;
        point->object_index = test->object_index;
        point->position = test->position;
    }
}

// @address 0x579700 ?havok_vehicle_physics_update_contact_description@@YAJPAUs_havok_vehicle_physics_instance@@PBUs_contact_description@c_impact@@PBTreal_vector3d@@M_N@Z
long havok_vehicle_physics_update_contact_description(s_havok_vehicle_physics_instance *havok_vehicle_physics_instance,
    const c_impact::s_contact_description *contact_description, const real_vector3d *effect_direction, float volume, bool vehicle)
{
    object_datum *object = object_get(havok_vehicle_physics_instance->object_index);
    c_havok_component *component = havok_vehicle_physics_component_datum_get(object->havok_component);
    long impact_index = component->find_impact_which_includes_contact(contact_description, false);
    if (impact_index == -1)
    {
        float score = c_impact::calculate_score(&contact_description->position, c_impact::state1);
        havok_vehicle_physics_remove_impacts_for_friction_point_index(havok_vehicle_physics_instance,
            contact_description->vehicle_friction_point_index, contact_description->disconnected_from_physics);
        if (component->make_space_for_new_impact(score, -1))
        {
            impact_index = impact_new_from_contact(contact_description, 1);
            c_impact *impact = havok_vehicle_physics_impact_datum_get(impact_index);
            component->add_impact(impact_index);
            impact->add_reference(impact_index);
        }
    }
    if (impact_index != -1)
    {
        c_impact *impact = havok_vehicle_physics_impact_datum_get(impact_index);
        impact->update_vehicle_impact(impact_index, contact_description, effect_direction, volume, vehicle);
    }
    return impact_index;
}

static __forceinline bool havok_vehicle_physics_is_distributed(long simulation_type)
{
    bool result = false;
    if (simulation_type >= 4 && simulation_type <= 5)
        result = true;
    return result;
}
static __forceinline bool havok_vehicle_physics_is_locally_controlled(unsigned long object_index, const s_object_header *headers)
{
    const unit_datum *unit = (const unit_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
    bool result = false;
    if (havok_vehicle_physics_is_distributed(game_globals->options.simulation_type) && ((1 << unit->object.object_type) & 0x1003) != 0)
    {
        long player_index = unit->unit.player_index;
        if (player_index == -1)
        {
            long primary_player_index = -1;
            if (unit->unit.driver_object_index != -1)
            {
                const unit_datum *primary = (const unit_datum *)headers[datum_index_to_absolute_index(unit->unit.driver_object_index)].datum;
                primary_player_index = primary->unit.player_index;
            }
            player_index = primary_player_index;
        }
        if (player_index != -1)
            result = player_is_on_local_machine(player_index);
    }
    return result;
}
static __forceinline unsigned char havok_vehicle_physics_should_use_default(
    bool use_default, const unit_datum *unit, const s_object_header *headers)
{
    long has_actor;
    if (unit->unit.driver_object_index == -1 ||
        ((const unit_datum *)headers[datum_index_to_absolute_index(unit->unit.driver_object_index)].datum)->unit.actor_index != -1)
        has_actor = true;
    else
        has_actor = false;
    return use_default | has_actor;
}
// Returns the region records and their permutation arrays.
static __forceinline void havok_vehicle_physics_object_get_region_information(
    long object_index, long *region_count, char **permutations, char **unknown, object_region_information **regions)
{
    object_datum *object = object_get(object_index);
    long size = object->region_information.size;
    char *base = (char *)object_header_block_get(object, &object->region_information);
    *region_count = size;
    if ((object->flags >> 11) & 1)
        *region_count = size - 16;
    *region_count = *region_count / 10;
    if (regions)
        *regions = (object_region_information *)base;
    if (permutations)
        *permutations = base + *region_count * 8;
    if (unknown)
        *unknown = base + *region_count * 9;
}
// @address 0x577fe8 havok_vehicle_physics_get_update_scale
extern "C" float havok_vehicle_physics_get_update_scale(unsigned long object_index)
{
    long simulation_type = game_globals->options.simulation_type;
    const s_object_header *headers = (const s_object_header *)object_header_data->data;
    const unit_datum *unit = (const unit_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
    const vehicle_definition *definition =
        (const vehicle_definition *)global_tag_instances[datum_index_to_absolute_index(unit->object.definition_index)].definition;
    bool use_default;
    if (havok_vehicle_physics_is_distributed(simulation_type))
        use_default = !havok_vehicle_physics_is_locally_controlled(object_index, headers);
    else
        use_default = !object_is_or_contains_player(object_index);
    if (havok_vehicle_physics_should_use_default(use_default, unit, headers))
        return definition->havok_vehicle_physics.maximum_remote_update_period;
    return definition->havok_vehicle_physics.maximum_update_period;
}
// @address 0xf1308 ?havok_vehicle_physics_instance_new@@YA_NPAUs_havok_vehicle_physics_instance@@J_N1@Z
bool havok_vehicle_physics_instance_new(s_havok_vehicle_physics_instance *instance, long object_index, bool destroyed, bool force_update)
{
    union real_rectangle3d aabb;
    game_time_globals_definition *time_globals = TLS_GAME_TIME_GLOBALS;
    vehicle_datum *object = OBJECT_GET(object_index, vehicle_datum, 2);
    vehicle_definition *vehicle = (vehicle_definition *)tag_get('vehi', object->object.definition_index);
    s_havok_vehicle_physics_definition *definition = &vehicle->havok_vehicle_physics;
    // match: 0xf1308: pre-existing definition hint; removal changes the address-operand tie.
    BLAM_ASSUME(definition != NULL);
    // match: 0xf1308: pre-existing object hint; removal changes the address-operand tie.
    BLAM_ASSUME(object != NULL);
    long update_period = (long)(havok_vehicle_physics_get_update_scale(object_index) * (float)time_globals->ticks_per_second + 0.5f);
    bool result = false;
    long update_offset = game_time_add_water_ripple(object_index, update_period, (float)definition->friction_points.count);
    bool update = object->vehicle.friction_update_time == -1 || time_globals->time - object->vehicle.friction_update_time >= update_period;
    instance->update_flag = update;
    long staggered;
    if (update_offset != -1 && (time_globals->time + update_offset) % update_period == 0)
        staggered = true;
    else
        staggered = false;
    instance->update_flag = update | staggered;
    if (object->object.havok_component != -1)
    {
        c_havok_component *component = havok_vehicle_physics_component_datum_get(object->object.havok_component);
        if ((definition->flags & 1) == 0 && component->get_rigid_body_count() > 0)
        {
            if (!component->get_rigid_body_has_infinite_inertia_tensor(0))
            {
                component->get_rigid_body_inertia_tensor(0, &instance->inertia_tensor);
                component->get_rigid_body_center_of_mass(0, &instance->center_of_mass);
                instance->object_index = object_index;
                instance->definition = definition;
                instance->mass = component->get_rigid_body_mass(0);
                instance->load_torque = 0.0f;
                instance->destroyed = destroyed;
                instance->upside_down = false;
                instance->previous_active_anti_gravity_point_count = 0;
                instance->previous_grounded_friction_point_count = 0;
                object_get_world_matrix(object_index, &instance->world_matrix);
                instance->steering = 0.0f;
                instance->throttle = object->unit.throttle;
                memset(instance->anti_gravity_points, 0,
                    definition->anti_gravity_points.count * sizeof(s_havok_vehicle_anti_gravity_point));
                memset(instance->friction_points, 0, definition->friction_points.count * sizeof(s_friction_vehicle_physics_point_datum));
                long region_count;
                object_region_information *regions;
                havok_vehicle_physics_object_get_region_information(object_index, &region_count, NULL, NULL, &regions);
                for (long index = 0; index < definition->anti_gravity_points.count; index++)
                {
                    object_marker markers[1];
                    s_havok_vehicle_anti_gravity_point_definition *point_definition =
                        (s_havok_vehicle_anti_gravity_point_definition *)tag_block_get_element_with_size(
                            &definition->anti_gravity_points, index, sizeof(s_havok_vehicle_anti_gravity_point_definition));
                    internal_object_get_markers_by_string_id(object_index, point_definition->marker_name, markers, 1, false);
                    instance->anti_gravity_points[index].local_physics_object_index = -1;
                    memcpy(&instance->anti_gravity_points[index], &instance->world_matrix, sizeof(real_matrix4x3));
                    instance->anti_gravity_points[index].marker_position = markers[0].matrix.position;
                    instance->anti_gravity_points[index].node_position = markers[0].node_to_marker.position;
                    instance->anti_gravity_points[index].is_root_node = !markers[0].node_index;
                }
                for (long index = 0; index < definition->friction_points.count; index++)
                {
                    object_marker markers[1];
                    s_havok_vehicle_friction_point_definition *point_definition =
                        (s_havok_vehicle_friction_point_definition *)tag_block_get_element_with_size(
                            &definition->friction_points, index, sizeof(s_havok_vehicle_friction_point_definition));
                    internal_object_get_markers_by_string_id(object_index, point_definition->marker_name, markers, 1, false);
                    instance->friction_points[index].object_index = -1;
                    memcpy(&instance->friction_points[index], &instance->world_matrix, sizeof(real_matrix4x3));
                    instance->friction_points[index].marker_position = markers[0].matrix.position;
                    instance->friction_points[index].destroyed = point_definition->m_flags.test(_friction_point_definition_flag4) && instance->destroyed;
                    instance->friction_points[index].current_radius = point_definition->radius;
                    if (point_definition->m_flags.test(_friction_point_definition_flag5)
                        && point_definition->destroyed_runtime_region_index != -1
                        && regions[point_definition->destroyed_runtime_region_index].state >= point_definition->destroyed_model_state)
                        instance->friction_points[index].use_surface_material = true;
                    }
                if (component->m_simple_shape_phantom)
                {
                    phys_transform transform;
                    phys_transform_from_matrix4x3(&transform, &instance->world_matrix);
                    component->m_simple_shape_phantom->set_transform(transform);
                }
                if (havok_vehicle_physics_get_phantom_aabb(definition, &aabb))
                    component->move_aabb_phantom(&instance->world_matrix, &aabb);
                result = true;
            }
        }
    }
    return result;
}

#define HAVOK_VEHICLE_PHYSICS_ABSOLUTE(value) ((value) >= 0.0f ? (value) : -(value))

static __forceinline float havok_vehicle_physics_absolute(float value)
{
    float magnitude = value;
    float absolute_value = value >= 0.0f ? magnitude : -magnitude;
    return absolute_value;
}

static float havok_vehicle_physics_pin_fraction(float value, float minimum, float maximum)
{
    float fraction = havok_vehicle_physics_absolute(maximum - minimum);
    if (fraction < 0.001f
        || (fraction = havok_vehicle_physics_absolute(minimum - maximum)) < 0.001f)
    {
        fraction = value <= minimum ? 0.0f : 1.0f;
    }
    else if (minimum < maximum)
    {
        if (value <= minimum)
            fraction = 0.0f;
        else if (value >= maximum)
            fraction = 1.0f;
        else
            fraction = (value - minimum) / (maximum - minimum);
    }
    else
    {
        if (value <= maximum)
            fraction = 1.0f;
        else if (value >= minimum)
            fraction = 0.0f;
        else
            fraction = (minimum - value) / (minimum - maximum);
    }
    return fraction;
}

static const float k_havok_vehicle_physics_maximum_mass_scale = 16.0f;
static float havok_vehicle_physics_maximum_mass_scale()
{
    return k_havok_vehicle_physics_maximum_mass_scale;
}

static __forceinline float havok_vehicle_physics_contact_dot_product3d(const real_vector3d *velocity, const real_vector3d *normal)
{
    float x_product = velocity->i * normal->i;
    float z_product = velocity->k * normal->k;
    float y_product = velocity->j * normal->j;
    return y_product + (z_product + x_product);
}

static __forceinline void havok_vehicle_physics_add_friction_rpm(float *rpm, float magnitude)
{
    *rpm += magnitude;
}

static __forceinline float havok_vehicle_physics_friction_magnitude_squared(const real_vector3d *vector)
{
    return vector->k * vector->k + vector->j * vector->j + vector->i * vector->i;
}

static __forceinline void havok_vehicle_physics_accumulate_anti_gravity_force(real_vector3d *force, const real_vector3d *addition,
    float *vertical_force)
{
    force->i += addition->i;
    force->j += addition->j;
    *vertical_force += addition->k;
}

// match: 0x578140: tuned against the old cross_product3d (one more local, i); 6 words differ without it
static __forceinline real_vector3d *havok_vehicle_physics_cross_product3d(const real_vector3d *a, const real_vector3d *b,
    real_vector3d *result)
{
    float i = a->j * b->k - a->k * b->j;
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}

// @address 0x578140
bool havok_vehicle_physics_compute(s_havok_vehicle_physics_instance *instance, bool disable_slip_friction, real_vector3d *force,
    real_vector3d *torque, const real_point3d *force_position)
{
    BLAM_ASSUME(force != 0);
    vehicle_datum *object = (vehicle_datum *)object_get_and_verify_type(instance->object_index, -1);
    c_havok_component *component = havok_vehicle_physics_component_datum_get(object->object.havok_component);
    s_havok_vehicle_physics_definition *physics = instance->definition;
    s_havok_sphere_test_cache *cache = (s_havok_sphere_test_cache *)object_header_block_get(&object->object, &object->vehicle.suspension);
    real_vector3d linear_velocity;
    real_vector3d angular_velocity;
    real_vector3d early_mover_linear_velocity;
    real_vector3d early_mover_angular_velocity;
    object_get_velocities(instance->object_index, &linear_velocity, &angular_velocity);
    if (object_get_early_mover_local_space_velocity(instance->object_index, &instance->world_matrix.position, &early_mover_linear_velocity,
            &early_mover_angular_velocity, true, false))
    {
        subtract_vectors3d(&linear_velocity, &early_mover_linear_velocity, &linear_velocity);
        subtract_vectors3d(&angular_velocity, &early_mover_angular_velocity, &angular_velocity);
    }
    *force = *global_zero_vector3d;
    *torque = *global_zero_vector3d;
    if (!instance->upside_down)
    {
        for (long index = 0; index < physics->anti_gravity_points.count; index++)
        {
            s_havok_vehicle_anti_gravity_point *point = &instance->anti_gravity_points[index];
            const float &throttle = instance->throttle.j;
            s_havok_vehicle_anti_gravity_point_definition *definition =
                (s_havok_vehicle_anti_gravity_point_definition *)tag_block_get_element_with_size(
                    &physics->anti_gravity_points, index, sizeof(s_havok_vehicle_anti_gravity_point_definition));
            vector_from_points3d(&instance->center_of_mass, force_position ? force_position : &point->marker_position,
                &point->position_offset);
            havok_vehicle_physics_cross_product3d(&angular_velocity, &point->position_offset, &point->velocity);
            add_vectors3d(&point->velocity, &linear_velocity, &point->velocity);
            float offset = definition->radius;
            float height = definition->antigrav_height;
            float node_height = point->node_position.y;
            const float &steering_scale = physics->steering_scale;
            float steering = PIN(steering_scale * instance->steering / k_pi, -1.0f, 1.0f) * (instance->throttle.i >= 0.0f ? -1.0f : 1.0f);
            float throttle_force = -throttle * node_height;
            float steering_force = (steering != 0.0f ? steering < 0.0f ? -1 : 1 : 0) * (steering * steering) * node_height;
            bool use_throttle = HAVOK_VEHICLE_PHYSICS_ABSOLUTE(throttle_force) > HAVOK_VEHICLE_PHYSICS_ABSOLUTE(steering_force);
            float dead_zone = 0.1f;
            float modulation = use_throttle ? throttle_force : steering_force;
            modulation = modulation > 0.0f ? PIN(modulation - dead_zone, 0.0f, 1.0f) : PIN(modulation + dead_zone, -1.0f, 0.0f);
            offset *= 1.0f + modulation * physics->anti_gravity_modulation;
            height *= 1.0f + modulation * physics->anti_gravity_modulation;
            s_havok_ray_test ray_test;
            scale_vector3d(global_down3d, offset + height, &ray_test.vector);
            ray_test.start = point->marker_position;
            if (point->is_root_node && havok_ray_cast_in_aabb_phantom(object->object.havok_component, &ray_test))
            {
                float distance = (offset + height) * ray_test.fraction - offset;
                float slope_fraction = havok_vehicle_physics_pin_fraction(point->matrix.up.k, definition->antigrav_normal_k0,
                    definition->antigrav_normal_k1);
                float compression = distance > 0.0f ? 1.0f - distance / height : 1.0f;
                float normal_velocity = havok_vehicle_physics_contact_dot_product3d(&point->velocity, &ray_test.normal);
                float spring_force = point->strength * definition->antigrav_strength * slope_fraction * instance->mass *
                    (g_physics_constants->gravity * compression * compression - definition->antigrav_damp_factor * normal_velocity);
                if (TEST_BIT(ray_test.flags, 6) && ray_test.normal.k < global_slip_surface_maximum_k_get())
                    instance->previous_active_anti_gravity_point_count++;
                if (point->oscillation > 0.0f)
                {
                    float frequency = 0.9f + (float)((index + 1) / physics->anti_gravity_points.count) * (1.0f - 0.9f);
                    float phase = (float)index * 42.0f + frequency * 0.3f * (float)TLS_GAME_TIME_GLOBALS->time * TLS_GAME_TIME_GLOBALS->tick_length;
                    float oscillation = point->oscillation * (0.45f + (1.0f - 0.45f) * periodic_function_evaluate(8, phase));
                    spring_force *= 1.0f - oscillation;
                }
                point_from_line3d(&point->spring_force, &ray_test.normal, spring_force, &point->spring_force);
                point->local_physics_object_index = ray_test.object_index;
                point->active = true;
                point->compression = compression;
            }
            add_vectors3d(&point->force, &point->spring_force, &point->force);
            cross_product3d(&point->position_offset, &point->force, &point->torque);
            havok_vehicle_physics_accumulate_anti_gravity_force(force, &point->force, &force->k);
            // match: 0x578140: keep the vertical torque address as an inline parameter after unifying gravity.
            havok_vehicle_physics_accumulate_anti_gravity_force(torque, &point->torque, &torque->k);
        }
    }
    long grounded_count = 0;
    long active_count = 0;
    long sound_impact_indexes_count = 0;
    long sound_impact_indexes[32];
    bool on_moving_object = false;
    s_havok_sphere_test tests[16];
    float maximum_friction_force = physics->maximum_friction_acceleration * instance->mass;
    if (physics->friction_points.count > 0)
    {
        havok_calculate_features_in_simple_shape_phantom(object->object.havok_component, physics->minimum_k,
            (const s_havok_sphere_query *)instance->friction_points, tests,
            physics->friction_points.count, cache, !instance->update_flag);
        if (instance->update_flag)
            object->vehicle.friction_update_time = TLS_GAME_TIME_GLOBALS->time;
    }
    for (long index = 0; index < physics->friction_points.count; index++)
    {
        s_friction_vehicle_physics_point_datum *point = &instance->friction_points[index];
        if (!point->use_surface_material)
        {
            s_havok_vehicle_friction_point_definition *definition = (s_havok_vehicle_friction_point_definition *)tag_block_get_element_with_size(
                &physics->friction_points, index, sizeof(s_havok_vehicle_friction_point_definition));
            bool powered = definition->m_flags.test(_friction_point_definition_flag1);
            real_vector3d friction_force = *global_zero_vector3d;
            s_havok_sphere_test *test = &tests[index];
            active_count++;
            vector_from_points3d(&instance->center_of_mass, &point->marker_position, &point->position_offset);
            cross_product3d(&angular_velocity, &point->position_offset, &point->velocity);
            add_vectors3d(&point->velocity, &linear_velocity, &point->velocity);
            compute_ground_plane(instance, test, index);
            if (point->flag)
            {
                subtract_vectors3d(&point->velocity, &point->contact_velocity, &point->velocity);
                on_moving_object = true;
            }
            if (point->compression > k_real_epsilon && definition->radius > k_real_epsilon)
            {
                real_vector3d effect_direction = *global_up3d;
                bool slip_surface = false;
                float spring_constant = physics->spring_constant * point->current_radius / definition->radius;
                float damping = physics->spring_damping;
                float normal_velocity = dot_product3d(&point->velocity, &point->normal);
                if (fabsf(spring_constant) >= k_real_epsilon)
                    point->normal_force = instance->mass * (g_physics_constants->gravity * point->compression / spring_constant - damping * normal_velocity);
                scale_vector3d(&point->normal, point->normal_force, &point->spring_force);
                if (test->object_index != -1 && point->normal_force > 0.0f)
                {
                    object_datum *contact_object = (object_datum *)object_get_and_verify_type(test->object_index, -1);
                    if (contact_object->havok_component != -1)
                    {
                        c_havok_component *contact_component = havok_vehicle_physics_contact_component_datum_get(contact_object->havok_component);
                        long body_index = test->rigid_body_index;
                        if (body_index >= 0 && body_index < contact_component->get_rigid_body_count()
                            && !contact_component->get_rigid_body_is_fixed(body_index)
                            && !contact_component->get_rigid_body_is_keyframed(body_index))
                        {
                            float maximum_acceleration = normal_velocity * (float)TLS_GAME_TIME_GLOBALS->ticks_per_second;
                            float body_mass = contact_component->get_rigid_body_mass(body_index);
                            float minimum_mass = instance->mass / havok_vehicle_physics_maximum_mass_scale();
                            float maximum_mass = instance->mass * havok_vehicle_physics_maximum_mass_scale();
                            float mass = PIN(body_mass, minimum_mass, maximum_mass);
                            float normal_scale = (point->normal.k + 1.0f) / 2.0f;
                            float acceleration = normal_scale * point->normal_force / mass;
                            acceleration *= acceleration;
                            acceleration = MIN(acceleration, MAX(maximum_acceleration, 8.0f));
                            real_vector3d contact_acceleration;
                            scale_vector3d(&point->normal, -1.0f * acceleration * TLS_GAME_TIME_GLOBALS->tick_length,
                                &contact_acceleration);
                            contact_component->rigid_body_apply_acceleration(body_index, &contact_acceleration);
                        }
                    }
                }
                slip_surface = TEST_BIT(test->surface_index, 6) && test->normal.k < global_slip_surface_maximum_k_get();
                instance->previous_grounded_friction_point_count += slip_surface != false;
                if (!disable_slip_friction || !slip_surface)
                {
                    real_vector3d normal_component;
                    real_vector3d forward;
                    scale_vector3d(&point->normal, dot_product3d(&point->matrix.forward, &point->normal), &normal_component);
                    subtract_vectors3d(&point->matrix.forward, &normal_component, &forward);
                    float forward_magnitude = normalize3d(&forward);
                    if (forward_magnitude != 0.0f)
                    {
                        real_vector3d tangent_velocity = point->velocity;
                        real_vector3d normal_velocity_vector = point->normal;
                        float friction_scale = 0.9f;
                        scale_vector3d(&normal_velocity_vector, dot_product3d(&point->velocity, &point->normal), &normal_velocity_vector);
                        subtract_vectors3d(&point->velocity, &normal_velocity_vector, &tangent_velocity);
                        real_vector3d sideways_velocity;
                        if (powered)
                        {
                            real_vector3d forward_velocity;
                            scale_vector3d(&forward, -point->forward_velocity, &forward_velocity);
                            add_vectors3d(&tangent_velocity, &forward_velocity, &sideways_velocity);
                        }
                        else
                        {
                            float forward_speed = dot_product3d(&tangent_velocity, &forward);
                            real_vector3d forward_velocity;
                            scale_vector3d(&forward, forward_speed, &forward_velocity);
                            subtract_vectors3d(&tangent_velocity, &forward_velocity, &sideways_velocity);
                            point->forward_velocity = sqrtf(magnitude_squared3d(&forward_velocity)) *
                                (dot_product3d(&forward_velocity, &forward) > 0.0f ? 1.0f : -1.0f);
                        }
                        point->lateral_speed = sqrtf(magnitude_squared3d(&sideways_velocity));
                        bool all_axes;
                        if (point->destroyed || point->braking)
                            all_axes = true;
                        else
                            all_axes = false;
                        float dynamic_friction = point->destroyed ? definition->e_brake_moving_friction : physics->dynamic_friction;
                        float threshold = point->destroyed ? definition->e_brake_moving_friction_vel_diff : definition->moving_friction_velocity_diff;
                        float static_friction = point->destroyed ? definition->e_brake_friction : physics->static_friction;
                        float speed = all_axes ? sqrtf(magnitude_squared3d(&tangent_velocity)) : point->lateral_speed;
                        const real_vector3d *velocity = all_axes ? &tangent_velocity : &sideways_velocity;
                        float slope_fraction = 1.0f - PIN((physics->maximum_k - point->normal.k) / (physics->maximum_k - physics->minimum_k), 0.0f, 1.0f);
                        if (speed > threshold)
                        {
                            float dynamic_friction_scale = sqrtf(dynamic_friction * friction_scale);
                            real_vector3d direction = *velocity;
                            normalize3d(&direction);
                            const float &normal_force = point->normal_force;
                            scale_vector3d(&direction,
                                -dynamic_friction_scale * definition->fraction_of_total_mass * normal_force * slope_fraction,
                                    &friction_force);
                            point->sliding = true;
                        }
                        else
                        {
                            float normal_force = PIN(point->normal_force, -maximum_friction_force, maximum_friction_force);
                            grounded_count++;
                            float static_friction_scale = sqrtf(static_friction * friction_scale);
                            scale_vector3d(velocity,
                                -static_friction_scale * TLS_GAME_TIME_GLOBALS->tick_length * definition->fraction_of_total_mass *
                                    normal_force * slope_fraction,
                                &friction_force);
                        }
                        effect_direction = sideways_velocity;
                    }
                }
                point->grounded = true;
                if (point->current_radius > k_real_epsilon && point->normal.k > physics->minimum_k)
                {
                    float volume = MAX(point->lateral_speed - definition->moving_friction_velocity_diff, 0.0f);
                    c_impact::s_contact_description description;
                    if (normalize3d(&effect_direction) == 0.0f)
                        effect_direction = *global_up3d;
                    real_point3d contact_position;
                    point_from_line3d(&point->marker_position, &point->normal,
                        -(point->current_radius - point->compression - k_real_epsilon), &contact_position);
                    impact_initialize_contact_description(&description, 0, false, instance->object_index, 0,
                        definition->collision_global_material_index, -1,
                        -1, global_material_resolve_underwater_material_proxy(point->material, &contact_position), &contact_position,
                        &point->normal, index, 0);
                    havok_vehicle_physics_update_contact_description(instance, &description, &effect_direction, volume,
                        point->destroyed || point->braking);
                    description.disconnected_from_physics = true;
                    long impact_index = havok_vehicle_physics_update_contact_description(
                        instance, &description, &effect_direction, volume, point->destroyed || point->braking);
                    if (impact_index != -1)
                    {
                        bool found = false;
                        for (long sound_index = 0; sound_index < sound_impact_indexes_count; sound_index++)
                        {
                            if (sound_impact_indexes[sound_index] == impact_index)
                            {
                                found = true;
                                break;
                            }
                        }
                        if (!found && (unsigned long)sound_impact_indexes_count < NUMBEROF(sound_impact_indexes))
                            sound_impact_indexes[sound_impact_indexes_count++] = impact_index;
                    }
                }
            }
            else
            {
                havok_vehicle_physics_remove_impacts_for_friction_point_index(instance, index, false);
                havok_vehicle_physics_remove_impacts_for_friction_point_index(instance, index, true);
            }
            add_vectors3d(&point->force, &point->spring_force, &point->force);
            add_vectors3d(&point->force, &friction_force, &point->force);
            cross_product3d(&point->position_offset, &point->force, &point->torque);
            add_vectors3d(force, &point->force, force);
            add_vectors3d(&point->torque, torque, torque);
            if (powered && point->grounded)
            {
                real_vector3d parallel;
                real_vector3d perpendicular;
                component_vectors_from_normal3d(&friction_force, &point->matrix.forward, &parallel, &perpendicular);
                float magnitude = (float)sqrt((double)havok_vehicle_physics_friction_magnitude_squared(&parallel));
                if (dot_product3d(&friction_force, &point->matrix.forward) > 0.0f)
                    instance->load_torque -= magnitude;
                else
                    havok_vehicle_physics_add_friction_rpm(&instance->load_torque, magnitude);
            }
        }
        else
        {
            havok_vehicle_physics_remove_impacts_for_friction_point_index(instance, index, false);
            havok_vehicle_physics_remove_impacts_for_friction_point_index(instance, index, true);
        }
    }
    for (long index = 0; index < component->get_impact_count(); index++)
    {
        long impact_index = datum_get_typed<s_impact_array>(TLS_IMPACT_ARRAY_DATA, component->m_impact_array_index)->impact_indices[index];
        c_impact *impact = havok_vehicle_physics_impact_datum_get(impact_index);
        if (impact->sound_only)
        {
            bool keep = false;
            for (long sound_index = 0; sound_index < sound_impact_indexes_count; sound_index++)
            {
                if (sound_impact_indexes[sound_index] == impact_index)
                {
                    for (long other_index = 0; other_index < component->get_impact_count(); other_index++)
                    {
                        long other_impact_index = datum_get_typed<s_impact_array>(
                            TLS_IMPACT_ARRAY_DATA, component->m_impact_array_index)->impact_indices[other_index];
                        c_impact *other_impact = havok_vehicle_physics_impact_datum_get(other_impact_index);
                        if (other_impact->vehicle_friction_point_index != -1 && !other_impact->sound_only
                            && other_impact->get_global_material_index_a() == impact->get_global_material_index_a()
                            && other_impact->get_global_material_index_b() == impact->get_global_material_index_b())
                        {
                            keep = true;
                            break;
                        }
                    }
                }
            }
            if (keep)
                impact->update_vehicle_impact_sound_only(impact_index);
            else
            {
                while (impact->physics_reference_count > 0)
                    impact->remove_reference(impact_index, false);
            }
        }
    }
    return !on_moving_object && physics->friction_points.count && grounded_count > (active_count >> 1) && instance->world_matrix.up.k > physics->minimum_k;
}

static const float k_havok_vehicle_physics_linear_damping = 270.0f;
static const float k_havok_vehicle_physics_angular_damping = 18.0f;

static __forceinline float havok_vehicle_physics_damping_scale(float damping, float tick_length)
{
    return -damping / tick_length;
}

static __forceinline void havok_vehicle_physics_subtract_mover_velocity(real_vector3d *velocity, const real_vector3d *mover_velocity)
{
    float velocity_j = velocity->j;
    float mover_k = mover_velocity->k;
    float mover_j = mover_velocity->j;
    velocity->i -= mover_velocity->i;
    velocity->j = velocity_j - mover_j;
    velocity->k -= mover_k;
}

static __forceinline bool havok_vehicle_physics_limit3d(real_vector3d *vector, float limit)
{
    float magnitude_squared = magnitude_squared3d(vector);
    if (magnitude_squared > limit * limit)
    {
        scale_vector3d(vector, limit / sqrtf(magnitude_squared), vector);
        return true;
    }
    return false;
}

static __forceinline void apply_rigid_body_linear_velocity(c_havok_component *component, long index, const real_vector3d *velocity)
{
    phys_rigid_body *body = component->get_rigid_body(index);
    if (!body->body_is_fixed())
    {
        phys_vec4 havok_velocity;
        havok_velocity.set(velocity->i, velocity->j, velocity->k, 0.0f);
        body->activate();
        body->get_motion()->set_linear_velocity(havok_velocity);
    }
}

// @address 0xf2778
void havok_vehicle_physics_update(long object_index, s_havok_vehicle_physics_instance *instance, float damping, bool disable_slip_friction,
    const real_vector3d *magic_force, const real_vector3d *magic_torque, const real_point3d *force_position)
{
    float tick_length = TLS_GAME_TIME_GLOBALS->tick_length;
    vehicle_datum *object = OBJECT_GET(object_index, vehicle_datum, 2);
    vehicle_definition *vehicle = (vehicle_definition *)tag_get('vehi', object->object.definition_index);
    s_havok_vehicle_physics_definition *physics = &vehicle->havok_vehicle_physics;
    c_havok_component *component = havok_vehicle_physics_component_datum_get(object->object.havok_component);
    long rigid_body_index = havok_vehicle_physics_controlling_body_index(component);
    long grounded_friction_points = 0;
    long active_anti_gravity_points = 0;
    bool lost_contact = false;
    real_vector3d force, torque;
    if (!havok_vehicle_physics_compute(instance, disable_slip_friction, &force, &torque, force_position))
        damping = 0.0f;
    for (long index = 0; index < physics->friction_points.count; index++)
        grounded_friction_points += instance->friction_points[index].grounded != 0;
    for (long index = 0; index < physics->anti_gravity_points.count; index++)
        active_anti_gravity_points += instance->anti_gravity_points[index].strength != 0.0f;
    lost_contact |= instance->previous_grounded_friction_point_count != 0 && instance->previous_grounded_friction_point_count >= grounded_friction_points;
    lost_contact |= instance->previous_active_anti_gravity_point_count != 0 && instance->previous_active_anti_gravity_point_count >= active_anti_gravity_points;
    if (magic_force && (!lost_contact || !disable_slip_friction))
        add_vectors3d(&force, magic_force, &force);
    if (magic_torque && (!lost_contact || !disable_slip_friction))
        add_vectors3d(&torque, magic_torque, &torque);
    real_vector3d linear_velocity, angular_velocity;
    real_vector3d early_mover_linear_velocity, early_mover_angular_velocity;
    object_get_velocities(object_index, &linear_velocity, &angular_velocity);
    bool on_early_mover = object_get_early_mover_local_space_velocity(
        object_index, &instance->world_matrix.position, &early_mover_linear_velocity, &early_mover_angular_velocity, true, false);
    if (on_early_mover)
    {
        havok_vehicle_physics_subtract_mover_velocity(&linear_velocity, &early_mover_linear_velocity);
        subtract_vectors3d(&angular_velocity, &early_mover_angular_velocity, &angular_velocity);
    }
    real_vector3d linear_acceleration, angular_acceleration;
    scale_vector3d(&force, 1.0f / instance->mass, &linear_acceleration);
    real_matrix3x3 inverse_inertia_tensor;
    component->get_rigid_body_inverse_inertia_tensor(0, &inverse_inertia_tensor);
    real_matrix3x3 rotation = instance->world_matrix.rotation;
    matrix3x3_multiply(&rotation, &inverse_inertia_tensor, &inverse_inertia_tensor);
    matrix3x3_multiply(&inverse_inertia_tensor, matrix3x3_transpose(&rotation, &rotation), &inverse_inertia_tensor);
    matrix3x3_transform_vector(&inverse_inertia_tensor, &torque, &angular_acceleration);
    point_from_line3d(&linear_velocity, &linear_acceleration, tick_length, &linear_velocity);
    point_from_line3d(&angular_velocity, &angular_acceleration, tick_length, &angular_velocity);
    if (damping > 0.0f)
    {
        real_vector3d linear_damping, angular_damping;
        scale_vector3d(&linear_velocity, havok_vehicle_physics_damping_scale(damping, tick_length), &linear_damping);
        linear_damping.k = 0.0f;
        havok_vehicle_physics_limit3d(&linear_damping, k_havok_vehicle_physics_linear_damping);
        linear_damping.k = 0.0f;
        scale_vector3d(&angular_velocity, havok_vehicle_physics_damping_scale(damping, tick_length), &angular_damping);
        havok_vehicle_physics_limit3d(&angular_damping, k_havok_vehicle_physics_angular_damping);
        angular_damping.i = 0.0f;
        angular_damping.j = 0.0f;
        point_from_line3d(&linear_velocity, &linear_damping, tick_length, &linear_velocity);
        point_from_line3d(&angular_velocity, &angular_damping, tick_length, &angular_velocity);
    }
    if (on_early_mover)
    {
        add_vectors3d(&linear_velocity, &early_mover_linear_velocity, &linear_velocity);
        add_vectors3d(&angular_velocity, &early_mover_angular_velocity, &angular_velocity);
    }
    if (rigid_body_index != -1)
    {
        apply_rigid_body_linear_velocity(component, rigid_body_index, &linear_velocity);
        component->set_rigid_body_angular_velocity(rigid_body_index, &angular_velocity);
    }
    object->vehicle.grounded = 0;
    for (long index = 0; index < instance->definition->friction_points.count && index < 0x10000; index++)
    {
        s_friction_vehicle_physics_point_datum *point = &instance->friction_points[index];
        if (!point->sliding && point->grounded)
            object->vehicle.grounded &= ~(1 << index);
        else
            object->vehicle.grounded |= 1 << index;
    }
}
