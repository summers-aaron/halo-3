#ifndef AI_AI_PLAYER_TYPES_H
#define AI_AI_PLAYER_TYPES_H

#include <stddef.h>

#include "math/real_math_types.h"

// Four campaign-player records owned by ai_player.cpp (TLS g_ai_players).
struct ai_player_state
{
    union { long id; long player_index; };
    long unit_index;
    short squad_index;
    short flags;
    unsigned short reset_a;
    unsigned short reset_b;
    long last_damage_time;
    long last_damage_object;
    signed char vehicle_counter;
    bool airborne_dialogue;
    bool flag;
    char pad1b;
    long last_change_time;
    // ai_player_update low-pass filters the unit's forward vector into this member.
    real_vector3d position;
};

typedef char ai_player_state_size[(sizeof(ai_player_state) == 0x2c) ? 1 : -1];
typedef char ai_player_state_timer_offset[(offsetof(ai_player_state, reset_a) == 0xc) ? 1 : -1];
typedef char ai_player_state_forward_offset[(offsetof(ai_player_state, position) == 0x20) ? 1 : -1];

typedef char ai_player_damage_time_offset[(offsetof(ai_player_state, last_damage_time) == 0x10) ? 1 : -1];
typedef char ai_player_damage_object_offset[(offsetof(ai_player_state, last_damage_object) == 0x14) ? 1 : -1];

#endif // AI_AI_PLAYER_TYPES_H
