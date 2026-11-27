// networkinglibxbox/life_cycle_handler_matchmaking_start.obj: from the records 0x3e3870 0x3e3a98 0x3e3d78 0x3e3f58 0x3e4320 0x3e4378 0x3e4440 0x3e4530 0x3e46b0 0x3e4780 0x3e4a70
#include "networking/session/network_game_simulation_protocol_types.h"
#include "game/multiplayer_game_hopper_globals.h"
#include "interface/user_interface_controller_globals.h"
#include "networking/logic/network_session_interface_globals.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/logic/network_banhammer_controller_types.h"
#include "interface/user_interface_controller.h"
#include "game/multiplayer_game_hopper.h"
#include "networking/logic/logic_qos_reply_manager.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/network_banhammer.h"
#include "networking/online/online_xenon.h"
#include "cseries/crt_xdk.h"
#include "networking/logic/logic_matchmaking_composition_types.h"
#include "networking/session/network_session_peer_properties_status_flags_types.h"
#include "networking/online/online_nat_types.h"
#include "networking/logic/life_cycle/peer_property_flag_test_types.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include <time.h>
#include "shell/shell.h"
#include <string.h>
#include <stdlib.h>
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_join_types.h"
#include "networking/transport/transport_security.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_managed_session.h"
#include "networking/session/network_session_types.h"
#include "interface/user_interface_error_manager.h"
#include "networking/session/network_session_parameters.h"
#include "game/hopper_configuration.h"
#include "networking/session/network_session_parameter_session_types.h"
#include "networking/logic/logic_matchmaking_seeker_search_types.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
#include "math/real_math_fraction_constants.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_entry_types.h"
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_start.h"
#include "networking/session/session_game_start_error_types.h"
#include "game/multiplayer_game_hopper_playability.h"
#include "networking/online/online_managed_session_types.h"
#include "networking/network_time_types.h"
#include "networking/network_configuration_types.h"
#include "networking/session/network_quality.h"
#include "networking/session/network_session_parameters_session.h"
#include "networking/network_configuration.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include "networking/logic/logic_matchmaking_composition_data_types.h"
#include "networking/logic/logic_matchmaking_composition.h"
#include "game/game_achievements_types.h"

// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x3e4320 ?handle_session_event@c_life_cycle_state_handler_matchmaking_start@@AAAXPBUs_matchmaking_start_event@@@Z
// @address 0x3e4440 ?determine_matchmaking_find_match_role@c_life_cycle_state_handler_matchmaking_start@@AAA?AW4e_life_cycle_matchmaking_find_match_role@@_N@Z
// @address 0x3e46b0 ?create_group_session@c_life_cycle_state_handler_matchmaking_start@@AAAXXZ
// @address 0x27298 k_achievement_name_ids
// @rdata 0x82027298..0x8202735c
extern "C" {
const long k_achievement_name_ids[k_number_of_achievements] = {
    0x00060000, 0x00060001, 0x00060002, 0x00060003, 0x00060004, 0x00060005, 0x00060006,
    0x00060007, 0x00060008, 0x00060009, 0x0006000A, 0x0006000B, 0x0006000C, 0x0006000D,
    0x0006000E, 0x0006000F, 0x00060010, 0x00060011, 0x00060012, 0x00060013, 0x00060014,
    0x00060015, 0x00060016, 0x00060017, 0x00060018, 0x00060019, 0x0006001A, 0x0006001B,
    0x0006001C, 0x0006001D, 0x0006001E, 0x0006001F, 0x00060020, 0x00060021, 0x00060022,
    0x00060023, 0x00060024, 0x00060025, 0x00060026, 0x00060027, 0x00060028, 0x00060029,
    0x0006002A, 0x0006002B, 0x0006002C, 0x0006002D, 0x0006002E, 0x0006002F, 0x00060030
};
}
// @rdata-end

enum e_life_cycle_matchmaking_find_match_role { _life_cycle_matchmaking_find_match_role_none = 1, _life_cycle_matchmaking_find_match_role_gather = 2 };

