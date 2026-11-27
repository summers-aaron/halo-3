#ifndef GAME_PLAYER_CONTROL_TYPES_H
#define GAME_PLAYER_CONTROL_TYPES_H
#include "game/player_control_globals.h"

#include <stddef.h>
#include "cseries/cseries.h"
#include "math/matrix_math_types.h"
#include "game/player_interaction.h"
#include "units/unit_control.h"

// Controller collection, desired-angle updates and action submission share these fields.
struct control {
    long flags;
    unsigned short action_flags;
    real_euler_angles2d look;
    real_point2d move;
    float primary, secondary;
    s_unit_weapon_set weapons;
    short grenade, zoom;
    s_player_action_context action_context;
    s_aim_assist_targeting_result aim_assist;
    union { unsigned char byte; bool look_override; };
    union { real_point2d extra; real_point2d look_override_value; };
    union { short extra_short; unsigned short look_flags; };
};
struct s_player_control_user_state
{
    long unit_index;
    control action;
    bool autolevel;
    float autolevel_time;
    real_matrix4x3 early_mover_transform;
    union { long target_unit_index; long early_mover_object; };
    bool aim_assist_valid;
    float yaw_acceleration_time;
    float pitch_acceleration_time;
    float minimum_pitch;
    float maximum_pitch;
    bool crouch_locked;
    short crouch_frames;
    bool zoom_held;
    short zoom_previous_level;
    short zoom_held_ticks;
    bool switch_weapon_held;
    bool switch_weapon_consumed;
    bool switch_grenade_held;
    bool hud_toggled;
    float pitch_kick;
    long pitch_kick_ticks;
    unsigned char active;
    real_point3d position;
    float angle;
};
struct s_player_control_state
{
    long unit_index;
    control action;
};
struct s_player_control_globals {
    unsigned long input_flags;
    unsigned long input_locked_flags;
    unsigned long input_held_flags;
    union { unsigned long input_released_flags; unsigned long input_disabled_flags; };
    s_player_control_user_state users[4];
    s_player_control_state states[4];
    bool enabled;
    float a, b, c, d, e;
    bool thread_active;
    bool thread_enabled;
    bool thread_unknown58a;
    bool flag;
};
typedef char player_control_control_size[(sizeof(control) == 0x68) ? 1 : -1];
typedef char player_control_user_state_size[(sizeof(s_player_control_user_state) == 0xec) ? 1 : -1];
typedef char player_control_state_size[(sizeof(s_player_control_state) == 0x6c) ? 1 : -1];
typedef char player_control_user_early_mover_transform_offset[(offsetof(s_player_control_user_state, early_mover_transform) == 0x74) ? 1 : -1];
typedef char player_control_user_early_mover_object_offset[(offsetof(s_player_control_user_state, target_unit_index) == 0xa8) ? 1 : -1];
typedef char player_control_user_minimum_pitch_offset[(offsetof(s_player_control_user_state, minimum_pitch) == 0xb8) ? 1 : -1];
typedef char player_control_user_pitch_kick_offset[(offsetof(s_player_control_user_state, pitch_kick) == 0xd0) ? 1 : -1];
typedef char player_control_user_target_active_offset[(offsetof(s_player_control_user_state, active) == 0xd8) ? 1 : -1];
typedef char player_control_globals_size[(sizeof(s_player_control_globals) == 0x58c) ? 1 : -1];


struct s_player_control_input {
    real_point2d throttle;
    float primary_trigger;
    float secondary_trigger;
    float yaw_delta;
    float pitch_delta;
    unsigned long action_flags;
    unsigned long control_flags;
    unsigned short button_flags;
    float vehicle_pitch;
    union {
        long suppressed_actions[9];
        s_aim_assist_targeting_result aim_assist;
    };
};

typedef char player_control_input_size[(sizeof(s_player_control_input) == 0x4c) ? 1 : -1];

#endif
