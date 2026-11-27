#ifndef INTERFACE_FIRST_PERSON_WEAPON_DATA_TYPES_H
#define INTERFACE_FIRST_PERSON_WEAPON_DATA_TYPES_H

#include "animations/animation_manager.h"
#include "math/matrix_math_types.h"
#include <stddef.h>

// First-person weapon and per-user state field layouts.
// These complete storage layouts replace effects.cpp's three overlapping views.
struct s_first_person_weapon_data
{
    unsigned long flags;
    long object_indices[10];
    c_animation_manager animations;
    c_animation_channel extra_channels[2];
    c_animation_id ids[3];
    float fire_overlay_weight;
    float firing_time;
    short idle_timer;
    short idle_delay;
    short sound_timer;
    short state_timer;
    long weapon_node_remap_count, hands_node_remap_count;
    long weapon_node_remapping[64], hands_node_remapping[64];
    long hands_node_secondary_remapping[64];
    short weapon_root_node_index, opposite_hand_node_index;
    long node_count;
    long node_matrix_count;
    real_matrix4x3 node_matrices[64];
    long hands_node_count;
    real_matrix4x3 camera_adjustment;
    long weapon_index, unknown_index, sound_index;
    bool pending_suppress;
};

struct s_first_person_weapon_user_data
{
    long flags;
    long unit_index;
    long object_indices[10];
    s_first_person_weapon_data weapons[2];
    float elapsed, duration;
    short type, control_flags;
    char camera_transition[12];
    float movement_yaw, movement_pitch;
    float movement_yaw_velocity, movement_pitch_velocity;
    float turn_yaw, turn_pitch;
    float turn_yaw_velocity, turn_pitch_velocity;
    real_euler_angles2d facing_angles;
    real_euler_angles2d facing_delta;
    real_matrix4x3 camera_matrix;
    long unknown_index, unknown_index2;
    c_animation_id id;
};

typedef char first_person_weapon_data_size[(sizeof(s_first_person_weapon_data) == 0x11bc) ? 1 : -1];
typedef char first_person_weapon_object_index_offset[(offsetof(s_first_person_weapon_data, object_indices) + 2 * sizeof(long) == 0xc) ? 1 : -1];
typedef char first_person_weapon_data_nodes_offset[(offsetof(s_first_person_weapon_data, node_matrices) == 0x474) ? 1 : -1];
typedef char first_person_weapon_user_data_size[(sizeof(s_first_person_weapon_user_data) == 0x2430) ? 1 : -1];
typedef char first_person_weapon_user_weapons_offset[(offsetof(s_first_person_weapon_user_data, weapons) == 0x30) ? 1 : -1];
typedef char first_person_weapon_user_camera_offset[(offsetof(s_first_person_weapon_user_data, camera_matrix) == 0x23f0) ? 1 : -1];

#endif
