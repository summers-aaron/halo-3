#ifndef UNITS_UNIT_TYPES_H
#define UNITS_UNIT_TYPES_H

#include "cseries/cseries_wrapped.h"
#include "math/real_math_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_types.h"
#include "game/game_types.h"
#include "units/unit_control.h"
#include "units/unit_passenger_iterator.h"
#include "units/unit_seat.h"
#include "objects/object_datum.h"
#include "ai/sector_ref.h"

enum e_unit_map_editor_helper_flags { _unit_map_editor_helper_flag_0 };
enum e_unit_control_context { _unit_control_context_0 };

enum e_unit_flags {
    _unit_flag_active_camouflage = 3,     // simulation_game_units: the unit state's active_camo_active mirrors this bit
    _unit_flag_reset_seat_acceleration = 26,
    _unit_flag_no_dual_wield_update = 30,
    _unit_flag_prefer_tight_camera_track = 16,
    _unit_flag_crouching = 22,           // mirrors control flag 0 (crouch); vehicles read it as the descend input
    _unit_flag_actively_controlled = 1,  // unit_set_actively_controlled sets it; update_control reports driver+gunner updated; died clears it
    _unit_flag_active_camouflage_hold = 4, // unit_active_camouflage_enable(hold) sets it; the fade update holds the secondary fade at 1 while set
    _unit_flag_placed_dead = 6,          // unit_place sets it when scenario flags&1 kills the unit at spawn (no readers in tree)
    _unit_flag_not_enterable_by_player = 12, // unit_place sets it from scenario flags&4; consider_nearby_seats ignores such targets for players
    _unit_flag_running_blindly = 20,     // unit_start_running_blindly sets it; update_control runs the blind-steering branch while set
    _unit_flag_suppress_control = 21,    // update_control skips the driver-control copy while set (no setters in tree)
    _unit_flag_acceleration_stun = 24,   // seat_acceleration_update sets it past the stun threshold; actions blocks actions while set
    _unit_flag_exit_vehicle_immediate = 25, // seat_acceleration_update sets it past the eject threshold; update submits _action_vehicle_exit_immediate
    _unit_flag_vision_mode = 27,         // update_vision_mode toggles it with the arbiter sound and timer; died clears it for players
};

