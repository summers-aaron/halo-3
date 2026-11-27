// networkinglibxbox/life_cycle_handler_matchmaking_find_match.obj: from the records 0x3e4a78 0x3e4b28 0x3e4bd8 0x3e4d50 0x3e4dc8 0x3e4ea0 0x3e52a8
// @flags /TP /O2 /Oc
// @address 0x3e4b28 ?update@c_life_cycle_state_handler_matchmaking_find_match@@UAAXXZ
// @address 0x3e4bd8 ?update_for_state_transition@c_life_cycle_state_handler_matchmaking_find_match@@UAA?AW4e_life_cycle_state_transition_type@@XZ
// @address 0x3e4dc8 ?exit@c_life_cycle_state_handler_matchmaking_find_match@@UAAXPAVc_life_cycle_state_handler@@@Z
#include "cseries/crt_xdk.h"
#include "networking/logic/session_tracker_class.h"
#include "networking/session_tracker_globals_types.h"
#include "networking/logic/logic_session_tracker.h"
#include "networking/logic/logic_matchmaking_seeker.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/logic_session_tracker_unsuitable_reason_types.h"
#include "networking/logic/logic_matchmaking_composition_types.h"
#include "networking/online/online_nat_types.h"
#include <string.h>
#include "networking/logic/network_session_interface.h"
#include "networking/logic/network_join.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include "networking/session/network_session_parameters_session.h"
#include "networking/transport/transport_security_types.h"
#include "networking/session_tracker_globals_types.h"
#include "networking/suitable_matchmaking_session_types.h"
#include "networking/logic/logic_matchmaking_composition.h"
class c_matchmaking_quality;


#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"


#include "networking/online/online_xenon.h"
#include "networking/logic/logic_matchmaking_composition_data_types.h"
#include "networking/logic/network_join_queue_types.h"
#include "game/hopper_skill_match_state_types.h"
#include "game/multiplayer_game_hopper.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_quality_legacy.h"
#include "networking/network_time_since_access.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "networking/logic/logic_session_tracker.h"
#include "networking/logic/logic_matchmaking_seeker_search_types.h"





#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_find_match.h"

// @address 0x8c447c g_network_time_globals













class c_matchmaking_quality;



#pragma pack(push, 4)
#pragma pack(pop)



void c_life_cycle_state_handler_matchmaking_find_match::update()
{
    int buffer[250];
    c_network_session *squad = m_manager->m_active_squad_session;
    c_life_cycle_state_handler *handler = (c_life_cycle_state_handler *)this;

    if (squad->is_host()) {
        bool ok = false;
        if (::network_session_build_matchmaking_composition(
                squad, (s_network_session_matchmaking_composition *)buffer) == 18)
        {
            ok = true;
        }
        if (!ok) {
            handler->squad_session_host_abort_matchmaking();
        } else {
            handler->update_matchmaking_search_criteria(false);
            handler->update_search_progress(&m_seeker, (m_update_flags.m_storage >> 2) & 1, m_value);
            update_joining();
            update_leaving();
        }
    }
}

e_life_cycle_state_transition_type c_life_cycle_state_handler_matchmaking_find_match::update_for_state_transition()
{
    e_life_cycle_state_transition_type transition = _life_cycle_state_transition_none;
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *squad = manager->m_active_squad_session;

    if (squad->is_host()) {
        switch (squad->get_session_parameters()->m_parameters.session_mode.get()) {
        case 6: {
            long request[3];

            request[0] = 1;
            request[1] = m_seeker.get_total_sessions_contacted_count();
            request[2] = m_seeker.get_total_session_search_count() % 63;
            manager->request_state_change(_life_cycle_state_matchmaking_start, sizeof(request), request);
            transition = _life_cycle_state_transition_change;
            break;
        }
        case 7:
            transition = _life_cycle_state_transition_continue;
            break;
        case 9:
            manager->request_state_change(_life_cycle_state_matchmaking_assemble_match, 0, 0);
            transition = _life_cycle_state_transition_change;
            break;
        }
    }
    return transition;
}