namespace {
static __forceinline const c_hopper_configuration *matchmaking_start_get_current_hopper_configuration()
{
    return g_multiplayer_game_hopper_globals.m_current_configuration;
}
static __forceinline float matchmaking_start_compute_gather_chance(long party_size)
{
    const c_hopper_configuration *configuration = matchmaking_start_get_current_hopper_configuration();
    float chance = 0.5f;
    if (configuration)
    {
        unsigned long party_index = party_size - 1;
        long index = party_index < 15 ? party_index : 15;
        chance = (float)configuration->m_configuration.chance_of_gathering[index] * k_real_hundredth_storage;
    }
    return chance;
}
}
namespace {
static __forceinline void network_join_local_squad_to_local_group()
{
    g_network_join_data.group_join_result = 1;
    memset(&g_network_join_data.queued_group_join, 0, sizeof(g_network_join_data.queued_group_join));
    g_network_join_data.request_join_squad_to_group = true;
    g_network_join_data.request_join_group_to_group = false;
    g_network_join_data.queued_group_join.session_class = _network_session_class_xbox_live;
}
}

void c_life_cycle_state_handler_matchmaking_start::get_progress(s_life_cycle_matchmaking_progress *progress_out)
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *squad_session = manager->m_active_squad_session;
    const c_network_session_parameters *squad_parameters = squad_session->get_session_parameters();
    if (squad_parameters->m_parameters.matchmaking_search_preference.get_allowed()
        && squad_parameters->m_parameters.matchmaking_progress_search_criteria.get_allowed()
        && squad_parameters->m_parameters.matchmaking_hopper_list.get_allowed()
        && squad_parameters->m_parameters.matchmaking_hopper.get_allowed()
        && (!squad_parameters->m_parameters.matchmaking_suppress_progress.get_allowed()
            || !*squad_parameters->m_parameters.matchmaking_suppress_progress.get()))
    {
        const e_matchmaking_search_preference *search_preference = squad_parameters->m_parameters.matchmaking_search_preference.get();
        const s_replicated_life_cycle_matchmaking_progress_search_criteria *search_criteria =
            squad_parameters->m_parameters.matchmaking_progress_search_criteria.get();
        progress_out->type = 1;
        progress_out->search_preference = *search_preference;
        progress_out->replicated_search_criteria = *search_criteria;
        progress_out->replicated_search_criteria.searching_skill_range_min = search_criteria->party_skill_range_min;
        progress_out->replicated_search_criteria.searching_skill_range_max = search_criteria->party_skill_range_max;
        memset(&progress_out->searching_for_match, 0, sizeof(progress_out->searching_for_match));
        const c_network_session_membership *membership = squad_session->get_session_membership();
        const s_network_session_matchmaking_hopper_list *hopper_list = squad_parameters->m_parameters.matchmaking_hopper_list.get();
        const s_network_session_matchmaking_hopper *hopper = squad_parameters->m_parameters.matchmaking_hopper.get();
        for (long hopper_index = 0; hopper_index < hopper_list->hopper_count; ++hopper_index)
        {
            if (hopper_list->hoppers[hopper_index].identifier == hopper->hopper_identifier)
            {
                ::utf8_string_to_wchar_string(
                    (utf8 const *)hopper_list->hoppers[hopper_index].name, (wchar_t *)(progress_out->hopper_name), 32, (long *)(0));
                break;
            }
        }
        progress_out->game_found = hopper->is_ranked;
        progress_out->teams = hopper->team_game;
        progress_out->group_player_count = 0;
        progress_out->squad_player_count = membership->get_player_count();
    }
}

void c_life_cycle_state_handler_matchmaking_start::handle_session_event(const s_matchmaking_start_event *event)
{
    g_network_session_interface_globals.peer.flags &= ~2;
    c_life_cycle_state_manager *manager = m_manager;
    if (event->type == 9)
    {
        manager->m_matchmaking_quality.m_event_9_pending = true;
    }
    else if (event->type == 10)
    {
        if (m_start_mode == 1)
            manager->m_matchmaking_quality.m_event_10_alternate_pending = true;
        else
            manager->m_matchmaking_quality.m_event_10_pending = true;
    }
}