// Unit state following object_datum.
struct _unit_datum {
    union {
        s_object_block_reference motor_tasks; // (unit_animation: object_header_block_get(unit, &unit->motor_tasks))
        struct {
            unsigned char pad160;
            unsigned char kind;
        };
    };
    union {
        s_object_block_reference motor_program;
        struct {
            short motor_program_size;
            union {
                short motor_offset;
                short motor_program_offset;
            };
        };
    };
    union {
        s_object_block_reference action;
        struct {
            short action_size;
            short action_offset;
        };
    };
    union {
        long actor_index;
        long actor;
    };
    long simulation_actor_datum_index;
    union {
        unsigned long unit_flags;
        unsigned short unit_flags_words[2];
        unsigned char unit_flags_bytes[4];
        c_flags_no_init<e_unit_flags, unsigned long, 31> m_unit_flags;
    };
    e_game_team team;
    long player_index;
    long controller_player_index;
    long sound_time;
    unsigned long control_flags;
    long animation_state;
    union { real_vector3d facing_vector; real_vector3d desired_facing_vector; };
    real_vector3d desired_aiming_vector;
    union
    {
        real_vector3d aiming_vector;
        real_point3d aiming_position;
    };
    real_vector3d aiming_velocity;
    real_vector3d desired_looking_vector;
    union {
        real_vector3d looking_vector;
        real_vector2d facing;
    };
    real_vector3d looking_velocity;
    real_point3d gaze_position;
    real_vector3d throttle;
    real_vector3d analog_blending_throttle;
    union {
        unsigned char unit_input_flags;
        c_flags_no_init<e_unit_control_context, unsigned char, 8> control_context;
    };
    signed char aiming_speed;
    signed char death_state;
    char pad20b;
    float primary_trigger;
    float secondary_trigger;
    s_aim_assist_targeting_result targeting;
    long persistent_control_ticks;
    union { unsigned long forced_control_flags; unsigned long persistent_control_flags; };
    union { signed char weapon_change_ticks; signed char berserk_charges; };
    signed char left_eye_marker;
    signed char right_eye_marker;
    unsigned char looking_change;
    unsigned char dual_error_ticks[2];
    short parent_seat_index;
    float ambient_illumination;
    float self_illumination;
    float mouth_aperture;
    float mouth_time;
    union {
        s_unit_weapon_set current_weapon_set;
        // simulation_action_pickup_weapon transfers the complete set in one word.
        unsigned long current_weapon_set_storage;
        struct {
            short weapon_set_identifier;
            union {
                struct {
                    signed char current_weapon_index;
                    union { signed char desired_weapon_index; signed char secondary_weapon_index; };
                };
                signed char current_weapon_indices[2];
            };
        };
    };
    union {
        s_unit_weapon_set desired_weapon_set;
        unsigned long desired_weapon_set_storage; // Complete 4-byte transfer used by simulation updates.
        struct {
            short desired_weapon_set_identifier;
            union {
                signed char desired_weapon_indices[2];
                signed char hidden_weapon_index;
            };
        };
    };
    union {
        long weapon_object_indices[4];
        long attachments[4];
    };
    long weapon_ages[4];
    long equipment_object_index;
    long equipment_pickup_time;
    short sound_type;
    signed char current_grenade_index;
    signed char desired_grenade_index;
    signed char grenade_counts[4];
    signed char zoom_level;
    signed char desired_zoom_level;
    char pad292;
    unsigned char aiming_change;
    long last_weapon_control_time;
    long driver_object_index;
    long gunner_object_index;
    long last_vehicle_index;
    long last_vehicle_exit_time;
    short last_squad_index;
    union {
        bool seat_exit_queued;                      // cleared when a seat exit starts (unit_exit_submit) and when a seat entry finishes
        signed char seat_impulse_ticks;
    };
    // Enabled seat-acceleration definitions, one bit per definition.
    signed char seat_acceleration_mask;
    union {
        struct {
            union {
                float seat_acceleration_fractions[2];
                float throttle_scale;
            };
            float vision_mode_fraction;
            float vision_mode_timer;
        };
        float deplete[4];
    };
    float zoom_transition_timer;
    real_vector3d seat_acceleration;
    real_point3d seat_acceleration_history[6];
    short seat_acceleration_history_index;
    long seat_acceleration_reset_time;
    union {
        short simulation_actor_index;
        short predicted_vehicle_seat_index;
    };
    short pad31e;
    union {
        long simulation_entity_index;
        long predicted_vehicle_index;
    };
    long simulation_player_index;
    long simulation_player_identifier;
    union { long simulation_actor_identifier; long simulation_actor_squad_index; };
    union { long simulation_control_index; long simulation_actor_variant_index; };
    union { long simulation_control_identifier; long simulation_actor_starting_location_index; };
    s_unit_predicted_weapon_state predicted_weapon_states[4];
    union { float active_camouflage; float active_camo_regrowth; };
    union { float active_camouflage_target; float active_camo_maximum; };
    float active_camouflage_secondary_fade;
    union { float active_camouflage_regrowth; float active_camo_level; };
    c_flags_no_init<e_unit_map_editor_helper_flags, unsigned char, 1> map_editor_flags;
    short stun_ticks;
    short emp_shield_stun_ticks;
    short pad36e;
    union { float crouch_amount; float powered_seat_power; };                        // (motor_system_biped)
    unsigned short damage_type;
    short damage_timer;
    float damage;
    long damaging_object_index;
    long flaming_death_attacker_object_index;
    float running_blind_angle;
    float running_blind_rate;
    union {
        long time_of_death;
        long last_active_time;
        long type_change_time;                    // actor_prop_state_refresh delays a changed prop type
    };
    float look_difficulty_scale;
    short damage_stun_ticks;
    short killing_spree_count;
    long killing_spree_time;
    s_unit_damage_record damage_records[4];
    unsigned char damage_record_flags[4];
    union { float boost_time; float vehicle_steering; };
    union { float boost_decay_time; float vehicle_throttle; };
    long last_enemies_moving_nearby_tick;
    long last_damage_time;
    // First contact tick in the current soft-ceiling damage interval.
    long soft_ceiling_contact_time;
    long last_in_phantom_volume_time;
    s_object_block_reference animation_storage; // (s_unit_animation: unit_animation reads it, hs_function called its offset animation_storage_offset)
    union {
        s_object_block_reference dialogue_reference;
        struct { short dialogue_size; short dialogue_offset; };
    };
    unsigned long flags400;
    c_sector_ref pathfinding_attached_sector;            // (March unit_get_pathfinding_get_attached_sector; valid while flags400 bit 0 is set)
    long debug_unit_input[16];
};

