#ifndef NETWORKING_LOGIC_LIFE_CYCLE_FIND_MATCH_CLIENT_H
#define NETWORKING_LOGIC_LIFE_CYCLE_FIND_MATCH_CLIENT_H

#include "networking/logic/life_cycle/life_cycle_handler_types.h"

class c_life_cycle_state_handler_matchmaking_find_match_client : public c_life_cycle_state_handler {
public:
    c_life_cycle_state_handler_matchmaking_find_match_client() : m_flags(0) {}
    virtual const char *get_state_string();
    virtual void update();
    __forceinline void get_progress(s_life_cycle_matchmaking_progress *progress) { get_matchmaking_progress_for_squad_search(progress); }
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual void enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data);
private:
    // State flags, cleared on construction.
    unsigned char m_flags;
};

extern "C" const char k_matchmaking_start_entry_data[12];

#endif
