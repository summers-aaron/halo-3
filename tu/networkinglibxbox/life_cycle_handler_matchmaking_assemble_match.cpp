#include "networking/session/network_session_parameters_generic_set.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
// networkinglibxbox/life_cycle_handler_matchmaking_assemble_match.obj: from the records 0x3d33c8 0x3e6198 0x3e6408 0x3e6510 0x3e6a00 0x3e6b38 0x3e6c48 0x3e6d50 0x3e6d70 0x3e6f68 0x3e7150 0x3e7218 0x3e7358 0x3e7620
// @flags /TP /O2
#include "game/game_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/logic_matchmaking_composition_types.h"
// @address 0x3e7620 ?set@?$c_generic_network_session_parameter@Us_game_hopper_picked_game_collection@@@@QAA_NPBUs_game_hopper_picked_game_collection@@@Z
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "game/multiplayer_game_hopper.h"
#include "networking/logic/network_join.h"
#include "networking/transport/transport_security.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include "networking/transport/transport_security_types.h"
#include "networking/life_cycle_session_address_state_types.h"
#include "networking/life_cycle_qos_request_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_assemble_match.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"
#include "networking/transport/transport_platform_types.h"
typedef unsigned int uint;
#include <string.h>

#include "networking/session/network_managed_session.h"
struct utf8;


#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"
#include "networking/logic/network_join_queue_types.h"
#include "networking/network_time_since_access.h"
#include "networking/logic/logic_matchmaking_composition_data_types.h"
#include "networking/logic/logic_matchmaking_composition.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"

class c_life_cycle_state_handler_matchmaking_assemble_match;
extern "C" {
// @address 0x8c447c g_network_time_globals
}

#include "game/hopper_configuration.h"


// @address 0x609b50 XNetQosGetListenStats


// @address 0x3d33c8 life_cycle_initialize_qos_request
extern "C" bool life_cycle_initialize_qos_request(const void *description, s_life_cycle_qos_request *request)
{
    bool result = false;
    memset(request, 0, sizeof(*request));
    request->size = sizeof(*request);
    if (XNetQosGetListenStats(description, request) == 0)
        result = true;
    return result;
}

e_life_cycle_state_transition_type c_life_cycle_state_handler_matchmaking_assemble_match::update_for_state_transition()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *groupSession = manager->get_group_session();
    c_network_session *squadSession = manager->get_active_squad_session();
    e_life_cycle_state_transition_type transition = _life_cycle_state_transition_none;
    switch (groupSession->get_session_parameters()->m_parameters.session_mode.get()) {
    case _network_session_mode_matchmaking_gathering:
        transition = _life_cycle_state_transition_continue;
        break;
    case _network_session_mode_matchmaking_disbanding:
        if (squadSession->is_host()) {
            squadSession->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_start);
        }
        transition = _life_cycle_state_transition_continue;
        break;
    case _network_session_mode_matchmaking_arbitrating:
        manager->request_state_change(_life_cycle_state_matchmaking_arbitration, 0, 0);
        transition = _life_cycle_state_transition_change;
        break;
    }
    if (transition == _life_cycle_state_transition_continue) {
        transition = _life_cycle_state_transition_none;
        switch (squadSession->get_session_parameters()->m_parameters.session_mode.get()) {
        case _network_session_mode_matchmaking_slave:
            transition = _life_cycle_state_transition_continue;
            break;
        case _network_session_mode_matchmaking_start:
            manager->request_state_change(_life_cycle_state_matchmaking_start, 12, k_matchmaking_start_entry_data);
            transition = _life_cycle_state_transition_change;
            break;
        }
    }
    return transition;
}

static __forceinline const long &assemble_match_maximum(const long &a, const long &b)
{
    return a > b ? a : b;
}

// @address 0x3e6408 ?get_gather_party_properties@c_life_cycle_state_handler_matchmaking_assemble_match@@QAA_NPAUs_matchmaking_gather_party_properties@@@Z
bool c_life_cycle_state_handler_matchmaking_assemble_match::get_gather_party_properties(
    s_matchmaking_gather_party_properties *properties)
{
    bool result = false;
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group = manager->get_group_session();
    s_network_session_matchmaking_composition composition;
    if (manager->get_active_squad_session()->is_host() && group->is_host() &&
        network_session_build_matchmaking_composition(group, &composition) ==
            _network_session_matchmaking_composition_build_result_success)
    {
        long seconds = network_time_since(m_start_time) / 1000;
        const c_network_session_parameters *parameters = group->get_session_parameters();
        long preference = 0;
        if (parameters->m_parameters.matchmaking_search_preference.get_allowed())
            preference = *parameters->m_parameters.matchmaking_search_preference.get();
        network_matchmaking_build_gather_party_properties(&composition.session_properties, seconds, preference, properties);
        result = true;
    }
    return result;
}

