#ifndef NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_MATCHMAKING_START_H
#define NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_MATCHMAKING_START_H

#include "networking/logic/life_cycle/life_cycle_handler_types.h"

enum e_life_cycle_matchmaking_find_match_role;
struct s_matchmaking_start_event;

struct s_matchmaking_start_data { long initial_session_search_count; long initial_desperation_sessions_found_count; long unused; };

class c_life_cycle_state_handler_matchmaking_start : public c_life_cycle_state_handler
{
public:
    c_life_cycle_state_handler_matchmaking_start() : m_flags(0) {}
    unsigned char m_flags;
    long m_search_count;
    union { long m_start_mode; s_matchmaking_start_data m_entry_data; };
    long m_desperation_search_count;
    long m_last_nat_warning_time;
public:
    virtual const char *get_state_string();
    virtual void enter(c_life_cycle_state_handler *from, long size, void *data);
    virtual void update();
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    void get_progress(s_life_cycle_matchmaking_progress *progress_out);
    void handle_missing_required_session_parameter(e_network_session_type session_type);
private:
    void handle_session_event(const s_matchmaking_start_event *event);
    bool squad_allowed_to_start_matchmaking(bool log);
    __forceinline void update_squad_ready();
    e_life_cycle_matchmaking_find_match_role determine_matchmaking_find_match_role(bool force_gather);
    void create_group_session();
    void finish_group_session_creation();
    void update_group_creation();
};

typedef char life_cycle_matchmaking_start_handler_size[(sizeof(void *) != 4 || ((sizeof(c_life_cycle_state_handler_matchmaking_start) == 0x44))) ? 1 : -1];
#endif
