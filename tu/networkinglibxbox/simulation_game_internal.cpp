// networkinglibxbox/simulation_game_internal.obj: from the records 0x40de30 0x40e340 0x40e770 0x40e9a0 0x40ea10 0x40eb58 0x40ebd8 0x40ec70 0x40ed08 0x40f0e0
// @flags /TP /O2 /Oc
#include "units/biped_types.h"
#include "memory/tls_data_arrays.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "objects/object_header_access.h"
#include "objects/object_type_access.h"
#include "networking/simulation/simulation_event_definition.h"
#include "physics/havok_group_types.h"
#include "simulation/simulation_game_position_update_types.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "memory/bitstream.h"
#include "math/real_math.h"
#include "game/game_globals_storage.h"
#include "game/map_variant.h"
#include "objects/object_datum.h"
#include "units/motor_enums.h"
#include "units/motor_request.h"
#include "units/unit_passenger_iterator.h"
#include "phys/phys_collide.h"
#include "tag_files/tag_types.h"
// @address 0x40ea10 ?simulation_calculate_position_relevance@@YAMPBTreal_point3d@@MPBUs_simulation_view_telemetry_data@@@Z
// @address 0x40eb58 ?simulation_write_definition_index@@YAXPBDPAVc_bitstream@@J@Z
// @address 0x40ebd8 ?simulation_read_definition_index@@YAJPBDPAVc_bitstream@@@Z
// @address 0x40ec70 ?object_is_dead_or_dying_biped@@YA_NJ@Z
#include <math.h>
#include "math/matrix_math.h"
#include "phys/phys_base.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "game/game_engine_sandbox.h"
#include "motor/motor_system.h"
#include "objects/damage.h"
#include "simulation/game_interface/simulation_game_internal.h"
#include "simulation/game_interface/simulation_game_objects.h"
#include "game/game_engine_teleporters.h"
#include "game/game_time_types.h"
#include "game/player_machine.h"
#include "simulation/game_interface/simulation_object_interpolation.h"
#include "cache/cache_files.h"
#include "physics/havok.h"

#include "cseries/sort.h"
#include "math/matrix_math_types.h"
#include "scenario/scenario_tag.h"
#include "objects/object_reference_types.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "scenario/scenario.h"



#include "memory/tls_globals.h"
#include "game/game_engine_scoring_globals.h"
#include "units/biped_definitions.h"
#include "physics/havok_constants_types.h"
#include "physics/character_physics_capsule_types.h"
#include "physics/character_physics_shape_types.h"
#include "networking/replication/replication_event.h"


typedef struct __regfix_ret_simulation_game_internal_object_datum { unit_datum * v; } __regfix_ret_simulation_game_internal_object_datum;




static __forceinline float relevance_normalize3d(real_vector3d *vector)
{
    float length = magnitude3d(vector);
    if (!(fabs(length - 0.0f) < 0.0001f))
    {
        scale_vector3d(vector, 1.0f / length, vector);
    }
    else
    {
        length = 0.0f;
    }
    return length;
}
float simulation_calculate_position_relevance(const real_point3d *position, float maximum_distance, const s_simulation_view_telemetry_data *telemetry_data)
{
    float relevance = 0.0f;
    const s_position_relevance_configuration *configuration = &g_network_configuration.position_relevance;

    for (long player_index = 0; player_index < telemetry_data->number_of_players; player_index++)
    {
        real_vector3d player_to_position;
        vector_from_points3d(&telemetry_data->players[player_index].position, position, &player_to_position);
        float distance = relevance_normalize3d(&player_to_position);
        if (distance < configuration->minimum_distance)
        {
            relevance = 1.0f;
            break;
        }
        else
        {
            float cosine = dot_product3d(&player_to_position, &telemetry_data->players[player_index].forward);
            if (cosine > configuration->maximum_cosine)
            {
                relevance = 1.0f;
                break;
            }
            else if ((cosine >= configuration->minimum_cosine && distance < maximum_distance) || distance < configuration->minimum_view_distance)
            {
                relevance = MAX(relevance, (maximum_distance - distance) / maximum_distance);
            }
        }
    }

    return relevance;
}

void simulation_write_definition_index(const char *description, c_bitstream *packet, long definition_index)
{
    long index = -1;
    if (definition_index != -1)
    {
        long key = definition_index;
        scenario *globals = global_scenario;
        if (globals != 0 && globals->simulation_definition_table.count > 0)
            index = ::bsearch_elements(&key, globals->simulation_definition_table.address,
                globals->simulation_definition_table.count, sizeof(key), armor_modifier_bsearch_proc, 0);
    }
    packet->write_integer(description, index + 1, 12);
}

