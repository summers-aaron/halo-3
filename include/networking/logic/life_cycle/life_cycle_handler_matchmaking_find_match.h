#ifndef NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_MATCHMAKING_FIND_MATCH_H
#define NETWORKING_LOGIC_LIFE_CYCLE_HANDLER_MATCHMAKING_FIND_MATCH_H
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/logic_matchmaking_seeker.h"
#include "networking/suitable_matchmaking_session_types.h"
enum e_matchmaking_find_match_flags
{
    _matchmaking_find_match_search_started_bit,
    _matchmaking_find_match_join_pending_bit,
    _matchmaking_find_match_search_complete_bit,
    k_matchmaking_find_match_flag_count
};

class c_life_cycle_state_handler_matchmaking_find_match : public c_life_cycle_state_handler {
public:
    virtual const char *get_state_string();
    __forceinline void get_progress(s_life_cycle_matchmaking_progress *progress) { get_matchmaking_progress_for_squad_search(progress); }
    bool get_search_party_properties(struct s_matchmaking_search_party_properties *properties);
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual void update();
    virtual void enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data);
    void exit(c_life_cycle_state_handler *life_cycle_state_handler);

private:
    void update_joining();
    void update_leaving();

    c_flags<e_matchmaking_find_match_flags, unsigned char, k_matchmaking_find_match_flag_count> m_update_flags;
    // Untouched by every March accessor; preserved for layout (m_seeker follows at 0x30).
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    c_matchmaking_seeker m_seeker;
    s_suitable_matchmaking_session m_session;
    long m_value;
};
#endif