void c_life_cycle_state_handler_matchmaking_find_match::update_leaving()
{
    if (m_update_flags.m_storage & 1) {
        if (m_seeker.get_session_search_stage() == 3 && (m_update_flags.m_storage & 4) == 0) {
            m_value = network_time_get();
            m_update_flags.set(_matchmaking_find_match_search_complete_bit, true);
        }
        if ((m_update_flags.m_storage & 4) != 0 && (m_update_flags.m_storage & 2) == 0) {
            long elapsed_seconds = network_time_since(m_value) / 1000;
            const s_hopper_skill_match_state *hopper = *(s_hopper_skill_match_state **)&g_multiplayer_game_hopper_globals;

            if (online_get_nat_type() == _online_nat_type_strict) {
                if (elapsed_seconds >= g_network_configuration.stream_period) {
                    c_network_session *session = m_manager->m_active_squad_session;
                    network_session_request_parameter_set_value(
                        &session->get_session_parameters()->m_parameters.matchmaking_messaging, 1);
                    this->squad_session_host_abort_matchmaking();
                }
            } else if (elapsed_seconds >= hopper->maximum_total_matchmaking_seconds) {
                c_network_session *session = m_manager->m_active_squad_session;
                c_network_session_parameters *parameters = session->get_session_parameters();
                parameters->m_parameters.session_mode.set(_network_session_mode_matchmaking_start);
                bool value = true;
                parameters->m_parameters.matchmaking_suppress_progress.set(&value);
            }
        }
    }
}

// @address 0x3e4d50 ?enter@c_life_cycle_state_handler_matchmaking_find_match@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_matchmaking_find_match::enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data)
{
    c_life_cycle_state_handler::enter(from_handler, 0, 0);
    m_update_flags.m_storage = 0;
    m_value = 0;
    memset(&m_session, 0, sizeof(m_session));
    network_group_session_set_disable_host_election(3);
    if (g_network_join_data.join_queue_mode != _network_join_queue_closed_while_in_matchmaking)
        network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_queue_closed_while_in_matchmaking;
}



static __forceinline void life_cycle_handler_matchmaking_find_match_seeker_search_end(c_matchmaking_seeker *seeker)
{
    if (seeker->m_flags.m_storage & 4) {
        seeker->m_flags.m_storage &= ~4;
    }
    ((c_session_tracker *)&session_tracker_globals)->release_storage();
    seeker->m_flags.m_storage &= ~1;
}

void c_life_cycle_state_handler_matchmaking_find_match::exit(c_life_cycle_state_handler *life_cycle_state_handler)
{
    if (m_update_flags.m_storage & 1) {
        unsigned long start_time = m_seeker.m_initial_search_start_time;
        c_matchmaking_quality *quality = m_manager->get_matchmaking_quality();
        quality->m_search_time = network_time_since(start_time) / 1000;
        m_seeker.add_session_searches_to_matchmaking_quality((c_matchmaking_quality *)quality);
        life_cycle_handler_matchmaking_find_match_seeker_search_end(&m_seeker);
        if (life_cycle_state_handler->get_state() != 11) {
            quality->m_aborted = 1;
            quality->submit_to_webstats();
        }
    }
}


// @address 0x3e4a78 ?get_search_party_properties@c_life_cycle_state_handler_matchmaking_find_match@@QAA_NPAUs_matchmaking_search_party_properties@@@Z
bool c_life_cycle_state_handler_matchmaking_find_match::get_search_party_properties(s_matchmaking_search_party_properties *properties)
{
    bool result = false;
    c_network_session *squad = m_manager->m_active_squad_session;
    s_network_session_matchmaking_composition composition;
    if (squad->is_host() &&
        network_session_build_matchmaking_composition((c_network_session *)squad, &composition) ==
            _network_session_matchmaking_composition_build_result_success) {
        long seconds = network_time_since(m_seeker.m_initial_search_start_time) / 1000;
        network_matchmaking_build_search_party_properties(&composition.session_properties, seconds, m_seeker.m_search_preference, properties);
        result = true;
    }
    return result;
}