long simulation_read_definition_index(const char *description, c_bitstream *packet)
{
    long result = -1;
    long index = packet->read_integer(description, 12) - 1;
    if (index != -1)
    {
        result = -1;
        scenario *globals = global_scenario;
        if (globals != 0 && globals->simulation_definition_table.count > 0 && index >= 0 && index < globals->simulation_definition_table.count)
            result = ((long *)globals->simulation_definition_table.address)[index];
    }
    return result;
}


namespace {
static __forceinline bool simulation_game_internal_damage_flag_test(const biped_datum *biped)
{
    return (bool)((biped->object.damage_flags >> 2) & 1);
}
}

// 0x40ec70
bool object_is_dead_or_dying_biped(long object_index)
{
    bool result = false;
    if (object_get_type(object_index) == 0)
    {
        biped_datum *biped = (biped_datum *)object_get_and_verify_type(object_index, 1);
        if ((simulation_game_internal_damage_flag_test(biped) && !motor_system_in_airborne_state(object_index)) ||
            ::unit_is_dying(object_index) || biped->unit.death_state != 0)
            result = true;
    }
    return result;
}

static __forceinline const s_entity_relevance_configuration &entity_relevance_configuration()
{
    return g_network_configuration.entity_relevance;
}
static __forceinline long entity_object_type(long index)
{
    return object_header_get(index)->object_type;
}

// @address 0x40e770 ?simulation_calculate_entity_relevance@@YAMJPBJMPBUs_simulation_view_telemetry_data@@PA_N@Z
float simulation_calculate_entity_relevance(long count, const long *indices, float distance, const s_simulation_view_telemetry_data *telemetry, bool *out)
{
    float minimum_distance_squared = 3.402823466e+38f;
    float relevance;
    bool local_entity = false;
    bool in_view = false;
    float view_relevance = 0.0f;
    for (long entity = 0; entity < count; ++entity)
    {
        for (long player = 0; player < telemetry->number_of_players; ++player)
        {
            if (telemetry->players[player].entity_index == indices[entity])
            {
                local_entity = true;
                break;
            }
            long object = simulation_object_get_replicated_object_from_entity(indices[entity]);
            if (object != -1)
            {
                real_point3d origin;
                object_get_origin(object, &origin);
                float distance_squared = distance_squared3d(&origin, &telemetry->players[player].position);
                minimum_distance_squared = minimum_distance_squared > distance_squared ? distance_squared : minimum_distance_squared;
                if ((1 << entity_object_type(object)) & 0x1003)
                {
                    long zoom_level = telemetry->players[player].zoom_level;
                    if (zoom_level != -1)
                    {
                        float cosine = 0.0f;
                        float minimum_cosine = 1.0f;
                        real_vector3d to_object;
                        vector_from_points3d(&telemetry->players[player].position, &origin, &to_object);
                        if (::normalize3d(&to_object) != 0.0f)
                        {
                            switch (zoom_level)
                            {
                            case 0:
                                minimum_cosine = entity_relevance_configuration().zoom_cosine[0];
                                view_relevance = entity_relevance_configuration().zoom_relevance[0];
                                break;
                            case 1:
                                minimum_cosine = entity_relevance_configuration().zoom_cosine[1];
                                view_relevance = entity_relevance_configuration().zoom_relevance[1];
                                break;
                            }
                            cosine = dot_product3d(&to_object, &telemetry->players[player].forward);
                            if (cosine >= minimum_cosine)
                                in_view = true;
                        }
                    }
                }
            }
        }
        if (local_entity)
            break;
    }
    if (local_entity)
        relevance = 1.0f;
    else
    {
        if (distance > 0.0f)
        {
            relevance = (distance - sqrtf(minimum_distance_squared)) / distance;
            if (relevance < 0.0f) relevance = 0.0f;
        }
        else
            relevance = 0.0f;
        if (in_view)
            relevance = relevance > view_relevance ? relevance : view_relevance;
    }
    if (out)
        *out = local_entity;
    return relevance;
}

