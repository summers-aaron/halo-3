#ifndef AI_COMMAND_SCRIPT_TYPES_H
#define AI_COMMAND_SCRIPT_TYPES_H

#include <stddef.h>
#include "memory/data_types.h"
#include "ai/cl_engine.h"

struct cs_target_specification
{
    short type;
    long index;
};

// The three command channels and their persistent controls occupy 0x24..0x114.
// hs_function, cl_engine and ai_atom_scripting share the state below.
// March actor_get_task_style reads the same style flag/index, four bytes earlier.
struct command_script_datum : s_datum_header
{
    short type;
    long actor_index;
    long script_index;
    long thread_index;
    long next_cs;
    bool reserved;
    bool abort;
    bool reserve_mandatory;
    bool abort_on_alert;
    bool abort_on_damage;
    bool abort_on_combat_status;
    short abort_on_combat_status_level;
    bool abort_on_vehicle_exit;
    short role_index;
    short priority;
    cs_command command[3];
    s_atom_control control[3];
    bool shoot_at_target;
    bool shoot_secondary_trigger;
    bool script_initiated_posture;
    bool script_initiated_custom_animation;
    bool walk;
    bool crouch;
    float crouch_speed;
    bool look_at_target;
    cs_target_specification look_target;
    bool face_exactly;
    bool face_lower_weapon;
    bool aim_at_target;
    bool face_at_target;
    cs_target_specification aim_target;
    bool vehicle_speed_valid;
    float vehicle_speed;
    bool turn_urgency_valid;
    float turn_urgency;
    bool boosting;
    bool ignore_obstacles;
    bool failsafe_pathfinding;
    bool enable_targeting;
    bool enable_looking;
    bool enable_moving;
    bool enable_dialogue;
    bool suppress_activity_termination;
    bool suppress_global_dialogue;
    bool style_index_valid;
    short forced_combat_status;
    long style_index;
    long action_animation_impulse;
    long animation_mode;
    bool grenade_throw_depress_trigger;
    bool grenade_throw_started;
    short grenade_throw_trajectory_type;
    c_ai_point3d grenade_target;
    bool approach_pending;
    bool approach_within_range;
    long approach_object_index;
    float approach_distance_sq;
    float approach_max_distance_sq;
    float approach_follow_distance_sq;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char command_script_size[(sizeof(command_script_datum) == 0x188) ? 1 : -1];
typedef char command_script_commands_offset[(offsetof(command_script_datum, command) == 0x24) ? 1 : -1];
typedef char command_script_controls_offset[(offsetof(command_script_datum, control) == 0xa8) ? 1 : -1];
typedef char command_script_style_flag_offset[(offsetof(command_script_datum, style_index_valid) == 0x151) ? 1 : -1];
typedef char command_script_style_offset[(offsetof(command_script_datum, style_index) == 0x154) ? 1 : -1];
typedef char command_script_approach_offset[(offsetof(command_script_datum, approach_object_index) == 0x178) ? 1 : -1];
#endif

#endif // AI_COMMAND_SCRIPT_TYPES_H
