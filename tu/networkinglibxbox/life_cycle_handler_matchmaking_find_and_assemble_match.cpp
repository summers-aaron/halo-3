#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
#include "game/multiplayer_game_hopper.h"
// networkinglibxbox/life_cycle_handler_matchmaking_find_and_assemble_match.obj: from the records 0x3e53f8 0x3e5458 0x3e5520 0x3e5628 0x3e56f0 0x3e5888 0x3e5960 0x3e5a38 0x3e5c10 0x3e60a0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/player_types.h"
#include "networking/logic/logic_matchmaking_seeker.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/logic_session_tracker_unsuitable_reason_types.h"
#include "networking/logic/logic_matchmaking_composition_types.h"
#include "cseries/cseries_wrapped.h"
#include "networking/logic/network_session_interface.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_join_queue_types.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session_parameters_session.h"
#include "networking/session/network_session.h"
#include "networking/transport/transport_security_types.h"
#include "networking/session_tracker_globals_types.h"
#include "networking/life_cycle_session_address_state_types.h"
#include "networking/life_cycle_qos_request_types.h"
#include "networking/suitable_matchmaking_session_types.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"
#include "cseries/crt_xdk.h"
#include <string.h>
#include "game/hopper_configuration.h"
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"
#include "networking/logic/logic_matchmaking_composition.h"
#include "networking/logic/logic_matchmaking_composition_data_types.h"
#include "networking/logic/logic_matchmaking_seeker_search_types.h"
#include "networking/logic/logic_session_tracker.h"
#include "networking/logic/session_tracker_class.h"
#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
#include "networking/network_time_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_find_and_assemble_match_internals.h"

// @address 0x8c447c g_network_time_globals
// @address 0x3e53f8 ?get_established_peer_count@c_network_session_membership@@QBAJXZ
long c_network_session_membership::get_established_peer_count() const
{
    long established_peer_count = 0;
    for (long peer_index = get_first_peer(); peer_index != -1; peer_index = get_next_peer(peer_index))
    {
        if (is_peer_established(peer_index))
            established_peer_count++;
    }
    return established_peer_count;
}

// @address 0x3e56f0 ?update_for_state_transition@c_life_cycle_state_handler_matchmaking_find_and_assemble_match@@UAA?AW4e_life_cycle_state_transition_type@@XZ
e_life_cycle_state_transition_type c_life_cycle_state_handler_matchmaking_find_and_assemble_match::update_for_state_transition()
{
    c_life_cycle_state_manager *m_manager = this->m_manager;
    c_network_session *squad = m_manager->m_active_squad_session;
    c_network_session *group = m_manager->m_group_session;
    e_life_cycle_state_transition_type result = _life_cycle_state_transition_none;
    if (squad->is_host()) {
        switch (squad->get_session_parameters()->m_parameters.session_mode.get()) {
        case 7:
            result = _life_cycle_state_transition_continue;
            break;
        case 6:
            {
                int start_mode = 0;
                m_manager->request_state_change(_life_cycle_state_matchmaking_start, 4, &start_mode);
                result = _life_cycle_state_transition_change;
            }
            break;
        case 9:
            m_manager->request_state_change(_life_cycle_state_matchmaking_assemble_match, 0, 0);
            result = _life_cycle_state_transition_change;
            break;
        }
        if (result == _life_cycle_state_transition_continue && group->is_host() && group->get_session_parameters()->m_parameters.session_mode.get() == 10) {
            squad->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_start);
            m_manager->request_state_change(_life_cycle_state_matchmaking_start, 12, k_matchmaking_start_entry_data);
            result = _life_cycle_state_transition_change;
        }
    }
    return result;
}

