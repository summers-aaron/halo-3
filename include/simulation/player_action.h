#ifndef SIMULATION_PLAYER_ACTION_H
#define SIMULATION_PLAYER_ACTION_H

#include "units/unit_control.h"
#include "game/player_interaction.h"
#include "game/player_types.h"

struct player_action
{
    long unit_valid;
    unsigned char unit_flags;
    unsigned long flags;
    real_euler_angles2d facing;
    union { real_point2d move; real_vector2d move_vector; s_player_input_pair target; };
    union { unsigned char byte; bool map_editor_rotation_valid; };
    union { real_point2d extra; s_player_input_pair extra_input; };
    unsigned short extra_short;
    float primary, secondary;
    unsigned short action_flags;
    s_unit_weapon_set weapons;
    short grenade;
    short zoom;
    s_player_action_context action_context;
    s_aim_assist_targeting_result aim_assist;
    bool velocity_exceeds_motion_tracker_threshold;
};

typedef char player_action_size[(sizeof(player_action) == 0x74) ? 1 : -1];
typedef char player_action_flags_offset[(offsetof(player_action, action_flags) == 0x34) ? 1 : -1];
typedef char player_action_zoom_offset[(offsetof(player_action, zoom) == 0x3c) ? 1 : -1];
typedef char player_action_context_offset[(offsetof(player_action, action_context) == 0x40) ? 1 : -1];
typedef char player_action_aim_assist_offset[(offsetof(player_action, aim_assist) == 0x4c) ? 1 : -1];
typedef char player_action_velocity_offset[(offsetof(player_action, velocity_exceeds_motion_tracker_threshold) == 0x70) ? 1 : -1];

typedef char player_action_target_offset[(offsetof(player_action, target) == 0x14) ? 1 : -1];
typedef char player_action_move_vector_offset[(offsetof(player_action, move_vector) == 0x14) ? 1 : -1];
typedef char player_action_extra_input_offset[(offsetof(player_action, extra_input) == 0x20) ? 1 : -1];

#endif // SIMULATION_PLAYER_ACTION_H
