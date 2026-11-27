#ifndef MOTOR_BIPED_GROUND_PROGRAM_STATE_H
#define MOTOR_BIPED_GROUND_PROGRAM_STATE_H

#include <stddef.h>
#include "units/motor_program_state.h"
#include "math/real_math_types.h"

struct biped_ground_program_state
{
    c_motor_program_state motor;

    short flags;
    unsigned char state_index;
    unsigned char control_state_index;
    unsigned char idle_ticks;

    short facing_direction;
    real_vector3d movement_vector;
    bool snap_facing;
    signed char airborne_ticks;
    signed char ticks_since_jump;

    short program;
    signed char landing_ticks;
    signed char landing_duration_ticks;
    char pad36;
    signed char slipping_ticks;
    unsigned char stopped_moving_ticks;

    real_vector3d delta;
    float turn_amount;
    float turn_velocity;
};
typedef char ground_state_flags_offset[(offsetof(biped_ground_program_state, flags) == 8) ? 1 : -1];
typedef char ground_state_delta_offset[(offsetof(biped_ground_program_state, delta) == 40) ? 1 : -1];
typedef char ground_state_size[(sizeof(biped_ground_program_state) == 60) ? 1 : -1];

#endif