// @address 0x40e9a0 simulation_calculate_event_relevance
extern "C" float simulation_calculate_event_relevance(c_simulation_event_definition *definition,
    const c_replication_outgoing_event *event, float relevance, const s_simulation_view_telemetry_data *telemetry, bool *out)
{
    long reference_count = definition->number_of_entity_references();
    long reference_indices[2];
    for (long i = 0; i < reference_count; i++)
        reference_indices[i] = event->get_entity_reference(i);
    return simulation_calculate_entity_relevance(reference_count, reference_indices, relevance, telemetry, 0);
}


static __forceinline bool simulation_game_internal_limit_vector3d(real_vector3d *vector, float limit)
{
    float squared = magnitude_squared3d(vector);
    if (squared > limit * limit)
    {
        scale_vector3d(vector, limit / sqrtf(squared), vector);
        return true;
    }
    return false;
}
// @address 0x40f0e0 ?simulation_calculate_velocity_for_velocity_adjustment@@YAXJPBTreal_point3d@@PBTreal_vector3d@@111PAT2@2@Z
void simulation_calculate_velocity_for_velocity_adjustment(long index, const real_point3d *position, const real_vector3d *forward,
    const real_vector3d *up, const real_vector3d *linear, const real_vector3d *angular, real_vector3d *new_linear,
    real_vector3d *new_angular)
{
    real_matrix4x3 world, desired, inverse;
    real_quaternion rotation;
    float angle;
    real_vector3d angular_adjustment, linear_adjustment;
    ::object_get_world_matrix(index, &world);
    matrix4x3_from_point_and_vectors(&desired, position, forward, up);
    matrix4x3_inverse(&world, &inverse);
    matrix4x3_compute_product(&desired, &inverse, &world);
    matrix3x3_rotation_to_quaternion(&world.rotation, &rotation);
    quaternion_to_angle_and_vector(&rotation, &angle, &angular_adjustment);
    scale_vector3d(&angular_adjustment, angle * 5.0f, &angular_adjustment);
    simulation_game_internal_limit_vector3d(&angular_adjustment, 1.0f);
    real_vector3d translation = { world.position.x, world.position.y, world.position.z };
    scale_vector3d(&translation, 0.9f, &linear_adjustment);
    simulation_game_internal_limit_vector3d(&linear_adjustment, 0.05f);
    add_vectors3d(linear, &linear_adjustment, new_linear);
    add_vectors3d(angular, &angular_adjustment, new_angular);
}



