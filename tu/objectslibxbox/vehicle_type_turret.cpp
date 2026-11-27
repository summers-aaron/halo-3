// objectslibxbox/vehicle_type_turret.obj: from the records 0xd7948 0xd7a80 0xd7c60 0xd7d58
// @flags /TP /O2 /Oc
#include "memory/data_types.h"
#include <math.h>
#include "game/game_globals_storage.h"
#include "game/game_time_types.h"
#include "game/game_time_access.h"
#include "game/game_time_inline.h"
#include "game/player_types.h"

#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "items/weapons.h"
#include "scenario/scenario_trigger_volumes.h"
#include "memory/data.h"
#include "objects/object_header.h"
#include "units/vehicle_type_turret.h"
#include "units/unit_facing.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "objects/object_datum.h"
#include "units/vehicle_datum.h"
#include "objects/objects.h"

#include "math/real_math.h"
#include "math/real_math_types.h"

#include "cseries/cseries.h"
#include "cseries/bit_operations.h"

// @rdata 0x820242d8
static const float k_turret_target_lost_timeout_seconds = 2.0f;
// @rdata 0x820224f4
static const float k_turret_target_switch_scale = 1.1f;

// @address 0xd7a80 ?update_auto_target@c_vehicle_type_turret@@AAAXJ@Z
void c_vehicle_type_turret::update_auto_target(long vehicle_index)
{
    float rounding_half = 0.5f;
    if (m_auto_control.target_time != NONE &&
        game_time_get() - m_auto_control.target_time <=
            (long)((float)tls_game_time_globals()->ticks_per_second + rounding_half))
        return;

    float switch_scale = 0.0f;
    float best_weight = 0.0f;
    long best_target_index = NONE;
    c_data_iterator<player_datum> player_iterator;
    player_iterator.begin(tls_player_data());
    if (player_iterator.next()) {
        switch_scale = k_turret_target_switch_scale;
        do {
            player_datum *player = player_iterator.get_datum();
            if (player->unit_index != NONE) {
                float weight = vehicle_type_turret_compute_target_weight(this, vehicle_index, player->unit_index);
                if (weight > best_weight * switch_scale) {
                    best_weight = weight;
                    best_target_index = player->unit_index;
                }
            }
        } while (player_iterator.next());
    }

    if (m_auto_control.target_object_index != NONE && best_target_index == NONE) {
        if (m_auto_control.target_lost_time == NONE)
            m_auto_control.target_lost_time = game_time_get();
        if (game_time_get() - m_auto_control.target_lost_time <
            (long)(k_turret_target_lost_timeout_seconds * (float)tls_game_time_globals()->ticks_per_second + rounding_half)) {
            object_datum *target = (object_datum *)object_get_and_verify_type(m_auto_control.target_object_index, NONE);
            const real_point3d *target_position = &target->bounding_center;
            if (m_volume_index == NONE || ::scenario_trigger_volume_test_point(m_volume_index, target_position))
                best_target_index = m_auto_control.target_object_index;
        }
    }

    if (m_auto_control.target_object_index != best_target_index) {
        m_auto_control.target_object_index = best_target_index;
        m_auto_control.target_time = game_time_get();
        m_auto_control.minimum_range_start_time = NONE;
        m_auto_control.target_lost_time = NONE;
    }
}

// @address 0xd7c60 vehicle_type_turret_compute_target_weight
extern "C" float vehicle_type_turret_compute_target_weight(c_vehicle_type_turret *turret, int a, int b)
{
    long handle = turret->m_volume_index;
    float result = 0.0f;
    object_datum *object_a = (object_datum *)object_get_and_verify_type(a, NONE);
    object_datum *object_b = (object_datum *)object_get_and_verify_type(b, NONE);
    const real_point3d *point = &object_b->bounding_center;
    if (handle == -1) {
        float dz = 0.0f;
        float dx = object_a->bounding_center.x - point->x;
        dz = object_a->bounding_center.z - point->z;
        float dy = object_a->bounding_center.y - point->y;
        float distance = dy * dy + (dx * dx + dz * dz);
        if (distance < 100.0f)
            result = 100.0f - distance;
    } else if (::scenario_trigger_volume_test_point((int)handle, point)) {
        float dz = 0.0f;
        float dx = object_a->bounding_center.x - point->x;
        dz = object_a->bounding_center.z - point->z;
        float dy = object_a->bounding_center.y - point->y;
        result = dy * dy + (dx * dx + dz * dz);
    }
    return result;
}

