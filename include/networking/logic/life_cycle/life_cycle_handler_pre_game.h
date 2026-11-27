#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_PRE_GAME_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_PRE_GAME_H

// Functions defined in tu/networkinglibxbox/life_cycle_handler_pre_game.cpp that other units call.

#include "networking/session/network_session_parameters.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"

#if !defined(_XBOX) || defined(__clang__)
extern "C" int port_coop_4p_enabled(void);
#define LIFE_CYCLE_PRE_GAME_MAXIMUM_CAMPAIGN_LOCAL_PLAYERS (port_coop_4p_enabled() ? 4 : 2)
#else
#define LIFE_CYCLE_PRE_GAME_MAXIMUM_CAMPAIGN_LOCAL_PLAYERS 2
#endif

class c_life_cycle_state_handler_pre_game : public c_life_cycle_state_handler {
public:
    virtual const char *get_state_string();
    virtual void enter(c_life_cycle_state_handler *handler, long entry_data_size, void *entry_data);
    virtual void exit(c_life_cycle_state_handler *handler);
    virtual void update();
    virtual e_life_cycle_state_transition_type update_for_state_transition();
private:
    unsigned long m_last_countdown_update;
    long m_last_countdown;
    long m_entry_state; // Cleared on enter.
    void reset_map_request();
    void leader_update_countdown();
    void host_update_countdown();
    void matchmaking_host_update();
    void squad_game_start_status_update();
};

typedef char life_cycle_pre_game_handler_size[(sizeof(void *) != 4 || ((sizeof(c_life_cycle_state_handler_pre_game) == 0x34))) ? 1 : -1];

#endif // NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_HANDLER_PRE_GAME_H