typedef char unit_types_deplete_offset[(offsetof(_unit_datum, deplete) == 0x14c) ? 1 : -1];
typedef char unit_types_deplete_size[(sizeof(((_unit_datum *)0)->deplete) == 0x10) ? 1 : -1];
typedef char unit_types_sound_time_offset[(offsetof(_unit_datum, sound_time) == 0x24) ? 1 : -1];
typedef char unit_types_sound_type_offset[(offsetof(_unit_datum, sound_type) == 0x128) ? 1 : -1];
typedef char unit_types_secondary_camouflage_fade_offset[(offsetof(_unit_datum, active_camouflage_secondary_fade) == 0x200) ? 1 : -1];
typedef char unit_types_last_damage_time_offset[(offsetof(_unit_datum, last_damage_time) == 0x28c) ? 1 : -1];

// Biped, vehicle and giant state follows this common unit state.
struct unit_datum {
    object_datum object;
    _unit_datum unit;
};
typedef unit_datum s_unit_datum;

typedef char unit_types_last_vehicle_offset[(offsetof(unit_datum, unit.last_vehicle_index) == 0x2a0) ? 1 : -1];
typedef char unit_types_last_vehicle_exit_offset[(offsetof(unit_datum, unit.last_vehicle_exit_time) == 0x2a4) ? 1 : -1];
typedef char unit_types_seat_impulse_ticks_offset[(offsetof(unit_datum, unit.seat_impulse_ticks) == 0x2aa) ? 1 : -1];
typedef char unit_types_seat_acceleration_mask_offset[(offsetof(_unit_datum, seat_acceleration_mask) == 0x14b) ? 1 : -1];
typedef char unit_types_soft_ceiling_contact_time_offset[(offsetof(_unit_datum, soft_ceiling_contact_time) == 0x290) ? 1 : -1];

typedef char unit_types_desired_weapon_storage_offset[(offsetof(unit_datum, unit.desired_weapon_set_storage) == 0x25c) ? 1 : -1];

typedef char unit_types_current_weapon_storage_offset[(offsetof(unit_datum, unit.current_weapon_set_storage) == 0x258) ? 1 : -1];