static __forceinline c_matchmaking_quality *assemble_match_get_quality(c_life_cycle_state_manager *manager)
{
    return (c_matchmaking_quality *)manager->get_matchmaking_quality();
}

// @address 0x3e6c48 ?exit@c_life_cycle_state_handler_matchmaking_assemble_match@@UAAXPAVc_life_cycle_state_handler@@@Z
void c_life_cycle_state_handler_matchmaking_assemble_match::exit(c_life_cycle_state_handler *life_cycle_state_handler)
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad_session = manager->get_active_squad_session();
    c_network_session *group_session = manager->get_group_session();
    if (g_network_join_data.join_queue_mode != 4) network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_queue_closed_while_in_matchmaking;
    if (squad_session->is_host()) {
        unsigned long start_time = m_start_time;
        c_matchmaking_quality *quality = manager->get_matchmaking_quality();
        quality->m_gather_time = (long)(network_time_get() - start_time) / 1000;
        if (life_cycle_state_handler->m_state != 12) {
            if (life_cycle_state_handler->m_state != 7) quality->m_aborted = 1;
            if (group_session->is_host() && (m_flags & 0x40)) quality->m_gather_timed_out = 1;
            quality->m_left_assemble = 1;
            quality->submit_to_webstats();
        }
    }
    group_session->m_close_session = false;
}

// @address 0x3e6b38 ?enter@c_life_cycle_state_handler_matchmaking_assemble_match@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_matchmaking_assemble_match::enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data)
{
    c_network_session *assemble_group_session;
    int current_join_mode;
    unsigned char time_override_enabled;
    unsigned char cond;
    int progress_timestamp;
    int start_timestamp;
    assemble_group_session = get_manager()->get_group_session();
    c_life_cycle_state_handler::enter(from_handler, 0, 0);
    m_flags = 0;
    time_override_enabled = g_network_time_globals.enabled;
    if (!((time_override_enabled == 0))) {
        progress_timestamp = (int)g_network_time_globals.time;
        start_timestamp = progress_timestamp;
    } else {
        start_timestamp = (int)GetTickCount();
        progress_timestamp = (int)g_network_time_globals.time;
        time_override_enabled = g_network_time_globals.enabled;
    }
    m_start_time = start_timestamp;
    if (!((((unsigned char)time_override_enabled) != 0))) {
        progress_timestamp = (int)GetTickCount();
    }
    m_progress_update_time = progress_timestamp;
    m_qos_update_time = 0;
    m_gather_start_time = 0;
    memset(&m_advertised_properties, 0, sizeof(m_advertised_properties));
    memset(&m_address_state, 0, sizeof(m_address_state));
    cond = assemble_group_session->is_host();
    if (cond) {
        assemble_group_session->get_session_parameters()->clear_parameters_with_mask(
            (((((unsigned __int64)1) << 37) | (((unsigned __int64)1) >> 27)) & 0xffffffffffffffffui64));
    }
    assemble_group_session->m_close_session = true;
    current_join_mode = g_network_join_data.join_queue_mode;
    if (current_join_mode != _network_join_process_queued_group_joins &&
        current_join_mode != _network_join_queue_joins_to_group)
        network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_process_queued_group_joins;
    network_group_session_set_disable_host_election(3);
    return;
}

// @address 0x3e6d50 ?handle_missing_required_session_parameter@c_life_cycle_state_handler_matchmaking_assemble_match@@UAAXW4e_network_session_type@@@Z
void c_life_cycle_state_handler_matchmaking_assemble_match::handle_missing_required_session_parameter(e_network_session_type network_session_type)
{
    if (network_session_type == _network_session_type_squad)
    {
        this->squad_session_host_abort_matchmaking();
        return;
    }
    if (network_session_type == _network_session_type_group)
        this->disband_group_session();
}

