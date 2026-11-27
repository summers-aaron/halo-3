#ifndef NETWORKING_LOGIC_LOGIC_MATCHMAKING_COMPOSITION_DATA_TYPES_H
#define NETWORKING_LOGIC_LOGIC_MATCHMAKING_COMPOSITION_DATA_TYPES_H

#include "networking/logic/logic_matchmaking_desirability.h"

// The composition builder (0x43ec80) fills these records for the life-cycle handlers.
struct s_network_session_player_matchmaking_composition
{
    long hopper_skill;
    long experience_rank;
    float mu;
    long adjusted_skill_level;
};

struct s_network_session_party_matchmaking_composition
{
    long player_count;
    long guest_player_count;
    long average_adjusted_skill_level;
    long maximum_skill_level;
    long minimum_adjusted_skill_level;
    float average_mu;
    long average_experience_rank;
    unsigned char flags;
};

struct s_network_session_matchmaking_composition
{
    s_matchmaking_session_properties session_properties;
    long guest_player_count;
    s_network_session_player_matchmaking_composition player_compositions[16];
    long team_imbalance;
    long party_count;
    long big_squad_count;
    long big_squad_size_imbalance;
    s_network_session_party_matchmaking_composition parties[16];
    long host_bandwidth;
    long host_capacity;
};

typedef char network_session_player_matchmaking_composition_size[(sizeof(s_network_session_player_matchmaking_composition) == 0x10) ? 1 : -1];
typedef char network_session_party_matchmaking_composition_size[(sizeof(s_network_session_party_matchmaking_composition) == 0x20) ? 1 : -1];
typedef char network_session_matchmaking_composition_size[(sizeof(s_network_session_matchmaking_composition) == 0x3e0) ? 1 : -1];
typedef char network_session_matchmaking_composition_players_offset[(offsetof(s_network_session_matchmaking_composition,
    player_compositions) == 0xc8) ? 1 : -1];
typedef char network_session_matchmaking_composition_parties_offset[(offsetof(s_network_session_matchmaking_composition, parties) == 0x1d8) ? 1 : -1];

typedef char matchmaking_composition_guest_count_offset[(offsetof(s_network_session_matchmaking_composition, guest_player_count) == 0xc4) ? 1 : -1];
typedef char matchmaking_party_guest_count_offset[(offsetof(s_network_session_party_matchmaking_composition, guest_player_count) == 4) ? 1 : -1];

#endif // NETWORKING_LOGIC_LOGIC_MATCHMAKING_COMPOSITION_DATA_TYPES_H