bool c_life_cycle_state_handler_matchmaking_start::squad_allowed_to_start_matchmaking(bool log)
{
    bool success = true;
    c_network_session *squad_session = m_manager->m_active_squad_session;
    const c_network_session_parameters *parameters = squad_session->get_session_parameters();
    const c_network_session_membership *membership = squad_session->get_session_membership();
    const s_network_session_matchmaking_hopper_list *hopper_list = parameters->m_parameters.matchmaking_hopper_list.get();
    const s_network_session_matchmaking_hopper *hopper = parameters->m_parameters.matchmaking_hopper.get();
    if (hopper_list->hopper_load_status != 2)
    {
        success = false;
    }
    else
    {
        unsigned short player_error_mask = 0;
        e_session_game_start_error error = multiplayer_game_is_playable(hopper->hopper_identifier, true, true, membership, &player_error_mask);
        if (error != _session_game_start_error_none)
            success = false;
    }
    return success;
}
e_life_cycle_matchmaking_find_match_role c_life_cycle_state_handler_matchmaking_start::determine_matchmaking_find_match_role(bool force_gather)
{
    e_life_cycle_matchmaking_find_match_role role = _life_cycle_matchmaking_find_match_role_none;
    if (online_get_nat_type() != _online_nat_type_strict)
    {
        if (force_gather)
        {
            role = _life_cycle_matchmaking_find_match_role_gather;
        }
        else
        {
            c_life_cycle_state_manager *manager = m_manager;
            c_network_session *squad_session = manager->m_active_squad_session;
            const c_network_session_membership *membership = squad_session->get_session_membership();
            long player_count = membership->get_player_count();
            float random = _real_random_range(get_local_random_seed_address(), 0.0f, 1.0f);
            float gather_chance = matchmaking_start_compute_gather_chance(player_count);
            if (random <= gather_chance)
                role = _life_cycle_matchmaking_find_match_role_gather;
        }
    }
    return role;
}

void c_life_cycle_state_handler_matchmaking_start::create_group_session()
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *squad = manager->m_active_squad_session;
    c_network_session_parameters *parameters = squad->get_session_parameters();
    bool group_creation_allowed = true;
    if (!parameters->m_parameters.matchmaking_hopper.get_allowed())
        group_creation_allowed = false;
    if (!parameters->m_parameters.matchmaking_hopper_list.get_allowed())
        group_creation_allowed = false;
    if (!group_creation_allowed)
    {
        c_network_session_parameters *squad_parameters = squad->get_session_parameters();
        squad_parameters->m_parameters.session_mode.set(_network_session_mode_idle);
    }
    else
    {
        network_join_local_squad_to_local_group();
    }
}
// @address 0x3e4780 ?finish_group_session_creation@c_life_cycle_state_handler_matchmaking_start@@AAAXXZ

// @address 0x80608 GetTickCount
// @address 0x3a1298 ?transport_secure_random@@YAXJPAE@Z
// @address 0x95db8 _time64