// @address 0x3e7218 ?ready_to_lockdown@c_life_cycle_state_handler_matchmaking_assemble_match@@AAA_NXZ
bool c_life_cycle_state_handler_matchmaking_assemble_match::ready_to_lockdown()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group = manager->get_group_session();
    const c_hopper_configuration *hopper = (*(c_hopper_configuration * *)&g_multiplayer_game_hopper_globals);
    s_network_session_matchmaking_composition composition;
    bool result = false;
    network_session_build_matchmaking_composition(group, &composition);
    if (composition.session_properties.player_count == hopper->get_maximum_player_count())
        result = true;
    if (!result)
    {
        long start_early_seconds = hopper->get_gather_start_game_early_seconds();
        long give_up_seconds = hopper->get_gather_give_up_seconds();
        long start_early_countdown = assemble_match_maximum(0, start_early_seconds -
            network_time_since(m_gather_start_time) / 1000);
        long give_up_countdown = assemble_match_maximum(0, give_up_seconds -
            network_time_since(m_start_time) / 1000);
        if (start_early_countdown == 0 || give_up_countdown == 0)
            return true;
    }
    return result;
}

template <>
bool c_generic_network_session_parameter<s_game_hopper_picked_game_collection>::set(
    const s_game_hopper_picked_game_collection *value)
{
    bool success = false;
    if (set_allowed()) {
        if (memcmp(&m_data, value, sizeof(m_data)) != 0 || !get_allowed()) {
            memcpy(&m_data, value, sizeof(m_data));
            set_update_required();
        }
        success = true;
    }
    return success;
}

// @address 0x3e6d70 ?update_progress@c_life_cycle_state_handler_matchmaking_assemble_match@@AAAXXZ
void c_life_cycle_state_handler_matchmaking_assemble_match::update_progress()
{
    c_network_session *group = get_manager()->get_group_session();
    c_network_session_parameters *parameters = group->get_session_parameters();
    const c_hopper_configuration *hopper = (*(c_hopper_configuration * *)&g_multiplayer_game_hopper_globals);
    s_replicated_life_cycle_matchmaking_progress progress;
    memset(&progress, 0, sizeof(progress));
    unsigned long contacted = 0;
    if (parameters->m_parameters.matchmaking_progress.get_allowed() && parameters->m_parameters.matchmaking_progress.get()->type == 2)
        contacted = parameters->m_parameters.matchmaking_progress.get()->assembling_match.qos_listener_data_replies_sent;
    progress.type = 2;
    if (network_time_since(m_progress_update_time) / 1000 >= 1)
    {
        s_transport_session_description description;
        s_life_cycle_qos_request stats;
        int managed_session_index = group->managed_session_index();
        if (::managed_session_get_security_information(
                managed_session_index, false, &description, 0) &&
            life_cycle_initialize_qos_request(&description, &stats))
        {
            contacted = (stats.words[3] - m_qos_update_time + contacted) % 255;
            m_qos_update_time = stats.words[3];
        }
        m_progress_update_time = network_time_get();
    }
    progress.assembling_match.qos_listener_data_replies_sent = contacted;
    progress.assembling_match.give_up_timer_seconds = assemble_match_maximum(0, hopper->get_gather_give_up_seconds() -
        network_time_since(m_start_time) / 1000);
    progress.assembling_match.give_up_timer_maximum_seconds = hopper->get_gather_give_up_seconds();
    parameters->m_parameters.matchmaking_progress.set(&progress);
}

// @address 0x3e7150 ?ready_to_start@c_life_cycle_state_handler_matchmaking_assemble_match@@AAA_NXZ

static __forceinline bool assemble_match_parameters_updated(c_network_session_parameters *parameters)
{
    bool result = true;
    result = result && parameters->parameters_transmitted_to_peers(0x40000000) != 0;
    return result;
}

static __forceinline void assemble_match_set_join_mode(long mode)
{
    s_networking_join_data *join = &g_network_join_data;
    if (join->join_queue_mode != mode && !(mode == 3 && join->join_queue_mode == 2))
        network_join_flush_join_queue();
    join->join_queue_mode = (e_network_join_queue_mode)mode;
}

bool c_life_cycle_state_handler_matchmaking_assemble_match::pick_games()
{
    c_network_session *pick_group = get_manager()->get_group_session();
    c_network_session_membership *membership = pick_group->get_session_membership_for_update();
    unsigned long game_mask = ~0UL;
    long player_count = membership->get_player_count();
    for (long peer = membership->get_first_peer(); peer != -1; peer = membership->get_next_peer(peer))
        game_mask &= membership->get_peer(peer)->properties.map_content_mask;
    s_game_hopper_picked_game_collection picked_games;
    bool picked = multiplayer_game_hoppers_pick_random_game_collection(player_count, game_mask, &picked_games);
    if (picked)
    {
        c_network_session_parameters *parameters = pick_group->get_session_parameters();
        for (long game_index = 0; game_index < 4; game_index++)
        {
        }
        parameters->m_parameters.matchmaking_game_list.set(&picked_games);
    }
    return picked;
}

