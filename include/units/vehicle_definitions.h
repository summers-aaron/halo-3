#ifndef UNITS_VEHICLE_DEFINITIONS_H
#define UNITS_VEHICLE_DEFINITIONS_H

#include <stddef.h>
#include "units/unit_seat.h"
#include "physics/havok_vehicle_physics.h"
#include "units/unit_definitions.h"

struct s_vehicle_human_jeep_definition;

enum e_vehicle_definition_flags
{
    _vehicle_definition_weapon_vector = 3
};

struct vehicle_definition : unit_definition
{
    union {
        unsigned long flags;
        c_flags_no_init<e_vehicle_definition_flags, unsigned long, 12> definition_flags;
    };
    union {
        struct {
            s_tag_block human_tank;
            long jeep_count;
            TAG_POINTER(s_vehicle_human_jeep_definition) jeep;
            TAG_POINTER(void) jeep_definition;
            s_tag_block human_plane;
            s_tag_block alien_scout;
            s_tag_block alien_fighter;
            s_tag_block turret;
            s_tag_block mantis;
            s_tag_block vtol;
            s_tag_block chopper;
            s_tag_block guardian;
        };
        s_tag_block types[10];
    };
    s_havok_vehicle_physics_definition havok_vehicle_physics;
    float powered_seat_power_rate;                         // vehicle_program_update, +0x3f8
    float seat_exit_impulse;
    float seat_enter_impulse;
    float damaged_threshold;                              // vehicle_program_update, +0x404
    long flip_message;
    s_tag_reference suspension_sound;
    s_tag_reference effect;
    s_tag_reference boost_rider_damage;
    s_tag_reference boost_rider_response;
};

typedef char vehicle_definition_radius_offset[(offsetof(vehicle_definition, bounding_radius) == 4) ? 1 : -1];

typedef char vehicle_definition_model_offset[(offsetof(vehicle_definition, model.index) == 0x40) ? 1 : -1];
typedef char vehicle_definition_ai_properties_offset[(offsetof(vehicle_definition, ai_properties) == 0xa0) ? 1 : -1];
typedef char vehicle_definition_boost_peak_power_offset[(offsetof(vehicle_definition, boost_peak_power) == 0x2d0) ? 1 : -1];
typedef char vehicle_definition_flags_offset[(offsetof(vehicle_definition, flags) == 0x30c) ? 1 : -1];
typedef char vehicle_definition_human_tank_offset[(offsetof(vehicle_definition, human_tank) == 0x310) ? 1 : -1];
typedef char vehicle_definition_jeep_offset[(offsetof(vehicle_definition, jeep) == 0x320) ? 1 : -1];
typedef char vehicle_definition_guardian_offset[(offsetof(vehicle_definition, guardian) == 0x37c) ? 1 : -1];
typedef char vehicle_definition_physics_offset[(offsetof(vehicle_definition, havok_vehicle_physics) == 0x388) ? 1 : -1];
typedef char vehicle_definition_gravity_scale_offset[(offsetof(vehicle_definition, havok_vehicle_physics.gravity_scale) == 0x3b0) ? 1 : -1];
typedef char vehicle_definition_friction_count_offset[(offsetof(vehicle_definition, havok_vehicle_physics.friction_points.count) == 0x3d0) ? 1 : -1];
typedef char vehicle_definition_size_offset[(offsetof(vehicle_definition, havok_vehicle_physics.size) == 0x3e9) ? 1 : -1];

typedef char vehicle_definition_types_offset[(offsetof(vehicle_definition, types) == 0x310) ? 1 : -1];

typedef char vehicle_definition_effect_offset[(offsetof(vehicle_definition, effect.index) == 0x428) ? 1 : -1];

typedef char vehicle_power_rate_offset[(offsetof(vehicle_definition, powered_seat_power_rate) == 0x3f8) ? 1 : -1];
typedef char vehicle_damage_threshold_offset[(offsetof(vehicle_definition, damaged_threshold) == 0x404) ? 1 : -1];
typedef char vehicle_suspension_sound_offset[(offsetof(vehicle_definition, suspension_sound.index) == 0x418) ? 1 : -1];

typedef char vehicle_definition_seats_offset[(offsetof(vehicle_definition, seats) == 0x2a0) ? 1 : -1];
typedef char vehicle_flip_message_offset[(offsetof(vehicle_definition, flip_message) == 0x408) ? 1 : -1];
typedef char vehicle_seat_exit_impulse_offset[(offsetof(vehicle_definition, seat_exit_impulse) == 0x3fc) ? 1 : -1];
typedef char vehicle_seat_enter_impulse_offset[(offsetof(vehicle_definition, seat_enter_impulse) == 0x400) ? 1 : -1];
typedef char vehicle_boost_rider_damage_offset[(offsetof(vehicle_definition, boost_rider_damage.index) == 0x438) ? 1 : -1];
typedef char vehicle_boost_rider_response_offset[(offsetof(vehicle_definition, boost_rider_response.index) == 0x448) ? 1 : -1];

#endif