static __forceinline unit_datum *simulation_game_internal_object_datum(long object_index)
{
    return (unit_datum *)object_header_get(object_index)->datum;
}
static __forceinline __regfix_ret_simulation_game_internal_object_datum simulation_game_internal_object_datum_rfret(long object_index)
{
    {
        __regfix_ret_simulation_game_internal_object_datum rf_ret;
        rf_ret.v = ((unit_datum *)object_header_get(object_index)->datum);
        return rf_ret;
    }
}
static __forceinline bool simulation_game_internal_query_local_predictive_control(long object_index)
{
    bool result = false;
    bool predicted_mode = false;
    if ((unsigned long)(tls_game_globals()->options.simulation_type - 4) <= 1)
        predicted_mode = true;
    unit_datum *object = simulation_game_internal_object_datum(object_index);
    if (predicted_mode && ((1 << object->object.object_type) & 0x1003))
    {
        long player_index = object->unit.player_index;
        if (player_index == -1)
        {
            long parent_index = object->unit.driver_object_index;
            player_index = parent_index != -1 ? simulation_game_internal_object_datum(parent_index)->unit.player_index : object->unit.player_index;
        }
        if (player_index != -1)
            result = player_is_on_local_machine(player_index);
    }
    return result;
}
static __forceinline void simulation_game_internal_flag_set_long(unsigned long *flags, long bit, bool value)
{
    if (value)
        *flags |= (1 << bit);
    else
        *flags &= ~(1 << bit);
}
static __forceinline void simulation_game_internal_set_acceleration(unit_datum *object, const real_vector3d *source, long game_time)
{
    real_vector3d *acceleration = &object->unit.seat_acceleration;
    simulation_game_internal_flag_set_long(&object->unit.unit_flags, 26, true);
    *acceleration = *source;
    object->unit.seat_acceleration_reset_time = game_time;
}
// 0x40ed08
// @address 0x40ed08 ?simulation_warp_object@@YAXJPBTreal_point3d@@PBTreal_vector3d@@111@Z
void simulation_warp_object(long object_index, const real_point3d *position, const real_vector3d *forward, const real_vector3d *up,
    const real_vector3d *linear_velocity, const real_vector3d *angular_velocity)
{
    unit_datum *object = simulation_game_internal_object_datum_rfret(object_index).v;
    bool interpolate = true;

    if (position)
    {
        bool is_biped = object_header_get(object_index)->object_type == 0;
        bool far_away = distance_squared3d(&object->object.position, position) > 1.0f;
        bool has_parent = object->object.parent_object_index != -1;
        bool predicted = simulation_game_internal_query_local_predictive_control(object_index);
        interpolate = !far_away && !has_parent && !predicted && is_biped;
    }

    if (interpolate)
    {
        if (position)
        {
            s_simulation_object_interpolation_data interpolation_data = { 0 };
            if (simulation_object_get_interpolation_data(object_index, &interpolation_data))
            {
                interpolation_data.flags |= 1;
                unsigned char *interpolation_flags = &object->object.flags_d6;
                *interpolation_flags = (unsigned char)(*interpolation_flags | 1);
                interpolation_data.position = object->object.position;
                simulation_object_set_interpolation_data(object_index, &interpolation_data);
            }
        }
        if (forward && up)
        {
            unit_datum *current = simulation_game_internal_object_datum_rfret(object_index).v;
            simulation_set_object_orientation_for_interpolation(object_index, &current->object.forward, &current->object.up);
        }
    }
    else
    {
        simulation_disable_object_interpolation(object_index);
    }

    object_set_position_direct(object_index, position, forward, up, 0, false);
    object_set_velocities(object_index, linear_velocity, angular_velocity);

    if ((1 << object_header_get(object_index)->object_type) & 0x1803)
    {
        c_motor_request request;
        request.setup_handle_simulation_warp(_action_none);
        motor_system_submit(object_index, &request);
    }

    if (object_header_get(object_index)->object_type == 1)
    {
        long game_time = tls_game_time_globals()->time;
        const real_vector3d *zero_vector = global_zero_vector3d;
        for (long child = object->object.first_child_object_index; child != -1; )
        {
            unit_datum *child_object = simulation_game_internal_object_datum_rfret(child).v;
            if ((1 << child_object->object.object_type) & 0x1003)
                simulation_game_internal_set_acceleration(simulation_game_internal_object_datum(child), zero_vector, game_time);
            child = child_object->object.next_object_index;
        }
        c_multi_unit_passenger_iterator iterator;
        iterator.begin(object_index);
        while (iterator.next())
            simulation_game_internal_set_acceleration(simulation_game_internal_object_datum_rfret(iterator.m_passenger_index).v, zero_vector, game_time);
    }
}



static __forceinline real_vector3d simulation_game_internal_vector_from_points(const real_vector3d *a, const real_vector3d *b)
{
    real_vector3d result;
    result.i = a->i - b->i;
    result.j = a->j - b->j;
    result.k = a->k - b->k;
    return result;
}

