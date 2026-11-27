#include "networking/session/network_session_parameters_generic_set.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_start_game.h"
// networkinglibxbox/life_cycle_handler_matchmaking_select_host.obj: from the records 0x3e76f0 0x3e7838 0x3e79b0 0x3e7a60
// @flags /TP /O2 /Oc
// @address 0x3e7838 ?update_for_state_transition@c_life_cycle_state_handler_matchmaking_select_host@@UAA?AW4e_life_cycle_state_transition_type@@XZ
// @address 0x3e7a60 ?exit@c_life_cycle_state_handler_matchmaking_select_host@@UAAXPAVc_life_cycle_state_handler@@@Z
#include "cseries/cseries.h"
#include <string.h>
#include "networking/logic/network_session_interface.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_join_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_select_host.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session.h"
#include "cseries/crt_xdk.h"
#include "networking/network_time_types.h"

e_life_cycle_state_transition_type c_life_cycle_state_handler_matchmaking_select_host::update_for_state_transition()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group = manager->get_group_session();
    c_network_session *squad = manager->get_active_squad_session();
    e_life_cycle_state_transition_type result = _life_cycle_state_transition_none;
    switch (group->get_session_parameters()->m_parameters.session_mode.get()) {
    case _network_session_mode_setup:
        result = _life_cycle_state_transition_continue;
        break;
    case _network_session_mode_matchmaking_disbanding:
        if (squad->is_host()) {
            squad->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_start);
        }
        result = _life_cycle_state_transition_continue;
        break;
    case _network_session_mode_in_game:
    case _network_session_mode_matchmaking_choosing_game:
        manager->request_state_change(_life_cycle_state_matchmaking_prepare_map, 0, 0);
        result = _life_cycle_state_transition_change;
        break;
    }
    if (result == 1) {
        result = _life_cycle_state_transition_none;
        switch (squad->get_session_parameters()->m_parameters.session_mode.get()) {
        case _network_session_mode_matchmaking_slave:
            result = _life_cycle_state_transition_continue;
            break;
        case _network_session_mode_matchmaking_start:
            manager->request_state_change(_life_cycle_state_matchmaking_start, 12, k_matchmaking_start_entry_data);
            result = _life_cycle_state_transition_change;
            break;
        }
    }
    return result;
}

// @address 0x8c447c g_network_time_globals
static __forceinline int select_host_system_milliseconds()
{
    return g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
}

void c_life_cycle_state_handler_matchmaking_select_host::exit(c_life_cycle_state_handler *next)
{
    c_life_cycle_state_manager *owner = get_manager();
    c_network_session *context = owner->get_active_squad_session();
    if (context->is_host()) {
        unsigned long start_time = this->start_time;
        c_matchmaking_quality *state = owner->get_matchmaking_quality();
        state->set_host_selection_time((long)(select_host_system_milliseconds() - start_time) / 1000);
        if (next->get_state() != _life_cycle_state_matchmaking_prepare_map) {
            c_network_session_parameters *parameters = context->get_session_parameters();
            if (parameters->m_parameters.matchmaking_abort_requested.get_allowed() && *parameters->m_parameters.matchmaking_abort_requested.get())
                state->set_aborted();
            state->set_left_host_selection();
            state->submit_to_webstats();
        }
    }
}


inline void c_life_cycle_state_handler_matchmaking_select_host::update_progress()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group = manager->get_group_session();
    c_network_session_parameters *parameters = group->get_session_parameters();
    s_replicated_life_cycle_matchmaking_progress progress;
    memset(&progress, 0, sizeof(progress));
    progress.type = 3;
    parameters->m_parameters.matchmaking_progress.set(&progress);
}

// @address 0x3e76f0 ?update@c_life_cycle_state_handler_matchmaking_select_host@@UAAXXZ
void c_life_cycle_state_handler_matchmaking_select_host::update()
{
    c_life_cycle_state_manager *state_manager = get_manager();
    c_network_session *group = state_manager->get_group_session();
    if (group->is_host()) {
        update_progress();
        if (!(flags & 1) && c_life_cycle_state_handler_start_game::check_host(group, true, &require_random_delegation, &attempted_delegation)) {
            if (group->established() && group->is_host() &&
                group->get_session_membership()->leader_peer_index() != group->get_session_membership()->local_peer_index())
            {
                group->get_session_membership_for_update()->set_leader_peer_index(group->get_session_membership()->local_peer_index());
            }
            flags |= 1;
        }
        if ((flags & 1) && group->get_session_membership()->all_peers_established())
            group->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_choosing_game);
    }
}

// @address 0x3e79b0 ?enter@c_life_cycle_state_handler_matchmaking_select_host@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_matchmaking_select_host::enter(c_life_cycle_state_handler *previous, long entry_data_size, void *entry_data)
{
    c_network_session *group = get_manager()->get_group_session();
    c_life_cycle_state_handler::enter(previous, 0, 0);
    this->require_random_delegation = group->is_host();
    unsigned long time;
    if (g_network_time_globals.enabled)
        time = g_network_time_globals.time;
    else
        time = GetTickCount();
    this->start_time = time;
    this->attempted_delegation = false;
    this->flags = 0;
    network_group_session_set_disable_host_election(3);
    if (g_network_join_data.join_queue_mode != _network_join_queue_closed_while_in_matchmaking)
        network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_queue_closed_while_in_matchmaking;
}
