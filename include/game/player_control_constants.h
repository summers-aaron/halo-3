#ifndef GAME_PLAYER_CONTROL_CONSTANTS_H
#define GAME_PLAYER_CONTROL_CONSTANTS_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

// One element of the globals tag's player control block (game_globals->player_control).
struct s_player_control_constants
{
    float magnetism_scale;
    float magnetism_limit;
    float aim_assist_scale;
    char padc[0x18 - 0xc];
    real_point2d crosshair_location;
    float sprint_ticks_seconds;
    float sprint_recovery_seconds;
    float sprint_recovery_rate;
    float sprint_speed_scale;
    float sprint_minimum_throttle;
    float sprint_maximum_angle;
    float stamina_deplete_restore_time;
    float sprint_cooldown_time;
    float look_default_pitch_rate;
    float look_default_yaw_rate;
    float look_acceleration_threshold;
    float look_yaw_acceleration_time;
    float look_yaw_acceleration_scale;
    float look_pitch_acceleration_time;
    float look_pitch_acceleration_scale;
    float recenter_scale;
    char pad60[8];
    float gravity_scale;
    char pad6c[2];
    short minimum_autolevelling_ticks;
    float vehicle_flip_angle;
    s_tag_block look_function;
    float minimum_hold_time;
    float crouch_look_deadzone;
};

typedef char player_control_constants_crosshair_offset[(offsetof(s_player_control_constants, crosshair_location) == 0x18) ? 1 : -1];

typedef char player_control_aim_assist_scale_offset[(offsetof(s_player_control_constants, aim_assist_scale) == 8) ? 1 : -1];

typedef char player_control_recenter_scale_offset[(offsetof(s_player_control_constants, recenter_scale) == 0x5c) ? 1 : -1];

typedef char player_control_vehicle_flip_offset[(offsetof(s_player_control_constants, vehicle_flip_angle) == 0x70) ? 1 : -1];

typedef char player_control_sprint_ticks_offset[(offsetof(s_player_control_constants, sprint_ticks_seconds) == 0x20) ? 1 : -1];
typedef char player_control_sprint_speed_scale_offset[(offsetof(s_player_control_constants, sprint_speed_scale) == 0x2c) ? 1 : -1];
typedef char player_control_sprint_angle_offset[(offsetof(s_player_control_constants, sprint_maximum_angle) == 0x34) ? 1 : -1];
typedef char player_control_look_function_offset[(offsetof(s_player_control_constants, look_function) == 0x74) ? 1 : -1];
typedef char player_control_minimum_hold_time_offset[(offsetof(s_player_control_constants, minimum_hold_time) == 0x80) ? 1 : -1];

// Addressable turn constants shared with biped and HUD angle wrapping.
extern float k_player_control_full_turn;
extern float k_player_control_half_turn;

#endif // GAME_PLAYER_CONTROL_CONSTANTS_H