// @address 0x40e340 ?simulation_apply_position_update@@YAXJJJPBTreal_point3d@@PBTreal_vector3d@@111@Z
void simulation_apply_position_update(long object_index, long context, long result, const real_point3d *position,
    const real_vector3d *forward, const real_vector3d *up, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity)
{
    switch (result)
    {
    case 0:
        break;
    case 3:
    {
        bool update_linear = linear_velocity != 0;
        bool update_angular = angular_velocity != 0;
        if (!object_is_dead_or_dying_biped(object_index) && (update_linear || update_angular))
        {
            real_matrix4x3 world;
            real_point3d current_position;
            real_vector3d current_forward, current_up, current_linear, current_angular;
            real_vector3d new_linear, new_angular;
            ::object_get_world_matrix(object_index, &world);
            current_position = position ? *position : world.position;
            current_forward = forward ? *forward : world.rotation.forward;
            current_up = up ? *up : world.rotation.up;
            object_get_velocities(object_index, &current_linear, &current_angular);
            if (update_linear)
            {
                real_vector3d delta = simulation_game_internal_vector_from_points(linear_velocity, &current_linear);
                if (context == 2 && !(magnitude_squared3d(&delta) > 0.06f) && !::game_engine_is_object_being_edited(object_index, (long *)(0)))
                    update_linear = false;
                else
                    current_linear = *linear_velocity;
            }
            if (update_angular)
            {
                real_vector3d delta = simulation_game_internal_vector_from_points(angular_velocity, &current_angular);
                if (context == 2 && !(magnitude_squared3d(&delta) > 0.01f) && !::game_engine_is_object_being_edited(object_index, (long *)(0)))
                    update_angular = false;
                else
                    current_angular = *angular_velocity;
            }
            simulation_calculate_velocity_for_velocity_adjustment(object_index, &current_position, &current_forward, &current_up,
                &current_linear, &current_angular, &new_linear, &new_angular);
            if (update_linear || update_angular)
                object_set_velocities(object_index, update_linear ? &new_linear : 0, update_angular ? &new_angular : 0);
        }
        break;
    }
    case 1:
        simulation_warp_object(object_index, position, forward, up, linear_velocity, angular_velocity);
        break;
    case 2:
        simulation_warp_object(object_index, position, 0, 0, linear_velocity, angular_velocity);
        break;
    default:
        // match: 0x40e340: unreachable hint omits dead branch check
        __assume(0);
    }
    if (result != 0 && ::game_engine_is_object_being_edited(object_index, (long *)(0)))
    {
        unit_datum *object = simulation_game_internal_object_datum(object_index);
        if ((unsigned short)object->object.map_variant_index != 0xffff)
        {
            s_variant_object_datum variant_object;
            tls_game_engine_globals()->map_info.get_variant_object_datum((short)object->object.map_variant_index, &variant_object);
            if (position)
                variant_object.position = *position;
            if (forward || up)
            {
                variant_object.forward = *forward;
                variant_object.up = *up;
            }
            tls_game_engine_globals()->map_info.set_variant_object_datum((short)object->object.map_variant_index, &variant_object, false);
        }
    }
    bool simulation_mode_4 = false;
    if (tls_game_globals()->options.simulation_type == 4)
        simulation_mode_4 = true;
    if (simulation_mode_4 && position)
    {
        unit_datum *object = simulation_game_internal_object_datum(object_index);
        object->object.simulation_object_index = tls_game_time_globals()->time;
        object->object.simulation_update_position = *position;
    }
}

static __forceinline bool simulation_game_internal_is_predicted()
{
    bool result = false;
    if (tls_game_globals()->options.simulation_type == 4)
        result = true;
    return result;
}
static __forceinline unsigned short *simulation_game_internal_try_interpolation(long object_index)
{
    unit_datum *object = (unit_datum *)object_try_and_get_and_verify_type(object_index, ~0UL);
    if (object && object->object.multiplayer_properties.offset != -1)
        return (unsigned short *)((char *)object + object->object.multiplayer_properties.offset);
    return 0;
}

static __forceinline const phys_shape *simulation_game_internal_character_physics_get_shape(
    const biped_definition *definition, long shape_type)
{
    if (definition->physics.pills.count)
        return &((const s_character_physics_capsule *)definition->physics.pills.address)[shape_type].shape;
    const s_character_physics_dead_sphere *sphere =
        &((const s_character_physics_dead_sphere *)definition->physics.spheres.address)[shape_type];
    if (!(definition->physics.flags & 1))
        return &sphere->transform;
    return &sphere->shape;
}
static __forceinline unsigned long simulation_game_internal_multiplayer_is_selected(unsigned long flags)
{
    return _rotl(flags, 21) & 1;
}

