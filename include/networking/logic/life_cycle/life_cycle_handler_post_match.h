#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_POST_MATCH_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_POST_MATCH_H

#include "networking/logic/life_cycle/life_cycle_handler_types.h"

// Functions defined in tu/networkinglibxbox/life_cycle_handler_post_match.cpp.

class c_network_session_membership;
class c_life_cycle_state_handler_post_match;

extern "C" long life_cycle_handler_post_match_find_peer_first_player(const c_network_session_membership *membership, long peer_index);
extern "C" bool life_cycle_handler_post_match_is_settled(c_life_cycle_state_handler_post_match *handler);

struct s_replicated_life_cycle_progress_post_match;

class c_life_cycle_state_handler_post_match : public c_life_cycle_state_handler {
    friend bool life_cycle_handler_post_match_is_settled(c_life_cycle_state_handler_post_match *handler);
public:
    c_life_cycle_state_handler_post_match() : m_flags(0) {}
    virtual const char *get_state_string();
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual void exit(c_life_cycle_state_handler *previous_handler);
    virtual void enter(c_life_cycle_state_handler *previous, long entry_data_size, void *entry_data);
    virtual void update();
    void get_progress(s_life_cycle_matchmaking_progress *progress);
    __forceinline unsigned short get_flags() const { return m_flags; }
private:
    void update_squad_host();
    void update_group_host();
    long get_peer_rematch_team_index(long peer_index);
    void update_leader_idle_controllers();
    __forceinline long get_player_rematch_team_index(long player_index);
    unsigned short m_flags;
    long m_2c;
    long m_30;
    long m_start_time;
    long m_settle_start_time;
    long m_3c;
    private:
    void get_state_from_progress(struct s_replicated_life_cycle_progress_post_match *progress);

};

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_POST_MATCH_H
