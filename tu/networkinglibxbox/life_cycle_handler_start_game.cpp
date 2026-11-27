// networkinglibxbox/life_cycle_handler_start_game.obj: from the records 0x3b5d90 0x3e0c90 0x3e0d80 0x3e0e28 0x3e0f18 0x3e1048 0x3e1140
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/game_types.h"
#include "networking/session/network_game_countdown_delayed_reason_types.h"
// @address 0x3b5d90 ?set@c_peer_request_parameter@@QAA_NPBUs_peer_request@@@Z
// @address 0x3e0d80 ?update_for_state_transition@c_life_cycle_state_handler_start_game@@UAA?AW4e_life_cycle_state_transition_type@@XZ
// @address 0x3e1048 ?check_host@c_life_cycle_state_handler_start_game@@CA_NPAVc_network_session@@_NPA_N2@Z
#include <string.h>
#include "networking/transport/transport_security.h"
#include "networking/logic/life_cycle/life_cycle_handler_start_game.h"
#include "networking/logic/network_session_interface.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_join_queue_types.h"
#include "networking/session/network_session_types.h"
#include "networking/logic/life_cycle/peer_request_types.h"
#include "scenario/scenario.h"
#include "game/game_globals.h"
#include "scenario/scenario_tag.h"
#include "game/player_identifier_types.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
// @address 0x3e1140 ?set@?$c_generic_network_session_parameter@_K@@QAA_NPB_K@Z
// @address 0x3e0f18 ?set_unique_game_parameters@c_life_cycle_state_handler_start_game@@AAAXXZ
// @address 0x3e0e28 ?enter@c_life_cycle_state_handler_start_game@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
// @address 0x3a1600 ?is_host@c_network_session@@QBA_NXZ
// @address 0x3a1298 ?transport_secure_random@@YAXJPAE@Z
// @address 0x3a2ec8 ?set@?$c_generic_network_session_parameter@_N@@QAA_NPB_N@Z
// @address 0x3b5bf0 ?set@?$c_generic_network_session_parameter@W4e_campaign_difficulty_level@@@@QAA_NPBW4e_campaign_difficulty_level@@@Z
// @address 0x1f6c58 ?generate_random_seed@@YAKXZ
// @address 0x3e0c90 ?update@c_life_cycle_state_handler_start_game@@UAAXXZ
#include "networking/session/network_session_parameters_base.h"
#include "networking/logic/life_cycle/life_cycle_handler_start_game_internals.h"
#include "networking/session/network_session_parameters_generic.h"
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"
#include "networking/session/network_session_parameter_session_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/network_bandwidth_globals.h"
#include "networking/session/network_session_membership_api.h"
#include "math/random_math.h"
#include "networking/session/network_session_parameters_generic_set.h"

