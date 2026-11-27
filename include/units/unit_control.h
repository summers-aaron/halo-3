#ifndef UNITS_UNIT_CONTROL_H
#define UNITS_UNIT_CONTROL_H

#include "math/real_math_types.h"

struct s_unit_weapon_set {
    union {
        short set_identifier;
        short identifier;
    };
    union {
        signed char weapon_indices[2];
        unsigned char indices[2];
    };
};

struct s_aim_assist_targeting_result {
    union {
        long target_player_index;
        long target_index;
    };
    union {
        long target_object_index;
        long object_index;
    };
    union {
        long model_target;
        long model_target_index;
        long model_index;
        long entry_index;
    };
    union {
        float primary_autoaim_level;
        float lock_amount;
    };
    float secondary_autoaim_level;
    real_vector3d lead_vector;
    unsigned long flags;
};

struct unit_control_data {
    union {
        long animation_state;
        long animation_seat;
    };
    union {
        short aiming_speed;
        short animation_speed;
    };
    union {
        s_unit_weapon_set weapon_set;
        struct {
            short weapon_set_id;
            unsigned char weapon_set_indices[2];
        };
    };
    short grenade_index;
    short zoom_level;
    unsigned long control_flags;
    real_vector3d throttle;
    float primary_trigger;
    float secondary_trigger;
    union {
        real_vector3d facing_vector;
        real_vector3d facing;
    };
    union {
        real_vector3d aiming_vector;
        real_vector3d aiming;
    };
    union {
        real_vector3d looking_vector;
        real_vector3d looking;
    };
    union {
        real_point3d gaze_position;
        real_point3d position;
    };
    union {
        s_aim_assist_targeting_result targeting;
        s_aim_assist_targeting_result aim_assist;
        unsigned char aim_assist_data[36];
    };
};

typedef char unit_ctrl_size_weapon_set[(sizeof(s_unit_weapon_set) == 4) ? 1 : -1];
typedef char unit_ctrl_size_aim_assist[(sizeof(s_aim_assist_targeting_result) == 0x24) ? 1 : -1];
typedef char unit_ctrl_size_control_data[(sizeof(unit_control_data) == 0x7c) ? 1 : -1];

#endif // UNITS_UNIT_CONTROL_H