typedef char unit_types_size_unit_datum[(sizeof(unit_datum) == 0x448) ? 1 : -1];
typedef char unit_types_offset_unit[(offsetof(unit_datum, unit) == 0x160) ? 1 : -1];
typedef char unit_types_offset_last_active_time[(offsetof(unit_datum, unit.last_active_time) == 0x38c) ? 1 : -1];
typedef char unit_types_offset_type_change_time[(offsetof(unit_datum, unit.type_change_time) == 0x38c) ? 1 : -1];
typedef char unit_types_offset_player_index[(offsetof(unit_datum, unit.player_index) == 0x17c) ? 1 : -1];
typedef char unit_types_offset_seat_acceleration[(offsetof(unit_datum, unit.seat_acceleration) == 0x2c0) ? 1 : -1];
typedef char unit_types_offset_acceleration_history[(offsetof(unit_datum, unit.seat_acceleration_history) == 0x2cc) ? 1 : -1];
typedef char unit_types_offset_acceleration_cursor[(offsetof(unit_datum, unit.seat_acceleration_history_index) == 0x314) ? 1 : -1];
typedef char unit_types_offset_acceleration_reset[(offsetof(unit_datum, unit.seat_acceleration_reset_time) == 0x318) ? 1 : -1];
typedef char unit_types_offset_motor_program[(offsetof(unit_datum, unit.motor_program) == 0x164) ? 1 : -1];
typedef char unit_types_offset_action[(offsetof(unit_datum, unit.action) == 0x168) ? 1 : -1];
typedef char unit_types_offset_mouth_aperture[(offsetof(unit_datum, unit.mouth_aperture) == 0x250) ? 1 : -1];
typedef char unit_types_offset_ambient_illumination[(offsetof(unit_datum, unit.ambient_illumination) == 0x248) ? 1 : -1];
typedef char unit_types_offset_self_illumination[(offsetof(unit_datum, unit.self_illumination) == 0x24c) ? 1 : -1];
typedef char unit_types_offset_mouth_time[(offsetof(unit_datum, unit.mouth_time) == 0x254) ? 1 : -1];
typedef char unit_types_offset_map_editor_flags[(offsetof(unit_datum, unit.map_editor_flags) == 0x368) ? 1 : -1];
typedef char unit_types_offset_stun_ticks[(offsetof(unit_datum, unit.stun_ticks) == 0x36a) ? 1 : -1];
typedef char unit_types_offset_flaming_death_attacker[(offsetof(unit_datum, unit.flaming_death_attacker_object_index) == 0x380) ? 1 : -1];
typedef char unit_types_offset_time_of_death[(offsetof(unit_datum, unit.time_of_death) == 0x38c) ? 1 : -1];
typedef char unit_types_offset_last_nearby_enemies[(offsetof(unit_datum, unit.last_enemies_moving_nearby_tick) == 0x3e8) ? 1 : -1];
typedef char unit_types_offset_last_phantom_volume[(offsetof(unit_datum, unit.last_in_phantom_volume_time) == 0x3f4) ? 1 : -1];
typedef char unit_types_offset_debug_unit_input[(offsetof(unit_datum, unit.debug_unit_input) == 0x408) ? 1 : -1];

typedef char unit_types_offset_persistent_control[(offsetof(unit_datum, unit.persistent_control_ticks) == 0x238) ? 1 : -1];
typedef char unit_types_offset_dialogue[(offsetof(unit_datum, unit.dialogue_reference) == 0x3fc) ? 1 : -1];

typedef char unit_desired_aiming_offset[(offsetof(unit_datum, unit.desired_aiming_vector) == 0x19c) ? 1 : -1];
typedef char unit_desired_looking_offset[(offsetof(unit_datum, unit.desired_looking_vector) == 0x1c0) ? 1 : -1];

typedef char unit_input_flags_offset[(offsetof(unit_datum, unit.unit_input_flags) == 0x208) ? 1 : -1];
typedef char unit_look_difficulty_scale_offset[(offsetof(unit_datum, unit.look_difficulty_scale) == 0x390) ? 1 : -1];

typedef char unit_flags_byte_offset[(offsetof(unit_datum, unit.unit_flags_bytes) == 0x174) ? 1 : -1];

typedef char unit_types_offset_analog_blending_throttle[(offsetof(unit_datum, unit.analog_blending_throttle) == 0x1fc) ? 1 : -1];

typedef char unit_flags_words_offset[(offsetof(unit_datum, unit.unit_flags_words) == 0x174) ? 1 : -1];

typedef char unit_vehicle_steering_offset[(offsetof(unit_datum, unit.vehicle_steering) == 0x3e0) ? 1 : -1];
typedef char unit_vehicle_throttle_offset[(offsetof(unit_datum, unit.vehicle_throttle) == 0x3e4) ? 1 : -1];

#endif // UNITS_UNIT_TYPES_H