bool c_peer_request_parameter::set(const s_peer_request *value)
{
    bool result = false;
    if (set_allowed())
    {
        // match: 0x3b5bf0: pointer __restrict qualifier prevents alias reload
        s_peer_request *__restrict current = &m_current;
        if (memcmp(current, value, sizeof(s_peer_request)) != 0 || !get_allowed())
        {
            *current = *value;
            set_update_required();
        }
        result = true;
    }
    return result;
}
e_life_cycle_state_transition_type c_life_cycle_state_handler_start_game::update_for_state_transition()
{
    e_life_cycle_state_transition_type transition = _life_cycle_state_transition_none;
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *squad = manager->m_active_squad_session;
    e_network_session_mode mode = squad->get_session_parameters()->m_parameters.session_mode.get();
    switch (mode)
    {
    case 2:
        transition = _life_cycle_state_transition_continue;
        break;
    case 3:
        manager->request_state_change(_life_cycle_state_in_game, 0, 0);
        transition = _life_cycle_state_transition_change;
        break;
    }
    return transition;
}
static __forceinline bool life_cycle_handler_start_game_delegating_host(const c_network_session *session)
{
    bool result = false;
    if (session->m_local_state == 8 && !session->m_local_state_data.host_handoff.host_leaving)
        result = true;
    return result;
}
static __forceinline bool life_cycle_handler_start_game_host_established(const c_network_session *session)
{
    return session->m_local_state == 6;
}
bool c_life_cycle_state_handler_start_game::check_host(
    c_network_session *session, bool allow_delegation,
    bool *require_random_delegation, bool *attempted_delegation)
{
    bool result = false;

    if ((session)->is_host())
    {
        bool can_delegate = allow_delegation;
        if (!network_bandwidth_globals.online_network_environment)
            can_delegate = false;

        if (!can_delegate)
        {
            result = true;
        }
        else
        {
            if (life_cycle_handler_start_game_delegating_host(session))
            {
                result = false;
            }
            else if (!life_cycle_handler_start_game_host_established(session))
            {
                result = false;
            }
            else if (!*attempted_delegation)
            {
                bool previous_require_random_delegation = *require_random_delegation;
                *require_random_delegation = false;
                *attempted_delegation = true;
                if ((session)->host_delegate_host(-1, true, previous_require_random_delegation))
                {
                    result = false;
                }
                else
                {
                    result = true;
                }
            }
            else
            {
                result = true;
            }
        }
    }
    else
    {
        *attempted_delegation = false;
        result = false;
    }

    return result;
}
template <>
bool c_generic_network_session_parameter<unsigned __int64>::set(const unsigned __int64 *parameter)
{
    bool result = false;
    if (set_allowed())
    {
        if (memcmp(&m_data, parameter, sizeof(m_data)) != 0 || !get_allowed())
        {
            m_data = *parameter;
            set_update_required();
        }
        result = true;
    }
    return result;
}
void c_life_cycle_state_handler_start_game::update()
{
    c_network_session *session = m_manager->m_active_squad_session;
    bool ready = true;
    ready = ready && session->is_host();
    ready = ready && setup_initial_participants(session);
    ready = ready && c_life_cycle_state_handler_start_game::check_host(session, m_allow_delegation, &m_require_random_delegation, &m_attempted_delegation);
    if (ready)
    {
        c_network_session_parameters::s_network_session_parameters_internal *parameters = &session->get_session_parameters()->m_parameters;
        set_unique_game_parameters();
        parameters->countdown_timer.set(false, 0, _network_game_countdown_delayed_reason_none, (s_player_identifier const *)0);
        parameters->session_mode.set(_network_session_mode_in_game);
    }
}
static __forceinline int life_cycle_handler_start_game_out_of_range(long value)
{
    if (value < 0)
        return 1;
    if (value >= 12)
        return 1;
    return 0;
}
static __forceinline c_network_session_parameters::s_network_session_parameters_internal *life_cycle_handler_start_game_parameters(c_network_session *session)
{
    return &session->get_session_parameters()->m_parameters;
}
static __forceinline const c_network_session_membership *life_cycle_handler_start_game_membership(const c_network_session *session)
{
    return &session->m_session_membership;
}
static __forceinline void life_cycle_handler_start_game_set_join_queue_mode(e_network_join_queue_mode join_queue_mode)
{
    if (g_network_join_data.join_queue_mode != join_queue_mode)
        network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = join_queue_mode;
}
void c_life_cycle_state_handler_start_game::enter(c_life_cycle_state_handler *previous, long entry_data_size, void *entry_data)
{
    c_network_session *session = m_manager->m_active_squad_session;
    c_life_cycle_state_handler::enter(previous, 0, 0);
    c_network_session_parameters::s_network_session_parameters_internal *parameters = life_cycle_handler_start_game_parameters(session);
    m_attempted_delegation = false;
    m_allow_delegation = true;
    m_require_random_delegation = false;
    session->m_close_session_for_matchmaking = false;
    if (parameters->start_mode.get_allowed() && *parameters->start_mode.get() == 3)
        m_allow_delegation = false;
    if (session->is_host() && parameters->session_mode.get() == 2)
        session->get_session_parameters()->clear_parameters_with_mask(0x100ui64);
    network_group_session_set_disable_host_election(0);
    life_cycle_handler_start_game_set_join_queue_mode(_network_join_open_to_join_squad);
}
void c_life_cycle_state_handler_start_game::set_unique_game_parameters()
{
    c_network_session *session = m_manager->m_active_squad_session;
    if (session->is_host())
    {
        c_network_session_parameters::s_network_session_parameters_internal *parameters = life_cycle_handler_start_game_parameters(session);
        bool theater = false;
        const e_gui_game_mode *game_mode = parameters->ui_game_mode.get();
        if (game_mode != 0 && *game_mode == 4)
            theater = true;
        if (!theater)
        {
            union
            {
                unsigned __int64 game_instance;
                unsigned long random_seed;
                long language;
                long determinism_version;
            } parameter_value;
            parameter_value.game_instance = 0;
            transport_secure_random(8, (unsigned char *)&parameter_value.game_instance);
            parameters->game_instance.set(&parameter_value.game_instance);
            parameter_value.random_seed = generate_random_seed();
            // match: 0x3e0f18: the unsigned-long random-seed setter shares this campaign-difficulty COMDAT.
            ((c_generic_network_session_parameter<e_campaign_difficulty_level> *)&parameters->random_seed)
                ->set((const e_campaign_difficulty_level *)&parameter_value.random_seed);
            long language = 0;
            if (global_scenario != 0)
            {
                long cache_language = global_game_globals->language;
                if (life_cycle_handler_start_game_out_of_range(cache_language) == 0)
                    language = cache_language;
            }
            parameter_value.language = language;
            parameters->language.set(&parameter_value.language);
            parameter_value.determinism_version = network_session_membership_get_common_determinism_version(life_cycle_handler_start_game_membership(session));
            parameters->determinism_version.set(&parameter_value.determinism_version);
        }
        bool search = false;
        parameters->end_game.set(&search);
        s_peer_request none;
        memset(&none, 0, sizeof(none));
        ((c_peer_request_parameter *)&parameters->request_campaign_quit)->set(&none);
    }
}