static __forceinline unsigned long find_assemble_network_time_get()
{
    if (g_network_time_globals.enabled)
        return g_network_time_globals.time;
    return GetTickCount();
}
static __forceinline long find_assemble_network_time_since(unsigned long time)
{
    return (long)(find_assemble_network_time_get() - time);
}
static __forceinline s_find_assemble_quality *find_assemble_quality(c_life_cycle_state_manager *m_manager)
{
    return (s_find_assemble_quality *)m_manager->get_matchmaking_quality();
}
static __forceinline void find_assemble_seeker_search_end(s_find_assemble_matchmaking_seeker *seeker)
{
    if (seeker->m_flags.test(_matchmaking_seeker_online_search_in_progress_bit))
        seeker->m_flags.set(_matchmaking_seeker_online_search_in_progress_bit, false);
    ((c_session_tracker *)&session_tracker_globals)->release_storage();
    seeker->m_flags.set(_matchmaking_seeker_flag_0, false);
}
// @address 0x3e5888 ?enter@c_life_cycle_state_handler_matchmaking_find_and_assemble_match@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_matchmaking_find_and_assemble_match::enter(c_life_cycle_state_handler *previous_handler, long entry_data_size, void *entry_data)
{
    c_life_cycle_state_handler::enter(previous_handler, 0, 0);
    flags.clear();
    m_time = find_assemble_network_time_get();
    leaving_start_time = 0;
    find_and_assemble_data = *(const s_find_and_assemble_data *)entry_data;
    memset(&m_advertised_session_properties, 0, sizeof(m_advertised_session_properties));
    memset(&m_session, 0, sizeof(m_session));
    for (long i = 0; i < 9; i++)
        session_address_words[i] = 0;
    network_group_session_set_disable_host_election(3);
    if (g_network_join_data.join_queue_mode != 2)
        network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_queue_joins_to_group;
}
// @address 0x3e5960 ?exit@c_life_cycle_state_handler_matchmaking_find_and_assemble_match@@UAAXPAVc_life_cycle_state_handler@@@Z
void c_life_cycle_state_handler_matchmaking_find_and_assemble_match::exit(c_life_cycle_state_handler *next_handler)
{
    if (flags.test(2)) {
        unsigned long search_start_time = seeker.get_initial_search_start_time();
        s_find_assemble_quality *quality = find_assemble_quality(m_manager);
        quality->search_time = find_assemble_network_time_since(search_start_time) / 1000;
        seeker.add_session_searches_to_matchmaking_quality((c_matchmaking_quality *)quality);
        find_assemble_seeker_search_end(&seeker);
        if (next_handler->get_state() != 11) {
            quality->aborted = 1;
            ((c_matchmaking_quality *)quality)->submit_to_webstats();
        }
    }
}
// @address 0x3e60a0 ?update_leaving@c_life_cycle_state_handler_matchmaking_find_and_assemble_match@@AAAXXZ
void c_life_cycle_state_handler_matchmaking_find_and_assemble_match::update_leaving()
{
    if (flags.test(2)) {
        if (seeker.get_session_search_stage() == 3 && !flags.test(6)) {
            leaving_start_time = find_assemble_network_time_get();
            flags.set(6, true);
        }
        if (flags.test(6) && !flags.test(4)) {
            long seconds = find_assemble_network_time_since(leaving_start_time) / 1000;
            if (seconds >= g_network_configuration.stream_period) {
                network_session_request_parameter_set_value(
                    &m_manager->m_active_squad_session->get_session_parameters()->m_parameters.matchmaking_messaging, 1);
                squad_session_host_abort_matchmaking();
            }
        }
    }
}

// @address 0x3e5458 ?get_search_party_properties@c_life_cycle_state_handler_matchmaking_find_and_assemble_match@@QAA_NPAUs_matchmaking_search_party_properties@@@Z
bool c_life_cycle_state_handler_matchmaking_find_and_assemble_match::get_search_party_properties(s_matchmaking_search_party_properties *properties)
{
    bool result = false;
    c_life_cycle_state_manager *state_manager = m_manager;
    c_network_session *squad = state_manager->m_active_squad_session;
    s_network_session_matchmaking_composition composition;
    if (squad->is_host() && state_manager->m_group_session->is_host() &&
        network_session_build_matchmaking_composition(squad, &composition) == _network_session_matchmaking_composition_build_result_success) {
        long seconds = find_assemble_network_time_since(seeker.get_initial_search_start_time()) / 1000;
        network_matchmaking_build_search_party_properties(&composition.session_properties, seconds, seeker.m_search_preference, properties);
        result = true;
    }
    return result;
}

// @address 0x3e5520 ?get_gather_party_properties@c_life_cycle_state_handler_matchmaking_find_and_assemble_match@@QAA_NPAUs_matchmaking_gather_party_properties@@@Z
bool c_life_cycle_state_handler_matchmaking_find_and_assemble_match::get_gather_party_properties(s_matchmaking_gather_party_properties *properties)
{
    bool result = false;
    c_network_session *group = m_manager->m_group_session;
    s_network_session_matchmaking_composition composition;
    if (m_manager->m_active_squad_session->is_host() && group->is_host() &&
        network_session_build_matchmaking_composition(group, &composition) == _network_session_matchmaking_composition_build_result_success) {
        long seconds = find_assemble_network_time_since(m_time) / 1000;
        const c_network_session_parameters *parameters = group->get_session_parameters();
        long preference = 0;
        if (parameters->m_parameters.matchmaking_search_preference.get_allowed())
            preference = *parameters->m_parameters.matchmaking_search_preference.get();
        network_matchmaking_build_gather_party_properties(&composition.session_properties, seconds, preference, properties);
        result = true;
    }
    return result;
}

