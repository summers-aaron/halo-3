#ifndef UNITS_BIPED_TYPES_H
#define UNITS_BIPED_TYPES_H

#include <stddef.h>

#include "units/unit_types.h"
#include "ai/sector_ref.h"
#include "physics/character_physics_component.h"
#include "math/matrix_math_types.h"

enum e_biped_flag {
    _biped_flag_3 = 3,
    _biped_kill_volume_damaged = 4,
    _biped_out_of_map = 5,
    _biped_moving_turn_constraints = 6,
    // Set by the climb program before a hard ping and by an interrupted custom animation; the dead program reads
    // them: 11 lets relaxation start without a relaxable contact, 12 resets the airborne state on activation.
    _biped_dead_relax_without_contact = 11,
    _biped_dead_reset_airborne_state = 12
};

struct _biped_datum {
    union {
        unsigned short flags;
        unsigned char flags_bytes[2];
        c_flags_no_init<e_biped_flag, unsigned short, 11> m_flags;
    };
    short pathfinding_structure_index;
    long pathfinding_time;
    long pathfinding_surface_index;
    c_sector_ref pathfinding_sector;
    long pathfinding_instanced_geometry_index;              // (March biped_verify_surface_reference assert)
    long pathfinding_object_index;
    unsigned long pathfinding_bsp_reference;                // (same assert)
    real_point3d position;
    real_vector3d ground_normal;
    real_point3d ground_point;
    long last_airborne_speech_time;
    long bump_object_index;
    signed char bump_ticks;
    signed char jump_control_ticks;
    unsigned char stun_ticks;
    signed char footstep_volume;
    float lean_input;
    // Filler between the lean input and the camera offset; no in-tree reader uses these bytes.
    char camera_offset_padding[0x4b4 - 0x498];
    real_vector3d camera_offset;
    c_character_physics_component physics;
    // Filler between the physics component and the simulation interpolation block; no in-tree reader uses these bytes.
    char interpolation_padding[0x540 - 0x53c];
    // Simulation interpolation storage.
    s_object_block_reference simulation_interpolation;
    real_point3d following_camera_offset;
    real_vector3d previous_ground_normal;
    float moving_turn_scale;
    float moving_turn_remaining_angle;
    real_point3d moving_turn_position;
    real_vector2d velocity;
    real_matrix4x3 camera_c;
    real_matrix4x3 camera_d;
    union {
        struct {
            real_point3d leap_destination;                  // (leap_anticipation_submit)
            real_vector3d leap_velocity;
        };
        struct {
            real_vector3d leap_recovery_velocity;           // (leap_anticipation_finished)
            real_vector3d leap_recovery_normal;
        };
    };
    union {
        struct {
            real_matrix4x3 camera_a;
            real_matrix4x3 camera_b;
            real_matrix4x3 camera_e;
        };
        real_matrix4x3 node_matrix_history[3];
    };
    float landing_root_offset;
    float aiming_vector_magnitude;
    real_vector3d aiming_vector;
    union { float scale; float gravity_scale; };
    long swarm_actor_index;                                 // (hs_function swarm branch)
    short squad_index;
    unsigned char falling_danger_cooldown;
    unsigned char combat_status;
};

// Biped state extends the common object and unit state.
struct biped_datum {
    object_datum object;
    _unit_datum unit;
    _biped_datum biped;
};

typedef char biped_types_size[(sizeof(biped_datum) == 0x6b4) ? 1 : -1];
typedef char biped_types_speech_time_offset[(offsetof(biped_datum, biped.last_airborne_speech_time) == 0x488) ? 1 : -1];
typedef char biped_types_falling_cooldown_offset[(offsetof(biped_datum, biped.falling_danger_cooldown) == 0x6b2) ? 1 : -1];
typedef char biped_types_offset_unit[(offsetof(biped_datum, unit) == 0x160) ? 1 : -1];
typedef char biped_types_offset_biped[(offsetof(biped_datum, biped) == 0x448) ? 1 : -1];
typedef char biped_types_offset_physics[(offsetof(biped_datum, biped.physics) == 0x4c0) ? 1 : -1];
typedef char biped_types_offset_camera[(offsetof(biped_datum, biped.following_camera_offset) == 0x544) ? 1 : -1];
typedef char biped_types_offset_leap_destination[(offsetof(biped_datum, biped.leap_destination) == 0x5e0) ? 1 : -1];
typedef char biped_types_offset_leap_velocity[(offsetof(biped_datum, biped.leap_velocity) == 0x5ec) ? 1 : -1];
typedef char biped_types_offset_swarm_actor[(offsetof(biped_datum, biped.swarm_actor_index) == 0x6ac) ? 1 : -1];

typedef char biped_types_offset_gravity_scale[(offsetof(biped_datum, biped.gravity_scale) == 0x6a8) ? 1 : -1];
typedef char biped_types_offset_squad[(offsetof(biped_datum, biped.squad_index) == 0x6b0) ? 1 : -1];

typedef char biped_types_interpolation_offset[(offsetof(biped_datum, biped.simulation_interpolation) == 0x540) ? 1 : -1];

#endif // UNITS_BIPED_TYPES_H
