#ifndef UNITS_UNIT_SEAT_H
#define UNITS_UNIT_SEAT_H

#include <stddef.h>

#include "tag_files/tag_types.h"
#include "units/unit_camera.h"

struct unit_seat_acceleration {
    float forward_scale;
    float side_scale;
    float up_scale;
    float stun_threshold;
    float eject_threshold;
};

struct unit_seat {
    unsigned long flags;
    long label;
    long marker_name;
    long entry_marker_name;
    long boarding_grenade_marker;
    long boarding_grenade_string;
    long boarding_melee_string;
    long detach_weapon_string;
    float soft_ping_weight;
    float speech_delay;
    unit_seat_acceleration acceleration;
    float scariness;                              // actor_get_inherent_scariness adds the occupied seat's value
    short type;
    union {
        unsigned short linked_seat;
        short index;
    };
    float listener_interpolation; // sound refresh_listener: blends head position and view orientation.
    float yaw_rate_bounds[2];
    float pitch_rate_bounds[2];
    float interpolation_ticks;
    float speed_reference_bounds[2];
    float speed_exponent;
    union {
        s_unit_camera camera;
        struct {
            unsigned short camera_flags;
            long camera_marker_name;
            char camera_reserved[0x3c - 8];
        };
    };
    c_typed_tag_block<s_tag_reference> hud_interfaces;
    long enter_seat_string;
    float left_aiming_angle;
    float right_aiming_angle;
    s_tag_reference built_in_gunner;
    float entry_radius;
    float entry_marker_angle;
    float entry_unit_angle;
    float entry_max_velocity;
    string_id invisible_seat_region;
    __forceinline bool test_flag(long bit) const { return (flags & (1 << bit)) != 0; }
    __forceinline bool is_driver_seat() const { return (flags & 4) != 0; }

    long region_index;                        // (NONE when the seat has no region)
};
typedef unit_seat s_unit_seat;

struct s_unit_predicted_weapon_state {
    short rounds_loaded;
    short rounds_inventory;
    float age;
};

struct s_unit_damage_record {
    unsigned long time;
    float damage;
    long object_index;
    long player_index;
};

typedef char unit_seat_listener_interpolation_offset[(offsetof(unit_seat, listener_interpolation) == 0x44) ? 1 : -1];

typedef char unit_seat_type_offset[(offsetof(unit_seat, type) == 0x40) ? 1 : -1];

typedef char unit_seat_yaw_rate_bounds_offset[(offsetof(unit_seat, yaw_rate_bounds) == 0x48) ? 1 : -1];
typedef char unit_seat_interpolation_ticks_offset[(offsetof(unit_seat, interpolation_ticks) == 0x58) ? 1 : -1];
typedef char unit_seat_camera_offset[(offsetof(unit_seat, camera) == 0x68) ? 1 : -1];

typedef char unit_seat_size_accel[(sizeof(unit_seat_acceleration) == 20) ? 1 : -1];
typedef char unit_seat_size_seat[(sizeof(unit_seat) == 0xe4) ? 1 : -1];
typedef char unit_seat_built_in_gunner_offset[(offsetof(unit_seat, built_in_gunner) == 0xbc) ? 1 : -1];
typedef char unit_seat_invisible_seat_region_offset[(offsetof(unit_seat, invisible_seat_region) == 0xdc) ? 1 : -1];
typedef char unit_seat_offset_scariness[(offsetof(unit_seat, scariness) == 0x3c) ? 1 : -1];
typedef char unit_seat_offset_hud_interfaces[(offsetof(unit_seat, hud_interfaces) == 0xa4) ? 1 : -1];
typedef char unit_seat_size_pred[(sizeof(s_unit_predicted_weapon_state) == 8) ? 1 : -1];
typedef char unit_seat_size_dmg[(sizeof(s_unit_damage_record) == 16) ? 1 : -1];

typedef char unit_seat_speech_delay_offset[(offsetof(unit_seat, speech_delay) == 0x24) ? 1 : -1];

typedef char unit_seat_boarding_grenade_marker_offset[(offsetof(unit_seat, boarding_grenade_marker) == 0x10) ? 1 : -1];
typedef char unit_seat_enter_seat_string_offset[(offsetof(unit_seat, enter_seat_string) == 0xb0) ? 1 : -1];

#endif // UNITS_UNIT_SEAT_H
