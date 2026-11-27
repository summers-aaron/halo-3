#include "interface/user_interface_dialog_globals.h"
// networkinglibxbox/life_cycle_handler_leaving.obj: from the records 0x3e33d0 0x3e34c0 0x3e3560 0x3e3620 0x3e36c8 0x3e3768
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include <string.h>
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/network_session_interface.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_join_types.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "interface/gui_pregame_setup_manager.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"
#include "networking/session/network_session_class.h"
#include "networking/logic/network_life_cycle.h"

#include "networking/logic/life_cycle/life_cycle_handler_leaving.h"
#include "networking/logic/life_cycle/life_cycle_handler_joining.h"


// @address 0x3e36c8 ?update_for_state_transition@c_life_cycle_state_handler_joining@@UAA?AW4e_life_cycle_state_transition_type@@XZ
e_life_cycle_state_transition_type c_life_cycle_state_handler_joining::update_for_state_transition()
{
    e_life_cycle_state_transition_type transition = _life_cycle_state_transition_continue;
    if ((m_flags & 1) != 0)
    {
        c_life_cycle_state_manager *manager = m_manager;
        if (m_previous_state == _life_cycle_state_post_match)
        {
            manager->request_state_change(_life_cycle_state_post_match, 0, 0);
            transition = _life_cycle_state_transition_change;
        }
        else
        {
            manager->request_state_change(_life_cycle_state_pre_game, 0, 0);
            transition = _life_cycle_state_transition_change;
        }
    }
    return transition;
}
// @address 0x3e33d0 ?update@c_life_cycle_state_handler_leaving@@UAAXXZ
// The update slot of this class's vtable (0x44498 + 4; its get_state_string 0x3a1750 is the leaving handler's).
void c_life_cycle_state_handler_leaving::update()
{
    c_network_session *session = m_manager->m_active_squad_session;
    if (m_leaving_41) {
        if (session->disconnected()) m_flags |= 1;
        else if (!session->leaving_session()) {
            session->m_disconnect_reestablish_state = 1;
            session->initiate_leave_protocol(false);
        }
    } else {
        if (session->disconnected()) m_leaving_41 = true;
        else if (!session->leaving_session()) {
            if (session->is_host() && session->get_session_membership()->get_peer_count() <= 1) {
                session->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_idle);
                m_flags |= 1;
            } else session->initiate_leave_protocol(false);
        }
    }
}
extern "C" {
}
struct s_networking_join_data;
// @address 0x3e3560 ?enter@c_life_cycle_state_handler_leaving@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_leaving::enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data)
{
    c_life_cycle_state_handler_leaving *handler = this;
    bool *entry_flag = (bool *)entry_data;
    c_network_session *squad_session = handler->get_manager()->m_active_squad_session;
    ((c_life_cycle_state_handler *)handler)->c_life_cycle_state_handler::enter(from_handler, 0, 0);
    handler->m_leaving_41 = *entry_flag;
    handler->m_flags = 0;
    if (!handler->m_leaving_41) {
        long game_mode;
        if (!squad_session->disconnected() && squad_session->get_session_parameters()->m_parameters.ui_game_mode.get_allowed())
            game_mode = *squad_session->get_session_parameters()->m_parameters.ui_game_mode.get();
        else
            game_mode = 2;
        handler->m_game_mode = game_mode;
    }
    network_group_session_set_disable_host_election(0);
    if (g_network_join_data.join_queue_mode != _network_join_closed_to_all_joins) network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_closed_to_all_joins;
}
// @address 0x3e3768 ?enter@c_life_cycle_state_handler_joining@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_joining::enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data)
{
    c_life_cycle_state_handler_joining *handler = this;
    c_network_session *squad_session = handler->get_manager()->m_active_squad_session;
    ((c_life_cycle_state_handler *)handler)->c_life_cycle_state_handler::enter(from_handler, 0, 0);
    handler->m_flags = 0;
    handler->m_game_mode = 2;
    handler->m_previous_state = from_handler->get_state();
    if (g_network_join_data.join_queue_mode != _network_join_closed_to_all_joins) {
        network_join_flush_join_queue();
    }
    g_network_join_data.join_queue_mode = _network_join_closed_to_all_joins;
    if (!squad_session->disconnected() && !squad_session->leaving_session())
        squad_session->leave_session();
    if (!squad_session->disconnected() && squad_session->get_session_parameters()->m_parameters.ui_game_mode.get_allowed())
        handler->m_game_mode = *squad_session->get_session_parameters()->m_parameters.ui_game_mode.get();
    else
        handler->m_game_mode = 2;
    g_user_interface_networking_globals.start_state = 0;
}
// @address 0x3e34c0 ?update_for_state_transition@c_life_cycle_state_handler_leaving@@UAA?AW4e_life_cycle_state_transition_type@@XZ
e_life_cycle_state_transition_type c_life_cycle_state_handler_leaving::update_for_state_transition()
{
    e_life_cycle_state_transition_type transition = _life_cycle_state_transition_continue;
    if ((m_flags & 1) != 0)
    {
        c_life_cycle_state_manager *manager = get_manager();
        if (m_leaving_41 != 0)
            manager->request_state_change(_life_cycle_state_none, 0, 0);
        else
            manager->request_state_change(_life_cycle_state_pre_game, 0, 0);
        transition = _life_cycle_state_transition_change;
    }
    return transition;
}

// @address 0x3e3620 ?update@c_life_cycle_state_handler_joining@@UAAXXZ
void c_life_cycle_state_handler_joining::update()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *active_squad_session = manager->m_active_squad_session;
    if ((m_flags & 1) == 0)
    {
        bool complete = true;
        if (active_squad_session->leaving_session())
        {
            complete = false;
        }
        else if (active_squad_session->disconnected())
        {
            if (manager->m_target_squad_session->disconnected())
            {
                network_life_cycle_create_local_squad(active_squad_session->session_class());
                gui_pregame_setup_manager_initialize_squad_for_mode(m_game_mode);
            }
            else
            {
                manager->m_active_squad_session = manager->m_target_squad_session;
                manager->m_target_squad_session = active_squad_session;
            }
        }
        if (complete)
            m_flags |= 1;
    }
}
