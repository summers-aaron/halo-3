#ifndef NETWORKING_LOGIC_LOGIC_MATCHMAKING_SEEKER_SEARCH_TYPES_H
#define NETWORKING_LOGIC_LOGIC_MATCHMAKING_SEEKER_SEARCH_TYPES_H

enum e_matchmaking_search_preference { _matchmaking_search_preference_none };

struct s_matchmaking_seeker_search_start_parameters
{
    e_matchmaking_search_preference search_preference;
    bool flag_2_enabled;
    bool force_desperation;
    long initial_desperation_sessions_found_count;
    long initial_desperation_session_search_count;
};

typedef char matchmaking_seeker_search_start_parameters_size[(sizeof(s_matchmaking_seeker_search_start_parameters) == 0x10) ? 1 : -1];

#endif // NETWORKING_LOGIC_LOGIC_MATCHMAKING_SEEKER_SEARCH_TYPES_H