void c_life_cycle_state_handler_matchmaking_start::finish_group_session_creation()
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *group_session = manager->m_group_session;
    c_network_session_parameters *group_parameters = group_session->get_session_parameters();
    const c_network_session_parameters *squad_parameters = manager->m_active_squad_session->get_session_parameters();
    group_parameters->m_parameters.session_mode.set(_network_session_mode_matchmaking_gathering);
    group_parameters->m_parameters.matchmaking_hopper.set(squad_parameters->m_parameters.matchmaking_hopper.get());
    group_parameters->m_parameters.matchmaking_hopper_list.set(squad_parameters->m_parameters.matchmaking_hopper_list.get());
    // match: 0x3e4780: the search-preference setter shares this campaign-difficulty COMDAT.
    ((c_generic_network_session_parameter<e_campaign_difficulty_level> *)&group_parameters->m_parameters.matchmaking_search_preference)
        ->set((const e_campaign_difficulty_level *)squad_parameters->m_parameters.matchmaking_search_preference.get());
    union
    {
        e_network_game_simulation_protocol simulation_protocol;
        unsigned __int64 nonce;
        unsigned long random_seed;
    } parameter_value;
    parameter_value.simulation_protocol = _network_game_simulation_protocol_distributed;
    // match: 0x3e4780: the simulation-protocol setter shares this campaign-difficulty COMDAT.
    ((c_generic_network_session_parameter<e_campaign_difficulty_level> *)&group_parameters->m_parameters.simulation_protocol)
        ->set((const e_campaign_difficulty_level *)&parameter_value.simulation_protocol);
    long max_player_count = g_multiplayer_game_hopper_globals.m_current_configuration->get_maximum_player_count();
    group_parameters->m_parameters.session_size.set_max_player_count(max_player_count);
    parameter_value.nonce = 0;
    transport_secure_random(8, (unsigned char *)&parameter_value.nonce);
    group_parameters->m_parameters.game_instance.set(&parameter_value.nonce);
    int tick_count = GetTickCount();
    unsigned int time_now = _time64(0);
    parameter_value.random_seed = (rand() ^ time_now) ^ tick_count;
    // match: 0x3e4780: the unsigned-long random-seed setter shares this campaign-difficulty COMDAT.
    ((c_generic_network_session_parameter<e_campaign_difficulty_level> *)&group_parameters->m_parameters.random_seed)
        ->set((const e_campaign_difficulty_level *)&parameter_value.random_seed);
    bool search_active = false;
    group_parameters->m_parameters.end_game.set(&search_active);
    long managed_index = group_session->managed_session_index();
    if (managed_index != -1)
        parameter_value.nonce = online_session_manager_globals.sessions[managed_index].actual.nonce;
    group_parameters->m_parameters.matchmaking_arbitration_nonce.set(&parameter_value.nonce);
}

// @address 0x3e4a70 ?handle_missing_required_session_parameter@c_life_cycle_state_handler_matchmaking_start@@UAAXW4e_network_session_type@@@Z
void c_life_cycle_state_handler_matchmaking_start::handle_missing_required_session_parameter(e_network_session_type session_type)
{
    this->squad_session_host_abort_matchmaking();
}

// @address 0x8c447c g_network_time_globals