// @address 0x3e6510 ?update@c_life_cycle_state_handler_matchmaking_assemble_match@@UAAXXZ
void c_life_cycle_state_handler_matchmaking_assemble_match::update()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group = manager->get_group_session();
    if (group->is_host())
    {
        if (!continue_assembling())
            this->disband_group_session();
        else
        {
            life_cycle_update_session_qos_address((c_life_cycle_state_handler *)this, &m_address_state);
            update_progress();
            if (m_flag_bits.test(5))
            {
                c_network_session_membership *membership = group->get_session_membership_for_update();
                unsigned long evict_mask = *group->get_session_parameters()->m_parameters.matchmaking_assembly_peer_evict_mask.get();
                bool evicted = true;
                for (long peer = membership->get_first_peer(); peer != -1; peer = membership->get_next_peer(peer))
                {
                    if (evict_mask & (1 << peer))
                        evicted = false;
                }
                if (evicted)
                    m_flag_bits.set(5, false);
            }
            if (!(m_flag_bits.test(5)))
            {
                s_matchmaking_gather_party_properties properties;
                if (get_gather_party_properties(&properties) &&
                    memcmp(&properties.session_properties, &m_advertised_properties, sizeof(m_advertised_properties)))
                {
                    memcpy(&m_advertised_properties, &properties.session_properties, sizeof(m_advertised_properties));
                    c_life_cycle_state_handler::update_advertised_session(&m_advertised_properties);
                }
                if (!(m_flag_bits.test(1)) && (m_flag_bits.test(0)))
                {
                    group->get_session_membership_for_update()->set_slot_counts(0,
                        (*(c_hopper_configuration * *)&g_multiplayer_game_hopper_globals)->get_maximum_player_count(), false);
                    m_flag_bits.set(1, true);
                }
                if (!(m_flag_bits.test(2)))
                {
                    if (!ready_to_start())
                        assemble_match_set_join_mode(3);
                    else
                    {
                        m_gather_start_time = network_time_get();
                        m_flag_bits.set(2, true);
                    }
                }
                if (m_flag_bits.test(2))
                {
                    if (!ready_to_start())
                        m_flag_bits.set(2, false);
                    else
                    {
                        if (!(m_flag_bits.test(3)) && ready_to_lockdown())
                        {
                            assemble_match_set_join_mode(4);
                            if (!pick_games())
                                this->disband_group_session();
                            else
                                m_flag_bits.set(3, true);
                        }
                        if (m_flag_bits.test(3))
                        {
                            if (!ready_to_lockdown())
                                m_flag_bits.set(3, false);
                            else if (assemble_match_parameters_updated(get_manager()->get_group_session()->get_session_parameters()))
                                group->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_arbitrating);
                        }
                    }
                }
            }
        }
    }
    c_network_session *squad = manager->get_active_squad_session();
    if (squad->is_host())
    {
        this->update_matchmaking_search_criteria(true);
        if (!group->is_host())
        {
            c_network_session_parameters *parameters = group->get_session_parameters();
            if (parameters->m_parameters.matchmaking_assembly_peer_evict_mask.get_allowed())
        {
            unsigned long evict_mask = *parameters->m_parameters.matchmaking_assembly_peer_evict_mask.get();
            c_network_session_membership *squad_membership = squad->get_session_membership_for_update();
            c_network_session_membership *group_membership = group->get_session_membership_for_update();
            bool evicted = false;
            for (long group_peer = group_membership->get_first_peer(); group_peer != -1; group_peer = group_membership->get_next_peer(group_peer))
            {
                if (evict_mask & (1 << group_peer))
                {
                    const s_network_session_peer *peer = group_membership->get_peer(group_peer);
                    for (long squad_peer = squad_membership->get_first_peer(); squad_peer != -1; squad_peer = squad_membership->get_next_peer(squad_peer))
                    {
                        if (transport_secure_address_compare(&peer->secure_address, squad_membership->get_peer_address(squad_peer)))
                        {
                            evicted = true;
                            break;
                        }
                    }
                }
                if (evicted)
                    break;
            }
            if (evicted)
                squad->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_start);
        }
        }
    }
}

