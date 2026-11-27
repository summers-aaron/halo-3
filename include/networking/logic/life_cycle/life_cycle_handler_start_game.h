#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_START_GAME_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_START_GAME_H

#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"

class c_network_session;

class c_life_cycle_state_handler_start_game : public c_life_cycle_state_handler
{
public:
    virtual const char *get_state_string();
    virtual void update();
    virtual e_life_cycle_state_transition_type update_for_state_transition();
    virtual void enter(c_life_cycle_state_handler *previous, long entry_data_size, void *entry_data);
private:
    friend class c_life_cycle_state_handler_matchmaking_select_host;
    static bool check_host(c_network_session *session, bool allow_delegation, bool *require_random_delegation, bool *attempted_delegation);
    bool m_allow_delegation;
    bool m_attempted_delegation;
    bool m_require_random_delegation;
    void set_unique_game_parameters();
};

// Functions defined in tu/networkinglibxbox/life_cycle_handler_start_game.cpp that other units call.

class c_peer_request_parameter;
struct s_peer_request;


#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_START_GAME_H