static __forceinline long matchmaking_start_next_controller(long index)
{
    long result = -1;
    if ((unsigned long)index <= 2) result = index + 1;
    return result;
}
static __forceinline long matchmaking_start_network_time_get()
{
    return g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
}
static __forceinline long matchmaking_start_network_time_since(long time)
{
    return matchmaking_start_network_time_get() - time;
}
static __forceinline bool matchmaking_start_get_hopper_data(s_transport_qos_result *data)
{
    bool result = false;
    if (g_qos_reply_manager_globals.live_service_qos.flags.bits & 2)
    {
        *data = g_qos_reply_manager_globals.live_service_qos.result;
        result = true;
    }
    return result;
}
static __forceinline void matchmaking_start_clear_controller_flag(long index, unsigned char *flag)
{
    bool changed = *flag != 0;
    *flag = false;
    if (changed) user_interface_controller_update_network_properties((e_controller_index)index);
}
static __forceinline void matchmaking_start_set_repeated_play_coefficient(long index, long coefficient)
{
    s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[index];
    if (controller->flags & 1) {
        controller->repeated_play_index = coefficient;
        user_interface_controller_update_network_properties((e_controller_index)index);
    }
}
static __forceinline void matchmaking_start_clear_experience_growth_ban(long index)
{
    s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[index];
    if (controller->flags & 1) {
        controller->flags &= ~2;
        user_interface_controller_update_network_properties((e_controller_index)index);
    }
}
// @address 0x3e3d78 ?update_for_state_transition@c_life_cycle_state_handler_matchmaking_start@@UAA?AW4e_life_cycle_state_transition_type@@XZ
e_life_cycle_state_transition_type c_life_cycle_state_handler_matchmaking_start::update_for_state_transition()
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *squad = manager->m_active_squad_session;
    c_network_session_parameters *parameters = squad->get_session_parameters();
    e_life_cycle_state_transition_type result = _life_cycle_state_transition_none;
    switch (parameters->m_parameters.session_mode.get())
    {
    case _network_session_mode_matchmaking_start:
        result = _life_cycle_state_transition_continue;
        break;
    case _network_session_mode_matchmaking_slave:
        if (squad->is_host())
            parameters->m_parameters.session_mode.set(_network_session_mode_idle);
        else
        {
            manager->request_state_change(_life_cycle_state_matchmaking_find_match_client, 0, 0);
            result = _life_cycle_state_transition_change;
        }
        break;
    case _network_session_mode_matchmaking_searching:
        switch (m_search_count)
        {
        case 0:
            manager->request_state_change(_life_cycle_state_matchmaking_find_match_client, 0, 0);
            result = _life_cycle_state_transition_change;
            break;
        case 1:
            manager->request_state_change(_life_cycle_state_matchmaking_find_match, 0, 0);
            result = _life_cycle_state_transition_change;
            break;
        default:
        {
            s_find_and_assemble_data request = {};
            if (m_start_mode == 1)
            {
                request.force_desperation = true;
                request.initial_sessions_found_count = m_entry_data.initial_desperation_sessions_found_count;
                request.initial_session_search_count = m_entry_data.unused;
            }
            manager->request_state_change(_life_cycle_state_matchmaking_find_and_assemble_match, sizeof(request), &request);
            result = _life_cycle_state_transition_change;
            break;
        }
        }
        break;
    }
    return result;
}
// @address 0x3e3f58 ?enter@c_life_cycle_state_handler_matchmaking_start@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_matchmaking_start::enter(c_life_cycle_state_handler *from, long size, void *data)
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *squad = manager->m_active_squad_session;
    c_life_cycle_state_handler::enter(from, 0, 0);
    m_flags = 0;
    m_desperation_search_count = 0;
    m_entry_data = *(const s_matchmaking_start_data *)data;
    m_search_count = 0;
    if (squad->is_host()) {
        c_network_session_parameters *parameters = squad->get_session_parameters();
        if (m_entry_data.initial_session_search_count == 0) {
            bool from_game = false;
            if ((unsigned long)(from->get_state() - 8) <= 6) from_game = true;
            if (from_game)
                network_session_request_parameter_set_value(
                    &parameters->m_parameters.matchmaking_messaging, 2);
        }
        if (!parameters->m_parameters.matchmaking_search_preference.get_allowed()) {
            e_matchmaking_search_preference preference = _matchmaking_search_preference_none;
            // match: 0x3e3f58: the search-preference setter shares this campaign-difficulty COMDAT.
            ((c_generic_network_session_parameter<e_campaign_difficulty_level> *)&parameters->m_parameters.matchmaking_search_preference)
                ->set((const e_campaign_difficulty_level *)&preference);
        }
        parameters->clear_parameters_with_mask(3ui64 << 35);
    }
    c_network_session *group = manager->m_group_session;
    group->leave_session();
    c_network_session *target = manager->m_target_squad_session;
    if (target->m_local_state != 0) {
        target->m_disconnect_reestablish_state = 1;
        target->disconnect();
    }
    c_matchmaking_quality *quality = manager->get_matchmaking_quality();
    quality->initialize();
    s_transport_qos_result hopper_data;
    if (matchmaking_start_get_hopper_data(&hopper_data)) {
        quality->hopper_data_valid = true;
        quality->hopper_data = hopper_data;
    }
    long nat_type = online_get_nat_type();
    quality->nat_type_valid = true;
    quality->nat_type = nat_type;
    g_network_session_interface_globals.peer.flags = 0;
    for (long index = 0; index != -1; index = matchmaking_start_next_controller(index)) {
        s_user_interface_controller *controller = &g_user_interface_controller_globals.controllers[index];
        matchmaking_start_clear_controller_flag(index, &controller->desires_veto);
        matchmaking_start_clear_controller_flag(index, &controller->desires_rematch);
        user_interface_controller_update_network_properties((e_controller_index)index);
    }
    for (long index = 0; index != -1; index = matchmaking_start_next_controller(index)) {
        matchmaking_start_set_repeated_play_coefficient(index, -1);
        matchmaking_start_clear_experience_growth_ban(index);
    }
    long last_warning_time = m_last_nat_warning_time;
    if (last_warning_time == -1 ||
        matchmaking_start_network_time_get() - last_warning_time >=
            g_network_configuration.nat_warning_interval_seconds * 1000)
    {
        s_network_quality_data network_quality;
        long nat, value_a, value_b;
        if (network_session_interface_get_network_quality(&network_quality, &nat, &value_a, &value_b) && nat != 1) {
            g_gui_error_manager.post_error_with_custom_message(0x20081, 0, 0, k_any_controller, false, false);
            m_last_nat_warning_time = matchmaking_start_network_time_get();
        }
    }
    network_group_session_set_disable_host_election(3);
    if (g_network_join_data.join_queue_mode != _network_join_queue_closed_while_in_matchmaking) network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_queue_closed_while_in_matchmaking;
}

