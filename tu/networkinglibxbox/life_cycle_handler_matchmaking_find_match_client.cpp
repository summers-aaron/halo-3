// networkinglibxbox/life_cycle_handler_matchmaking_find_match_client.obj: from the records 0x3e4900 0x3e4a18
// @flags /TP /O2 /Oc
// @address 0x3e4900 ?update_for_state_transition@c_life_cycle_state_handler_matchmaking_find_match_client@@UAA?AW4e_life_cycle_state_transition_type@@XZ
// @address 0x3e4a18 ?enter@c_life_cycle_state_handler_matchmaking_find_match_client@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
#include "cseries/cseries_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include <string.h>
#include "networking/logic/network_session_interface.h"
#include "networking/logic/network_join.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"

#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_find_match_client.h"
#include "networking/logic/network_join_queue_types.h"

extern "C" {
// @address 0x266c0 k_matchmaking_start_entry_data
// @rdata 0x820266c0..0x820266cc
const char k_matchmaking_start_entry_data[12] = { 0 };
// @rdata-end
}

e_life_cycle_state_transition_type c_life_cycle_state_handler_matchmaking_find_match_client::update_for_state_transition()
{
    e_life_cycle_state_transition_type transition = _life_cycle_state_transition_none;
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squadSession = manager->get_active_squad_session();
    if (squadSession->is_host()) {
        squadSession->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_start);
    }
    switch (squadSession->get_session_parameters()->m_parameters.session_mode.get()) {
    case _network_session_mode_matchmaking_searching:
        transition = _life_cycle_state_transition_continue;
        break;
    case _network_session_mode_matchmaking_start:
        manager->request_state_change(_life_cycle_state_matchmaking_start, 12, k_matchmaking_start_entry_data);
        transition = _life_cycle_state_transition_change;
        break;
    case _network_session_mode_matchmaking_slave:
        manager->request_state_change(_life_cycle_state_matchmaking_assemble_match, 0, 0);
        transition = _life_cycle_state_transition_change;
        break;
    }
    return transition;
}
void c_life_cycle_state_handler_matchmaking_find_match_client::enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data)
{
    c_life_cycle_state_handler::enter(from_handler, 0, 0);
    network_group_session_set_disable_host_election(3);
    if (g_network_join_data.join_queue_mode != _network_join_queue_closed_while_in_matchmaking) network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_queue_closed_while_in_matchmaking;
}

// @address 0x5bec98 ?update@c_life_cycle_state_handler_matchmaking_find_match_client@@UAAXXZ
void c_life_cycle_state_handler_matchmaking_find_match_client::update()
{
}