static __forceinline s_networking_join_data *life_cycle_handler_matchmaking_find_and_assemble_match_join_data()
{
    return &g_network_join_data;
}
static __forceinline void life_cycle_handler_matchmaking_find_and_assemble_match_leave_group(long reason)
{
    s_networking_join_data *join = life_cycle_handler_matchmaking_find_and_assemble_match_join_data();
    join->request_group_disband = true;
    join->group_disband_type = reason;
    join->request_join_squad_to_group = false;
    join->request_join_squad_to_target_group = false;
    join->request_join_group_to_group = false;
}
// @address 0x3e5a38 ?update_gathering@c_life_cycle_state_handler_matchmaking_find_and_assemble_match@@AAAXXZ
void c_life_cycle_state_handler_matchmaking_find_and_assemble_match::update_gathering()
{
    c_network_session *group = m_manager->m_group_session;
    c_network_session *squad = m_manager->m_active_squad_session;
    c_network_session_membership *group_membership = group->get_session_membership_for_update();
    c_network_session_parameters *squad_parameters = squad->get_session_parameters();
    s_networking_join_data *join = life_cycle_handler_matchmaking_find_and_assemble_match_join_data();
    if (join->join_queue_mode == 2 && find_assemble_network_time_since(m_time) >= g_network_configuration.group_join_delay) {
        if (join->join_queue_mode != 3 && join->join_queue_mode != 2)
            network_join_flush_join_queue();
        join->join_queue_mode = _network_join_process_queued_group_joins;
    }
    if (group_membership->get_established_peer_count() > squad->get_session_membership()->get_peer_count() ||
        (group_membership->all_peers_established() && session_has_minimum_player_count_to_start_game_in_hopper(group))) {
        if (flags.test(4))
            flags.set(4, false);
        flags.set(3, false);
        life_cycle_handler_matchmaking_find_and_assemble_match_leave_group(0);
        squad_parameters->m_parameters.session_mode.set(_network_session_mode_matchmaking_slave);
    }
    s_matchmaking_gather_party_properties properties;
    if (get_gather_party_properties(&properties) &&
        memcmp(&properties.session_properties, &m_advertised_session_properties, sizeof(properties.session_properties))) {
        memcpy(&m_advertised_session_properties, &properties.session_properties, sizeof(properties.session_properties));
        update_advertised_session(&m_advertised_session_properties);
    }
    if (!flags.test(1) && flags.test(0)) {
        group_membership->set_slot_counts(0, (*(c_hopper_configuration * *)&g_multiplayer_game_hopper_globals)->get_maximum_player_count(), false);
        flags.set(1, true);
    }
}
static __forceinline void life_cycle_handler_matchmaking_find_and_assemble_match_notify_join_failed(
    c_matchmaking_seeker *seeker, const s_transport_session_description *description, long reason)
{
    ++seeker->m_session_searches[seeker->m_current_session_search_index].join_failures[reason];
    network_session_tracker_mark_session_unsuitable(description, _session_unsuitable_matchmaking_rejected);
}
static __forceinline void life_cycle_handler_matchmaking_find_and_assemble_match_begin_join(const s_transport_secure_identifier *secure_identifier,
    const s_transport_secure_address *host_address, const s_transport_secure_key *key, const s_matchmaking_search_party_properties *properties)
{
    s_networking_join_data *join = life_cycle_handler_matchmaking_find_and_assemble_match_join_data();
    join->group_join_result = 1;
    join->queued_group_join.session_class = _network_session_class_xbox_live;
    join->queued_group_join.identifier = *secure_identifier;
    memcpy(&join->queued_group_join.key, key, sizeof(join->queued_group_join.key));
    memcpy(&join->queued_group_join.address, host_address, sizeof(join->queued_group_join.address));
    join->queued_group_join.nonce = transport_secure_nonce_generate();
    join->queued_group_join.timestamp = 0;
    join->queued_group_join.join_as_friend = true;
    join->group_join_state = 2;
    memcpy(&join->search_properties, properties, sizeof(*properties));
    join->request_join_squad_to_group = false;
    join->request_join_squad_to_target_group = true;
    join->request_join_group_to_group = false;
}
// @address 0x3e5c10 ?update_joining@c_life_cycle_state_handler_matchmaking_find_and_assemble_match@@AAAXXZ
void c_life_cycle_state_handler_matchmaking_find_and_assemble_match::update_joining()
{
    c_life_cycle_state_manager *state_manager = m_manager;
    c_network_session *squad = state_manager->m_active_squad_session;
    s_network_session_matchmaking_composition composition;
    if (!flags.test(2)) {
        network_session_build_matchmaking_composition(squad, &composition);
        s_matchmaking_seeker_search_start_parameters parameters;
        memset(&parameters, 0, sizeof(parameters));
        parameters.search_preference = (e_matchmaking_search_preference)*squad->get_session_parameters()->m_parameters.matchmaking_search_preference.get();
        parameters.force_desperation = find_and_assemble_data.force_desperation;
        parameters.initial_desperation_session_search_count = find_and_assemble_data.initial_session_search_count;
        parameters.initial_desperation_sessions_found_count = find_and_assemble_data.initial_sessions_found_count;
        parameters.flag_2_enabled = true;
        seeker.search_begin(&parameters, &composition.session_properties, state_manager->get_matchmaking_quality());
        flags.set(2, true);
    }
    if (flags.test(2)) {
        network_session_build_matchmaking_composition(squad, &composition);
        if (memcmp(&seeker.m_session_properties, &composition.session_properties, sizeof(composition.session_properties)))
            seeker.set_session_properties(&composition.session_properties);
        seeker.update();
        bool can_join = find_assemble_network_time_since(seeker.get_initial_search_start_time()) > g_network_configuration.search_join_delay;
        bool stale = find_assemble_network_time_since(seeker.get_current_search_start_time()) >= g_network_configuration.search_stale_timeout;
        bool alone = squad->get_session_membership()->get_peer_count() == state_manager->m_group_session->get_session_membership()->get_peer_count();
        if (stale && !flags.test(4)) {
            if (seeker.m_flags.m_storage & 4)
                seeker.m_flags.m_storage &= ~4;
            seeker.m_flags.m_storage &= ~16;
            ((c_session_tracker *)&session_tracker_globals)->clear();
        }
        flags.set(3, can_join);
        s_networking_join_data *join = life_cycle_handler_matchmaking_find_and_assemble_match_join_data();
        if (flags.test(3) && !flags.test(4) && !flags.test(5) && !stale &&
            !network_join_is_join_to_squad_in_progress() && join->join_queue_entry_count == 0 && alone) {
            if (seeker.get_session_to_join(&m_session)) {
                s_matchmaking_search_party_properties properties;
                memset(&properties, 0, sizeof(properties));
                if (!get_search_party_properties(&properties)) {
                    life_cycle_handler_matchmaking_find_and_assemble_match_notify_join_failed(&seeker, &m_session.description, 2);
                } else {
                    properties.latency = m_session.qos_status.ping_msec_median;
                    life_cycle_handler_matchmaking_find_and_assemble_match_begin_join(&m_session.description.secure_identifier,
                        &m_session.description.secure_address, &m_session.description.secure_key, &properties);
                    flags.set(4, true);
                }
            }
        } else if (flags.test(4)) {
            long result = join->group_join_result;
            if (result != 1) {
                if (result != 0) {
                    life_cycle_handler_matchmaking_find_and_assemble_match_notify_join_failed(&seeker, &m_session.description, result);
                } else {
                    c_network_session *group = state_manager->m_group_session;
                    if (group->is_host())
                        group->get_session_membership_for_update()->set_slot_counts(0, 0, false);
                    life_cycle_handler_matchmaking_find_and_assemble_match_leave_group(1);
                    state_manager->m_matchmaking_quality.m_ping = m_session.qos_status.ping_msec_median;
                    flags.set(5, true);
                }
                flags.set(4, false);
                join->group_join_result = 0;
            }
        } else if (flags.test(5) && !network_join_is_leaving_group()) {
            flags.set(5, false);
            squad->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_slave);
        }
    }
}

static __forceinline bool life_cycle_handler_matchmaking_find_and_assemble_match_composition_valid(const c_network_session *session)
{
    s_network_session_matchmaking_composition composition;
    bool valid = false;
    if (network_session_build_matchmaking_composition(session, &composition) == _network_session_matchmaking_composition_build_result_success)
        valid = true;
    return valid;
}

// @address 0x3e5628 ?update@c_life_cycle_state_handler_matchmaking_find_and_assemble_match@@UAAXXZ
void c_life_cycle_state_handler_matchmaking_find_and_assemble_match::update()
{
    c_life_cycle_state_manager *state_manager = m_manager;
    c_network_session *squad = state_manager->m_active_squad_session;
    if (squad->is_host()) {
        if (!life_cycle_handler_matchmaking_find_and_assemble_match_composition_valid(squad)) {
            squad_session_host_abort_matchmaking();
            return;
        }
        c_network_session *group = state_manager->m_group_session;
        update_matchmaking_search_criteria(false);
        update_search_progress(&seeker, flags.test(6), leaving_start_time);
        update_joining();
        update_leaving();
        if (group->is_host()) {
            life_cycle_update_session_qos_address(this, &session_address_state);
            update_gathering();
        }
    }
}
