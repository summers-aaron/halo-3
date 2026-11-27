#ifndef GAME_PLAYER_INTERACTION_H
#define GAME_PLAYER_INTERACTION_H

#include <stddef.h>

struct s_player_interaction
{
    unsigned short type;
    union
    {
        short seat_index;
        unsigned char weapon_flags;
    };
    long object_index;
};

struct s_player_action_context
{
    s_player_interaction interaction;
    long melee_target_unit;
};

typedef char player_interaction_size[(sizeof(s_player_interaction) == 8) ? 1 : -1];
typedef char player_interaction_object_offset[(offsetof(s_player_interaction, object_index) == 4) ? 1 : -1];
typedef char player_action_context_size[(sizeof(s_player_action_context) == 12) ? 1 : -1];
typedef char player_action_context_melee_offset[(offsetof(s_player_action_context, melee_target_unit) == 8) ? 1 : -1];

#endif // GAME_PLAYER_INTERACTION_H