static __forceinline bool matchmaking_start_test_flag(const unsigned char &flags, long bit)
{
    unsigned char result = (flags >> bit) & 1;
    return result;
}
static __forceinline void matchmaking_start_set_flag(unsigned char *flags, long bit, bool enabled)
{
    if (enabled)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}
static __forceinline bool matchmaking_start_composition_valid(const c_network_session *session)
{
    s_network_session_matchmaking_composition composition;
    bool valid = false;
    if (network_session_build_matchmaking_composition(session, &composition) == _network_session_matchmaking_composition_build_result_success)
        valid = true;
    return valid;
}
// @address 0x3e4530 ?update_group_creation@c_life_cycle_state_handler_matchmaking_start@@AAAXXZ
void c_life_cycle_state_handler_matchmaking_start::update_group_creation()
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *squad = manager->m_active_squad_session;
    if (!matchmaking_start_test_flag(m_flags, 4))
    {
        g_network_join_data.group_join_result = 0;
        create_group_session();
        m_flags |= 16;
    }
    if (matchmaking_start_test_flag(m_flags, 4))
    {
        if (!matchmaking_start_test_flag(m_flags, 5))
        {
            long result = g_network_join_data.group_join_result;
            if (result != 1)
            {
                unsigned char success = result == 0;
                if (success)
                    matchmaking_start_set_flag(&m_flags, 5, true);
                else
                {
                    c_network_session_parameters *parameters = squad->get_session_parameters();
                    parameters->m_parameters.session_mode.set(_network_session_mode_idle);
                }
                g_network_join_data.group_join_result = 0;
            }
        }
        if (matchmaking_start_test_flag(m_flags, 5))
        {
            c_network_session *group = manager->m_group_session;
            c_network_session_parameters *parameters = squad->get_session_parameters();
            if (!group->established())
                parameters->m_parameters.session_mode.set(_network_session_mode_idle);
            else if (group->get_session_membership()->all_peers_established())
            {
                if (!all_squad_peers_established_in_group())
                    parameters->m_parameters.session_mode.set(_network_session_mode_idle);
                else
                {
                    finish_group_session_creation();
                    if (!matchmaking_start_composition_valid(group))
                        parameters->m_parameters.session_mode.set(_network_session_mode_idle);
                    else
                        parameters->m_parameters.session_mode.set(_network_session_mode_matchmaking_searching);
                }
            }
        }
    }
}