bool c_life_cycle_state_handler_matchmaking_assemble_match::ready_to_start()
{
    bool result = false;
    c_network_session *group = get_manager()->get_group_session();
    if (group->m_session_membership.all_peers_established())
    {
        const c_hopper_configuration *hopper = (*(c_hopper_configuration * *)&g_multiplayer_game_hopper_globals);
        s_network_session_matchmaking_composition composition;
        network_session_build_matchmaking_composition(group, &composition);
        long player_count = composition.session_properties.player_count;
        if (player_count == hopper->get_maximum_player_count())
            result = true;
        if (!result && player_count >= hopper->get_minimum_player_count())
            result = true;
        result = result && this->pick_teams_for_group_internal(false, 0);
    }
    return result;
}

static __forceinline bool assemble_match_player_valid(const s_matchmaking_session_properties *properties, long index)
{
    return (properties->player_valid_flags[index >> 5] & (1 << (index & 31))) != 0;
}

// @address 0x3e7358 ?get_peer_mask_to_evict_for_team_balance@c_life_cycle_state_handler_matchmaking_assemble_match@@AAAKXZ
unsigned long c_life_cycle_state_handler_matchmaking_assemble_match::get_peer_mask_to_evict_for_team_balance()
{
    unsigned long peer_evict_mask = 0;
    c_network_session *group = get_manager()->get_group_session();
    s_network_session_matchmaking_composition composition;
    if (network_session_build_matchmaking_composition(group, &composition) ==
        _network_session_matchmaking_composition_build_result_success)
    {
        const c_hopper_configuration *hopper = (*(c_hopper_configuration * *)&g_multiplayer_game_hopper_globals);
        if (hopper->is_team_based() && composition.session_properties.player_count > hopper->get_minimum_player_count() &&
            !this->pick_teams_for_group_internal(false, 0))
        {
            long local_peer_index = group->m_session_membership.m_local_peer_index;
            unsigned long party_exclude_mask = 0;
            bool can_pick_teams = false;
            long local_party_index = -1;
            for (long player_index = 0; player_index < 16; ++player_index)
            {
                if (assemble_match_player_valid(&composition.session_properties, player_index) &&
                    composition.session_properties.players[player_index].peer_index == local_peer_index)
                {
                    local_party_index = composition.session_properties.players[player_index].party_index;
                    break;
                }
            }
            long party_count = composition.party_count;
            for (long party_size = 1; party_size < 16; ++party_size)
            {
                for (long party_index = 0; party_index < party_count; ++party_index)
                {
                    if (party_index != local_party_index && composition.parties[party_index].player_count == party_size)
                    {
                        party_exclude_mask |= 1UL << party_index;
                        if (this->pick_teams_for_group_internal(false, party_exclude_mask))
                        {
                            can_pick_teams = true;
                            break;
                        }
                    }
                }
                if (can_pick_teams)
                    break;
            }
            if (can_pick_teams)
            {
                for (long player_index = 0; player_index < 16; ++player_index)
                {
                    if (assemble_match_player_valid(&composition.session_properties, player_index) &&
                        (party_exclude_mask & (1 << composition.session_properties.players[player_index].party_index)))
                        peer_evict_mask |= 1 << composition.session_properties.players[player_index].peer_index;
                }
            }
        }
    }
    return peer_evict_mask;
}
// @address 0x3e6198 ?get_progress@c_life_cycle_state_handler_matchmaking_assemble_match@@QAAXPAUs_life_cycle_matchmaking_progress@@@Z
void c_life_cycle_state_handler_matchmaking_assemble_match::get_progress(s_life_cycle_matchmaking_progress *progress_out)
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *group_session = manager->get_group_session();
    c_network_session *squad_session = manager->get_active_squad_session();
    const c_network_session_parameters *squad_parameters = squad_session->get_session_parameters();
    const c_network_session_parameters *group_parameters = group_session->get_session_parameters();
    if (squad_parameters->m_parameters.matchmaking_search_preference.get_allowed() &&
        squad_parameters->m_parameters.matchmaking_progress_search_criteria.get_allowed() &&
        group_parameters->m_parameters.matchmaking_progress.get_allowed() &&
        group_parameters->m_parameters.matchmaking_hopper_list.get_allowed() &&
        group_parameters->m_parameters.matchmaking_hopper.get_allowed()) {
        const s_replicated_life_cycle_matchmaking_progress *progress = group_parameters->m_parameters.matchmaking_progress.get();
        if (progress->type == 2) {
            const e_matchmaking_search_preference *search_preference = squad_parameters->m_parameters.matchmaking_search_preference.get();
            const s_replicated_life_cycle_matchmaking_progress_search_criteria *search_criteria =
                squad_parameters->m_parameters.matchmaking_progress_search_criteria.get();
            progress_out->type = 2;
            progress_out->assemble = progress->assembling_match;
            progress_out->replicated_search_criteria = *search_criteria;
            progress_out->search_preference = *search_preference;
            const c_network_session_membership *group_membership = group_session->get_session_membership();
            const c_network_session_membership *squad_membership = squad_session->get_session_membership();
            const s_network_session_matchmaking_hopper_list *hopper_list = group_parameters->m_parameters.matchmaking_hopper_list.get();
            const s_network_session_matchmaking_hopper *hopper =
                group_parameters->m_parameters.matchmaking_hopper.get();
            for (long hopper_index = 0; hopper_index < hopper_list->hopper_count; hopper_index++) {
                if (hopper_list->hoppers[hopper_index].identifier == hopper->hopper_identifier) {
                    ::utf8_string_to_wchar_string((utf8 const *)(const unsigned char *)hopper_list->hoppers[hopper_index].name,
                        (wchar_t *)(progress_out->hopper_name), 32, (long *)(0));
                    break;
                }
            }
            progress_out->minimum_players = hopper->is_ranked;
            progress_out->maximum_players = hopper->team_game;
            progress_out->squad_player_count = squad_session->get_session_membership()->get_player_count();
            long established_count = 0;
            for (long player_index = group_membership->get_first_player(); player_index != -1;
                 player_index = group_membership->get_next_player(player_index)) {
                if (group_membership->m_current_membership.peers[group_membership->m_current_membership.players[player_index].peer_index].connection_state == 8)
                    established_count++;
            }
            progress_out->group_player_count = established_count;
        }
    }
}