static __forceinline bool life_cycle_handler_matchmaking_find_match_pending_qoses(const c_matchmaking_seeker *seeker)
{
    bool result = false;
    if (seeker->m_current_session_search_index >= 0) {
        const s_matchmaking_session_search *search = &seeker->m_session_searches[seeker->m_current_session_search_index];
        result = search->uncontacted > 0 || search->contacting > 0;
    }
    return result;
}
static __forceinline long life_cycle_handler_matchmaking_find_match_joinable_count(const c_matchmaking_seeker *seeker)
{
    long count = 0;
    if (seeker->m_current_session_search_index >= 0)
        count = seeker->m_session_searches[seeker->m_current_session_search_index].contacted;
    return count;
}
static __forceinline void life_cycle_handler_matchmaking_find_match_notify_join_failed(
    c_matchmaking_seeker *seeker, const s_transport_session_description *description, long reason)
{
    ++seeker->m_session_searches[seeker->m_current_session_search_index].join_failures[reason];
    network_session_tracker_mark_session_unsuitable(description, _session_unsuitable_matchmaking_rejected);
}
static __forceinline s_networking_join_data *life_cycle_handler_matchmaking_find_match_join_data()
{
    return &g_network_join_data;
}
static __forceinline bool life_cycle_handler_matchmaking_find_match_flag(unsigned char flags, long bit)
{
    return (flags & (1 << bit)) != 0;
}
static __forceinline void life_cycle_handler_matchmaking_find_match_begin_join(const s_transport_secure_identifier *secure_identifier,
    const s_transport_secure_address *host_address, const s_transport_secure_key *key, const s_matchmaking_search_party_properties *properties)
{
    s_networking_join_data *join = life_cycle_handler_matchmaking_find_match_join_data();
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
    join->request_join_squad_to_group = true;
    join->request_join_squad_to_target_group = false;
    join->request_join_group_to_group = false;
}

// @address 0x3e4ea0 ?update_joining@c_life_cycle_state_handler_matchmaking_find_match@@AAAXXZ
void c_life_cycle_state_handler_matchmaking_find_match::update_joining()
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *squad = manager->m_active_squad_session;
    s_network_session_matchmaking_composition composition;
    if (!(m_update_flags.m_storage & 1)) {
        network_session_build_matchmaking_composition((c_network_session *)squad, &composition);
        s_matchmaking_seeker_search_start_parameters parameters;
        memset(&parameters, 0, sizeof(parameters));
        parameters.search_preference = *squad->get_session_parameters()->m_parameters.matchmaking_search_preference.get();
        m_seeker.search_begin(&parameters, &composition.session_properties,
            manager->get_matchmaking_quality());
        m_update_flags.m_storage |= 1;
    }
    if (m_update_flags.m_storage & 1) {
        network_session_build_matchmaking_composition((c_network_session *)squad, &composition);
        if (memcmp(&m_seeker.m_session_properties, &composition.session_properties, sizeof(composition.session_properties)))
            m_seeker.set_session_properties(&composition.session_properties);
        m_seeker.update();
        long search_time = network_time_since(m_seeker.get_current_search_start_time());
        bool stale = search_time >= g_network_configuration.search_stale_timeout;
        bool can_join = !life_cycle_handler_matchmaking_find_match_pending_qoses(&m_seeker) ||
            life_cycle_handler_matchmaking_find_match_joinable_count(&m_seeker) >= g_network_configuration.minimum_joinable_sessions ||
            network_time_since(search_time) >= g_network_configuration.join_delay;
        if (stale && !(m_update_flags.m_storage & 2)) {
            if (m_seeker.m_flags.m_storage & 4)
                m_seeker.m_flags.m_storage &= ~4;
            m_seeker.m_flags.m_storage &= ~16;
            ((c_session_tracker *)&session_tracker_globals)->clear();
        }
        if (!life_cycle_handler_matchmaking_find_match_flag(m_update_flags.m_storage, 1) && !stale && can_join &&
            !network_join_is_join_to_squad_in_progress()) {
            if (m_seeker.get_session_to_join(&m_session)) {
                s_matchmaking_search_party_properties properties;
                memset(&properties, 0, sizeof(properties));
                if (!get_search_party_properties(&properties)) {
                    life_cycle_handler_matchmaking_find_match_notify_join_failed(&m_seeker, &m_session.description, 2);
                } else {
                    properties.latency = m_session.qos_status.ping_msec_median;
                    life_cycle_handler_matchmaking_find_match_begin_join(&m_session.description.secure_identifier,
                        &m_session.description.secure_address, &m_session.description.secure_key, &properties);
                    m_update_flags.m_storage |= 2;
                }
            }
        } else if (life_cycle_handler_matchmaking_find_match_flag(m_update_flags.m_storage, 1)) {
            s_networking_join_data *join = life_cycle_handler_matchmaking_find_match_join_data();
            long result = join->group_join_result;
            if (result != 1) {
                if (result != 0) {
                    life_cycle_handler_matchmaking_find_match_notify_join_failed(&m_seeker, &m_session.description, result);
                } else {
                    manager->m_matchmaking_quality.m_ping = m_session.qos_status.ping_msec_median;
                    squad->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_slave);
                }
                m_update_flags.m_storage &= ~2;
                join->group_join_result = 0;
            }
        }
    }
}