__forceinline void c_life_cycle_state_handler_matchmaking_start::update_squad_ready()
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *squad_session = manager->m_active_squad_session;
    const c_network_session_membership *squad_membership = squad_session->get_session_membership();
    if (squad_membership->peer_property_flag_test(_peer_property_flag_test_all, _network_session_peer_properties_status_squad_ready_bit))
        matchmaking_start_set_flag(&m_flags, 1, true);
}

// @address 0x3e3a98 ?update@c_life_cycle_state_handler_matchmaking_start@@UAAXXZ
void c_life_cycle_state_handler_matchmaking_start::update()
{
    c_life_cycle_state_manager *manager = m_manager;
    c_network_session *squad_session = manager->m_active_squad_session;
    const c_network_session_membership *squad_membership = squad_session->get_session_membership();
    matchmaking_update_required_files_for_hopper();
    if (!matchmaking_start_test_flag(m_flags, 0))
    {
        c_network_session *group_session = manager->m_group_session;
        if (group_session->m_local_state == 0)
        {
            g_network_session_interface_globals.peer.flags |= 2;
            matchmaking_start_set_flag(&m_flags, 0, true);
        }
        else if (!group_session->leaving_session())
        {
            group_session->leave_session();
        }
    }
    if (squad_session->is_host() && squad_membership->all_peers_established())
    {
        c_network_session_parameters *squad_parameters = squad_session->get_session_parameters();
        if (!matchmaking_start_composition_valid(squad_session))
        {
            squad_parameters->m_parameters.session_mode.set(_network_session_mode_idle);
        }
        else if (squad_membership->peer_property_flag_test_any_peer(_network_session_peer_properties_status_leader_idle_controller_bit))
        {
            squad_parameters->m_parameters.session_mode.set(_network_session_mode_idle);
        }
        else
        {
            if (!matchmaking_start_test_flag(m_flags, 2))
            {
                if (!squad_allowed_to_start_matchmaking(false))
                {
                    if (!matchmaking_start_test_flag(m_flags, 3))
                    {
                        matchmaking_start_set_flag(&m_flags, 3, true);
                        m_desperation_search_count = matchmaking_start_network_time_get();
                    }
                    if (matchmaking_start_network_time_since(m_desperation_search_count) >=
                        g_network_configuration.squad_start_failure_give_up_time_msec)
                    {
                        squad_allowed_to_start_matchmaking(true);
                        squad_parameters->m_parameters.session_mode.set(_network_session_mode_idle);
                    }
                }
                else
                {
                    update_matchmaking_search_criteria(false);
                    matchmaking_start_set_flag(&m_flags, 2, true);
                }
            }
            if (!matchmaking_start_test_flag(m_flags, 1))
            {
                update_squad_ready();
            }
            if (matchmaking_start_test_flag(m_flags, 2) && matchmaking_start_test_flag(m_flags, 1))
            {
                if (m_search_count == 0)
                {
                    if (session_has_minimum_player_count_to_start_game_in_hopper(squad_session))
                    {
                        m_search_count = _life_cycle_matchmaking_find_match_role_gather;
                    }
                    else
                    {
                        switch (m_start_mode)
                        {
                        case 0:
                            m_search_count = determine_matchmaking_find_match_role(false);
                            break;
                        case 1:
                            m_search_count = determine_matchmaking_find_match_role(true);
                            break;
                        default:
                            // match: 0x3e3a98: unreachable hint omits dead branch check
                            __assume(0);
                        }
                    }
                }
                switch (m_search_count)
                {
                case 0:
                    squad_parameters->m_parameters.session_mode.set(_network_session_mode_idle);
                    break;
                case 1:
                    squad_parameters->m_parameters.session_mode.set(_network_session_mode_matchmaking_searching);
                    break;
                case 2:
                    update_group_creation();
                    break;
                default:
                    // match: 0x3e3a98: unreachable hint omits dead branch check
                    __assume(0);
                }
            }
        }
    }
}