static __forceinline bool life_cycle_handler_matchmaking_assemble_match_composition_valid(c_network_session *group)
{
    s_network_session_matchmaking_composition composition;
    bool valid = false;
    if (network_session_build_matchmaking_composition(group, &composition) ==
        _network_session_matchmaking_composition_build_result_success)
        valid = true;
    return valid;
}

// @address 0x3e6f68 ?continue_assembling@c_life_cycle_state_handler_matchmaking_assemble_match@@AAA_NXZ
bool c_life_cycle_state_handler_matchmaking_assemble_match::continue_assembling()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group = manager->get_group_session();
    bool result = (*(c_hopper_configuration * *)&g_multiplayer_game_hopper_globals) != 0;
    if (result && group->get_session_membership()->all_peers_established() &&
        !c_life_cycle_state_handler::session_has_minimum_player_count_to_start_game_in_hopper(group))
        result = group->get_session_membership()->m_current_membership.peer_count >
            manager->get_active_squad_session()->get_session_membership()->m_current_membership.peer_count;
    if (result && group->get_session_membership()->all_peers_established()) {
        if (!life_cycle_handler_matchmaking_assemble_match_composition_valid(group))
            result = false;
    }
    if (result && !m_flag_bits.test(2) &&
        !m_flag_bits.test(5))
    {
        const c_hopper_configuration *hopper = (*(c_hopper_configuration * *)&g_multiplayer_game_hopper_globals);
        if (network_time_since(m_start_time) / 1000 > hopper->get_gather_give_up_seconds()) {
            m_flag_bits.set(6, true);
            if (!group->get_session_membership()->all_peers_established())
                return false;
            if (m_flag_bits.test(4))
                return false;
            unsigned long peer_mask = get_peer_mask_to_evict_for_team_balance();
            if (!peer_mask)
                return false;
            s_networking_join_data *join_data = &g_network_join_data;
            if (join_data->join_queue_mode != 4)
                network_join_flush_join_queue();
            join_data->join_queue_mode = _network_join_queue_closed_while_in_matchmaking;
            // match: 0x3e6f68: the unsigned-long evict-mask setter shares this campaign-difficulty COMDAT.
            ((c_generic_network_session_parameter<e_campaign_difficulty_level> *)&group->get_session_parameters()
                    ->m_parameters.matchmaking_assembly_peer_evict_mask)
                ->set((const e_campaign_difficulty_level *)&peer_mask);
            m_flag_bits.set(4, true);
            m_flag_bits.set(5, true);
            bool success = result;
            return success;
        }
    }
    return result;
}

