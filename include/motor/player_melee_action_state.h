#ifndef MOTOR_PLAYER_MELEE_ACTION_STATE_H
#define MOTOR_PLAYER_MELEE_ACTION_STATE_H

#include <stddef.h>

// The active melee strike, with its animation and timing;
// units.cpp reads the same strike and records its last damage time at +0x68.
struct s_player_melee_action_state
{
    // Action-block bytes ahead of the melee strike fields, shared with the
    // generic action views; the melee-specific state starts at +0x3c.
    char action_storage[0x3c];
    long animation;
    signed char weapon_slot;
    unsigned char intensity;
    signed char ticks;
    signed char primary_key_ticks;
    signed char secondary_key_ticks;
    signed char total_ticks;
    bool started;
    bool silent;
    // No observed accesses through the melee view; preserved for layout
    // (last_melee_time follows at +0x68).
    char spare_storage[0x20];
    long last_melee_time;
};

typedef char player_melee_action_size[(sizeof(s_player_melee_action_state) == 0x6c) ? 1 : -1];
typedef char player_melee_action_animation_offset[(offsetof(s_player_melee_action_state, animation) == 0x3c) ? 1 : -1];
typedef char player_melee_action_ticks_offset[(offsetof(s_player_melee_action_state, ticks) == 0x42) ? 1 : -1];
typedef char player_melee_action_time_offset[(offsetof(s_player_melee_action_state, last_melee_time) == 0x68) ? 1 : -1];

#endif