// Five degrees in radians through the codebase float factor (0x3c8efa35); the named
// shared constant is unusable here because referencing it forces this double into .data.
// @rdata 0x82058af0
extern "C" {
const double k_turret_track_angle = 5.0 * 0.017453292f;
}

// @address 0xd7d58 ?track_auto_target@c_vehicle_type_turret@@AAAXJ@Z
void c_vehicle_type_turret::track_auto_target(long vehicle_index)
{
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    bool tracking = false;
    bool lost = false;

    if (m_auto_control.target_object_index != NONE) {
        object_datum *target = (object_datum *)object_get_and_verify_type(m_auto_control.target_object_index, NONE);
        real_point3d target_position = target->bounding_center;
        real_point3d aim_position;
        unit_get_aim_position(vehicle_index, &aim_position);
        real_vector3d aim_vector;
        vector_from_points3d(&aim_position, &target_position, &aim_vector);
        float distance = normalize3d(&aim_vector);

        if (distance < m_min_range)
            m_auto_control.minimum_range_start_time = NONE;
        else if (m_auto_control.minimum_range_start_time == NONE)
            m_auto_control.minimum_range_start_time = game_time_get();

        if (m_max_range != 0.0f && distance >= m_max_range) {
            lost = true;
        } else if (m_lost_seconds != 0.0f && m_auto_control.minimum_range_start_time != NONE &&
                   game_time_get() - m_auto_control.minimum_range_start_time >
                       game_seconds_to_ticks_round(m_lost_seconds)) {
            lost = true;
        } else if (distance > m_min_range) {
            tracking = true;
        }

        if (distance > 0.0f) {
            vehicle_datum *unit = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
            long weapon_index = unit->unit.current_weapon_index != NONE ? unit->unit.weapon_object_indices[unit->unit.current_weapon_index] : NONE;
            if (weapon_index != NONE) {
                float time = weapon_estimate_time_to_target(weapon_index, lost != 0, distance);
                real_vector3d velocity;
                object_get_velocities(m_auto_control.target_object_index, &velocity, 0);
                scale_vector3d(&velocity, time, &velocity);
                point_from_line3d(&target_position, &velocity, 1.0f, &target_position);
                vector_from_points3d(&aim_position, &target_position, &aim_vector);
                normalize3d(&aim_vector);
            }
            vehicle->vehicle.forward = aim_vector;
            ::simulation_action_object_update(vehicle_index, 0x8000);
            float dot = dot_product3d(&aim_vector, &vehicle->unit.aiming_vector);
            if (dot < (float)cos(k_turret_track_angle)) {
                lost = false;
                tracking = false;
            }
        }
    }

    if (tracking) vehicle->vehicle.flags |= 0x100; else vehicle->vehicle.flags &= ~0x100;
    if (lost) vehicle->vehicle.flags |= 0x200; else vehicle->vehicle.flags &= ~0x200;
    ::simulation_action_object_update(vehicle_index, 0x4000);
}

// @address 0xd7948 ?auto_control@c_vehicle_type_turret@@QAA_NJ@Z
bool c_vehicle_type_turret::auto_control(long vehicle_index)
{
    bool result = false;
    vehicle_datum *vehicle = (vehicle_datum *)object_get_and_verify_type(vehicle_index, _object_mask_vehicle);
    do {
        if (!bit_test(vehicle->vehicle.flags, 7))
            break;
        bool skip_update = false;
        if (tls_game_globals()->options.simulation_type == 4)
            skip_update = true;
        if (!skip_update) {
            if (m_volume_index != NONE && (m_volume_index < 0 || m_volume_index >= global_scenario->trigger_volumes.count)) {
                m_volume_index = NONE;
                vehicle->vehicle.flags &= ~0x80;
                break;
            }
            update_auto_target(vehicle_index);
            track_auto_target(vehicle_index);
        }
        unsigned short flags = vehicle->vehicle.flags;
        SET_BIT(vehicle->unit.control_flags, 18, bit_test(flags, 8));
        SET_BIT(vehicle->unit.control_flags, 19, bit_test(flags, 9));
        unit_set_facing(vehicle_index, &vehicle->vehicle.forward, &vehicle->vehicle.forward);
        result = true;
    } while (0);
    return result;
}
