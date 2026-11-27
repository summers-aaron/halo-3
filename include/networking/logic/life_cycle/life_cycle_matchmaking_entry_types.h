#ifndef NETWORKING_LOGIC_LIFE_CYCLE_MATCHMAKING_ENTRY_TYPES_H
#define NETWORKING_LOGIC_LIFE_CYCLE_MATCHMAKING_ENTRY_TYPES_H

// Matchmaking start consumes only the event type; the leading bytes are unused.
struct s_matchmaking_start_event { unsigned char leading_bytes[8]; long type; };

// Entry data matchmaking start hands to the find-and-assemble handler.
struct s_find_and_assemble_data
{
    bool force_desperation;
    long initial_sessions_found_count;
    long initial_session_search_count;
};
#endif
