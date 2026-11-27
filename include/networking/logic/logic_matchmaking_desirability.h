#ifndef NETWORKING_LOGIC_LOGIC_MATCHMAKING_DESIRABILITY_H
#define NETWORKING_LOGIC_LOGIC_MATCHMAKING_DESIRABILITY_H

// Functions defined in tu/networkinglibxbox/logic_matchmaking_desirability.cpp.

#include "networking/logic/matchmaking_desirability_result_types.h"
#include <stddef.h>
#include "cseries/cseries_wrapped.h"

struct s_matchmaking_session_player_properties
{
    long peer_index;
    long party_index;
};

struct s_matchmaking_session_properties
{
    unsigned short hopper_identifier;
    bool ranked;
    long player_count;
    unsigned long player_valid_flags[1];
    s_matchmaking_session_player_properties players[16];
    long average_skill_level;
    long minimum_skill_level;
    long maximum_skill_level;
    float average_mu;
    long average_experience;
    union { unsigned long preferred_party_size_mask; c_flags_no_init<long, unsigned long, 32> preferred_party_sizes; };
    union { unsigned long acceptable_party_size_mask; c_flags_no_init<long, unsigned long, 32> acceptable_party_sizes; };
    union { long good_host_count; long guest_count; };
    union { long maximum_peer_nat_type; long nat_type; };
    long language;
    long country;
    long region;
    long worst_nat_type;
    union { unsigned char composition_flags; c_flags_no_init<long, unsigned char, 8> flags; };
};

struct s_matchmaking_search_party_preferences
{
    long type;
    long minimum;
    long maximum;
};

struct s_matchmaking_gather_party_properties
{
    long gather_seconds;
    s_matchmaking_search_party_preferences preferences;
    s_matchmaking_session_properties session_properties;
};

struct s_matchmaking_search_party_properties
{
    long seconds;
    long latency;
    s_matchmaking_search_party_preferences preferences;
    s_matchmaking_session_properties session_properties;
};

typedef char matchmaking_good_host_count_offset[(offsetof(s_matchmaking_session_properties, good_host_count) == 0xa8) ? 1 : -1];
typedef char matchmaking_maximum_peer_nat_type_offset[(offsetof(s_matchmaking_session_properties, maximum_peer_nat_type) == 0xac) ? 1 : -1];

typedef char matchmaking_session_properties_size[(sizeof(s_matchmaking_session_properties) == 196) ? 1 : -1];
typedef char matchmaking_session_properties_player_count[(offsetof(s_matchmaking_session_properties, player_count) == 4) ? 1 : -1];
typedef char matchmaking_session_properties_average_mu[(offsetof(s_matchmaking_session_properties, average_mu) == 0x98) ? 1 : -1];
typedef char matchmaking_session_properties_flags[(offsetof(s_matchmaking_session_properties, composition_flags) == 0xc0) ? 1 : -1];
typedef char matchmaking_gather_party_properties_size[(sizeof(s_matchmaking_gather_party_properties) == 212) ? 1 : -1];
typedef char matchmaking_search_party_properties_size[(sizeof(s_matchmaking_search_party_properties) == 216) ? 1 : -1];

class c_bitstream;
struct s_matchmaking_gather_party_properties;
struct s_matchmaking_search_party_properties;
struct s_matchmaking_session_properties;

void network_matchmaking_build_allowed_skill_range(const s_matchmaking_session_properties *session_properties, long skill_range,
    long *minimum_skill_level_out, long *maximum_skill_level_out);
void network_matchmaking_session_properties_encode(c_bitstream *packet, const s_matchmaking_session_properties *properties);
bool network_matchmaking_gather_party_properties_decode(c_bitstream *packet, s_matchmaking_gather_party_properties *properties);
bool network_matchmaking_search_party_properties_decode(c_bitstream *packet, s_matchmaking_search_party_properties *properties);
extern "C" void network_matchmaking_build_gather_party_properties(const s_matchmaking_session_properties *composition, long seconds,
    long preference, s_matchmaking_gather_party_properties *properties);
extern "C" void network_matchmaking_build_search_party_properties(const s_matchmaking_session_properties *composition, long seconds,
    long preference, s_matchmaking_search_party_properties *properties);
extern "C" bool network_matchmaking_gather_party_properties_valid(const s_matchmaking_gather_party_properties *properties);

struct s_matchmaking_search_party_preferences;

bool network_matchmaking_session_properties_valid(const s_matchmaking_session_properties *session_properties);
bool network_matchmaking_session_preferences_valid(const s_matchmaking_search_party_preferences *preferences);
extern "C" bool network_matchmaking_search_party_preferences_decode(c_bitstream *packet, s_matchmaking_search_party_preferences *preferences);
e_matchmaking_desirability_result network_matchmaking_compute_session_desirability( const s_matchmaking_gather_party_properties *gather,
    const s_matchmaking_search_party_properties *search, long *desirability_out);

extern const long logic_matchmaking_desirability_skill_level_bounds[2];

#endif // NETWORKING_LOGIC_LOGIC_MATCHMAKING_DESIRABILITY_H