// @address 0x40de30 ?simulation_evaluate_position_update@@YA?AW4e_simulation_game_position_update_result@@JW4e_simulation_game_position_update_context@@PBTreal_point3d@@PBTreal_vector3d@@222@Z
e_simulation_game_position_update_result simulation_evaluate_position_update(long object_index,
    e_simulation_game_position_update_context context, const real_point3d *position, const real_vector3d *forward, const real_vector3d *up,
    const real_vector3d *linear, const real_vector3d *angular)
{
    unit_datum *object = simulation_game_internal_object_datum(object_index);
    bool vehicle = ((1 << object->object.object_type) & 2) != 0;
    bool large_update = false;
    bool small_update = false;
    real_point3d current_position;
    real_vector3d current_forward, current_up;
    object_get_origin(object_index, &current_position);
    object_get_orientation(object_index, &current_forward, &current_up);
    if (position)
    {
        float small_distance = vehicle ? 0.4f : 0.2f;
        if (object_is_dead_or_dying_biped(object_index))
        {
            biped_datum *biped = (biped_datum *)object_get_and_verify_type(object_index, 1);
            small_distance = biped->unit.death_state ? 1.5f : 0.8f;
        }
        float large_distance;
        if (context == _simulation_game_position_update_context_predicted)
            large_distance = vehicle ? g_network_configuration.vehicle_position_update_distance : g_network_configuration.position_update_distance;
        else
            large_distance = vehicle ? g_network_configuration.vehicle_position_update_distance_remote
                                     : g_network_configuration.position_update_distance_remote;
        unsigned short *interpolation = simulation_game_internal_try_interpolation(object_index);
        if (simulation_game_internal_is_predicted() &&
            (::game_engine_is_object_being_edited(object_index, (long *)(0)) ||
             (interpolation && simulation_game_internal_multiplayer_is_selected(*interpolation))))
        {
            large_distance = 1.0f;
            small_distance = 0.5f;
        }
        if (!simulation_game_internal_is_predicted() && context == _simulation_game_position_update_context_client_prediction &&
            ::game_engine_is_object_being_edited(object_index, (long *)(0)))
        {
            large_distance = 1000.0f;
            small_distance = 0.5f;
        }
        else if ((1 << object_get_type(object_index)) & 0x1003)
        {
            long last_update_time = simulation_game_internal_object_datum(object_index)->unit.last_in_phantom_volume_time;
            if (last_update_time != -1)
            {
                float elapsed_ticks = (float)(tls_game_time_globals()->time - last_update_time);
                if (tls_game_time_globals()->tick_length * elapsed_ticks < 1.0f)
                    large_distance *= 2.5f;
            }
        }
        large_update = distance_squared3d(&current_position, position) > large_distance * large_distance;
        small_update = distance_squared3d(&current_position, position) > small_distance * small_distance;
    }
    e_simulation_game_position_update_result result;
    if (object->object.parent_object_index == -1)
    {
        switch (context)
        {
        case _simulation_game_position_update_context_authority:
            if (small_update)
                result = _simulation_game_position_update_result_warp;
            else
                result = _simulation_game_position_update_result_adjust_velocity;
            break;
        case _simulation_game_position_update_context_predicted:
            if (large_update || (position && teleporter_are_points_linked(&current_position, position)))
                result = vehicle ? _simulation_game_position_update_result_warp : _simulation_game_position_update_result_warp_position;
            else
                result = _simulation_game_position_update_result_none;
            break;
        case _simulation_game_position_update_context_client_prediction:
            if (large_update)
                result = _simulation_game_position_update_result_none;
            else if (small_update)
                result = _simulation_game_position_update_result_warp;
            else
                result = _simulation_game_position_update_result_adjust_velocity;
            break;
        default:
            // match: 0x40de30: unreachable hint omits dead branch check
            __assume(0);
        }
    }
    else
        result = _simulation_game_position_update_result_none;
    if (result == _simulation_game_position_update_result_adjust_velocity &&
        !((1 << object_get_type(object_index)) & 0x1003) &&
        !((1 << object_get_type(object_index)) & 0x400) &&
        !((1 << object_get_type(object_index)) & 0xc))
        result = _simulation_game_position_update_result_warp;
    if (!simulation_game_internal_is_predicted() && position && object_get_type(object_index) == 0 &&
        (result == _simulation_game_position_update_result_warp || result == _simulation_game_position_update_result_warp_position))
    {
        biped_datum *biped = (biped_datum *)object_get_and_verify_type(object_index, 1);
        bool collides = false;
        const biped_definition *definition =
            (const biped_definition *)global_tag_instances[datum_index_to_absolute_index(biped->object.definition_index)].definition;
        const phys_capsule_shape *shape = (const phys_capsule_shape *)simulation_game_internal_character_physics_get_shape(definition, 0);
        if (shape->m_type == 7)
        {
            float radius = MAX(shape->get_convex_radius() - (g_havok_constants.minimum_radius + 0.001f), g_havok_constants.minimum_radius);
            phys_capsule_shape capsule(shape->get_vertex(0), shape->get_vertex(1), radius);
            real_matrix4x3 matrix;
            object_get_world_matrix(object_index, &matrix);
            matrix.position = *position;
            collides = havok_world_shape_collides(&capsule, biped->object.havok_component_index, &matrix, _havok_group_biped);
        }
        if (collides)
            result = _simulation_game_position_update_result_adjust_velocity;
    }
    long player_index;
    if (::game_engine_is_object_being_edited(object_index, (long *)(&player_index)) && player_is_on_local_machine(player_index))
        result = _simulation_game_position_update_result_none;
    return result;
}

// Empty body per the March debug build (simulation_game_internal.obj).
// @address 0x5bec98 ?gather_memory_stats@phys_ref_object@@UBAXPAVphys_statistics_collector@@@Z
void phys_ref_object::gather_memory_stats(phys_statistics_collector *) const
{
}
