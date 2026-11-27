// networkinglibxbox/life_cycle_manager.obj: from the records 0x93650 0x93780 0x93a80 0x93e28 0x93f18 0x94020 0x3cc8b0 0x3dd2b0 0x3dd2e8 0x3dd348 0x3dd3a8 0x3dd3f8 0x3dd420 0x3dd4a8 0x3dd538 0x3dd748 0x3dd7a0 0x3ddaa0 0x3ddc18 0x3ddc90 0x3ddec0 0x3de0c0 0x3de1e0 0x3de348 0x3de3f0 0x3de560 0x3de630 0x3de720 0x3de7c0 0x3de8a0 0x3dedd0 0x3deeb8 0x3ebd18
// @compiler xdk6130
// @flags /TP /O2 /Oc
#define NOMINMAX // keep this unit's max/min below, not windef.h's
#include <xtl.h> // XUSER_DATA_TYPE_* for k_online_property_data_type_map
#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"
#include "game/game_globals_storage.h"
#include "networking/session/network_session_parameter_initial_participants_types.h"
#include "networking/session/network_session_parameter_variant_types.h"
#include "networking/logic/logic_matchmaking_seeker.h"
#include "networking/logic/logic_matchmaking_composition_data_types.h"
#include "networking/network_team_picker_player_types.h"
#include "networking/logic/network_join_types.h"
#include "cseries/crt_xdk.h"
#include "networking/logic/logic_matchmaking_composition_types.h"
#include "networking/session/network_game_countdown_delayed_reason_types.h"
#include "networking/transport/transport_security_types.h"
#include "memory/static_arrays.h"
#include "game/game_variant.h"
#include "cseries/cseries_types.h"
#include "networking/logic/logic_matchmaking_desirability.h"
#include <string.h>
#include "multithreading/synchronization.h"
#include "simulation/simulation.h"
#include "text/unicode.h"
#include "game/multiplayer_game_hopper.h"
#include "game/multiplayer_game_hopper_globals.h"
#include "networking/online/online_xenon.h"
#include "networking/session/network_managed_session.h"
#include "networking/transport/transport_security.h"
#include "text/font_cache.h"
#include "networking/session/network_session.h"
#include "main/loading.h"
#include "networking/logic/network_join.h"
#include "networking/network_team_picker.h"
#include "networking/session/network_session_types.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"
#include "networking/session/network_session_class.h"
#include "shell/shell.h"
#include "game/game_options.h"
#include "interface/user_interface_error_manager.h"
#include "networking/online_property_id_types.h"
#include "networking/life_cycle_session_address_state_types.h"
#include "networking/life_cycle_qos_request_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_matchmaking_assemble_match.h"
#include "networking/logic/logic_managed_user.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/transport/transport_platform_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/logic_managed_user_types.h"
#include "networking/logic/logic_matchmaking_seeker_search_types.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"
#include "networking/session/network_session_parameters_base.h"
#include "game/hopper_configuration.h"
#include "networking/session/network_session_parameters.h"
#include "interface/gui_game_mode_types.h"
#include "networking/session/network_session_parameters_generic_set.h"
#include "networking/online/online_constants.h"
#include "networking/logic/life_cycle_manager_services.h"
#include "networking/network_time_types.h"
#include "main/levels_globals.h"
#include "memory/tls_globals.h"
#include "networking/logic/logic_matchmaking_composition.h"
#include "networking/session/session_game_start_error_types.h"
#include "networking/logic/network_session_interface_globals.h"
#include "game/multiplayer_game_hopper.h" // multiplayer_game_hopper_check_required_files
enum e_matchmaking_search_stage { _matchmaking_search_stage_none };
// @address 0x93f18 ?current_state_ready_for_state_transition_or_query@c_life_cycle_state_manager@@QBA_NXZ
// @address 0x3cc8b0 ?all_peers_established@c_network_session_membership@@QBA_NXZ
// @address 0x3dd2e8 ?get_minimum_player_count@c_hopper_configuration@@QBAJXZ
// @address 0x3dd348 ?get_maximum_player_count@c_hopper_configuration@@QBAJXZ
// @address 0x3ddaa0 ?build_player_options@c_life_cycle_state_handler@@KAXPBUs_network_session_player@@KPBUs_machine_identifier@@PBVc_game_variant@@JJ_NJPAUgame_player_options@@@Z
// @address 0x3de0c0 ?update_search_progress@c_life_cycle_state_handler@@IAAXPBVc_matchmaking_seeker@@_NK@Z
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

// Values are the 59 longs at 0x8273a6b0..0x8273a79b. Each entry is an XUSER_DATA_TYPE_* code: the map feeds
// XUSER_DATA.type directly (network_leaderboard_build_property_from_statistic_and_results writes it into XUSER_PROPERTY).
// @address 0x73a6b0 k_online_property_data_type_map
e_online_data_type k_online_property_data_type_map[] =
{
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_UNICODE,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_DOUBLE,
    (e_online_data_type)XUSER_DATA_TYPE_DOUBLE, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_FLOAT, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_FLOAT, (e_online_data_type)XUSER_DATA_TYPE_FLOAT, (e_online_data_type)XUSER_DATA_TYPE_INT32,
    (e_online_data_type)XUSER_DATA_TYPE_INT32, (e_online_data_type)XUSER_DATA_TYPE_INT64
};


struct s_transport_session_description;
enum e_transport_platform;
struct utf8;
struct game_machine_options;
struct game_player_options;



static __forceinline bool inl_test_flag(unsigned char flags, int bit)
{
    unsigned char result = (flags >> bit) & 1;
    if (result)
        return true;
    return false;
}
static __forceinline bool life_cycle_manager_states_established(const c_network_session *session)
{
    unsigned char result = (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
    return result;
}


class c_network_session;
class c_matchmaking_seeker;
class c_life_cycle_state_handler;
struct s_life_cycle_matchmaking_progress;
struct s_matchmaking_session_properties;
struct s_network_session_player;
struct s_machine_identifier;
struct game_player_options;


// @address 0x93650 ?update@c_life_cycle_state_manager@@QAAXXZ
void c_life_cycle_state_manager::update()
{
    life_cycle_manager_handle_game_end_errors(this);
    if (m_matchmaking_quality.m_needs_submission)
        get_matchmaking_quality()->submit_to_webstats();
    if (m_pending_state_change)
    {
        m_pending_state_change = false;
        set_current_state(m_pending_state, m_entry_data_size, m_entry_data_size > 0 ? m_entry_data : 0);
    }
    if (!m_pending_state_change)
        update_handle_session_states();
    if (!m_pending_state_change)
        update_handle_session_parameters();
    if (!m_pending_state_change && current_state_ready_for_state_transition_or_query())
    {
        c_life_cycle_state_handler *handler = get_current_state_handler();
        e_life_cycle_state_transition_type transition = handler->update_for_state_transition();
        if ((unsigned long)transition >= 1)
        {
            if ((unsigned long)transition == _life_cycle_state_transition_continue && current_state_ready_for_update())
                handler->update();
        }
        else
        {
            set_current_state(_life_cycle_state_none, 0, 0);
        }
    }
}
/* True when every valid peer of the session membership is in connection state 8. */


struct s_game_context;





// @address 0x93e28 ?update_handle_session_parameters@c_life_cycle_state_manager@@AAAXXZ
void c_life_cycle_state_manager::update_handle_session_parameters()
{
    e_life_cycle_state current_state = get_current_state();
    bool in_matchmaking = false;
    if (current_state >= _life_cycle_state_matchmaking_start && current_state <= _life_cycle_state_matchmaking_assemble_match)
        in_matchmaking = true;
    if (in_matchmaking)
    {
        c_network_session *squad_session = get_active_squad_session();
        if (squad_session->is_host())
        {
            c_network_session_parameters *parameters = squad_session->get_session_parameters();
            bool abort_requested = false;
            if (parameters->m_parameters.matchmaking_abort_requested.get_allowed())
                abort_requested = *parameters->m_parameters.matchmaking_abort_requested.get();
            if (abort_requested)
            {
                bool clear_abort = false;
                parameters->m_parameters.matchmaking_abort_requested.set(&clear_abort);
                parameters->m_parameters.countdown_timer.set(false, 0, _network_game_countdown_delayed_reason_none, 0);
                parameters->m_parameters.session_mode.set(_network_session_mode_idle);
            }
        }
    }
}

bool c_life_cycle_state_manager::current_state_ready_for_state_transition_or_query() const
{
    
    bool ready = true;
    unsigned char flags = get_current_state_handler()->get_handler_flags();
    if (inl_test_flag(flags, 0))
    {
        c_network_session *session = m_active_squad_session;
        const c_network_session_parameter_base *parameters = &session->get_session_parameters()->m_parameters.session_mode;
        ready = ready && life_cycle_manager_states_established(session);
        ready = ready && parameters->get_allowed();
    }
    if (inl_test_flag(flags, 1))
    {
        c_network_session *session = m_group_session;
        const c_network_session_parameter_base *parameters = &session->get_session_parameters()->m_parameters.session_mode;
        ready = ready && life_cycle_manager_states_established(session);
        ready = ready && parameters->get_allowed();
    }
    return ready;
}

// @address 0x94020 ?current_state_ready_for_update@c_life_cycle_state_manager@@QBA_NXZ
bool c_life_cycle_state_manager::current_state_ready_for_update() const
{
    bool ready = current_state_ready_for_state_transition_or_query();
    if (ready)
    {
        c_life_cycle_state_handler *handler = get_current_state_handler();
        if (handler->test_flag(0))
        {
            c_network_session *squad_session = get_active_squad_session();
            const c_network_session_parameters *parameters = squad_session->get_session_parameters();
            if (!parameters->parameters_valid(handler->get_required_squad_session_parameter_mask()))
            {
                if (squad_session->is_host())
                    handler->handle_missing_required_session_parameter(_network_session_type_squad);
                ready = false;
            }
        }
        if (handler->test_flag(1))
        {
            c_network_session *group_session = get_group_session();
            const c_network_session_parameters *parameters = group_session->get_session_parameters();
            if (!parameters->parameters_valid(handler->get_required_group_session_parameter_mask()))
            {
                if (group_session->is_host())
                    handler->handle_missing_required_session_parameter(_network_session_type_group);
                ready = false;
            }
        }
    }
    return ready;
}

bool c_network_session_membership::all_peers_established() const
{
  const c_network_session_membership *membership = this;
  bool result = true;
  for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
       peer_index = (membership)->get_next_peer(peer_index)) {
    if (membership->m_current_membership.peers[peer_index].connection_state != 8) {
      result = false;
      break;
    }
  }
  return result;
}

#if defined(_XBOX) && !defined(__clang__)
inline long c_hopper_configuration::get_maximum_team_imbalance() const
#else
long c_hopper_configuration::get_maximum_team_imbalance() const
#endif
{
    long result = 0;
    switch (get_hopper_type())
    {
    case 2:
        if (m_configuration.team_configuration.configuration.unranked.allow_uneven_teams)
            result = 15;
        break;
    case 3:
        result = m_configuration.team_configuration.configuration.ranked.maximum_team_imbalance;
        break;
    }
    return result;
}

#if defined(_XBOX) && !defined(__clang__)
inline long c_hopper_configuration::get_maximum_player_count() const
#else
long c_hopper_configuration::get_maximum_player_count() const
#endif
{
#if defined(_XBOX) && !defined(__clang__)
  // match: 0x43e648: the address anchors emit the get_maximum_team_imbalance, get_team_count and get_maximum_team_size COMDATs that build_party_masks calls
  (void)&c_hopper_configuration::get_maximum_team_imbalance;
  (void)&c_hopper_configuration::get_team_count;
  (void)&c_hopper_configuration::get_maximum_team_size;
#endif
  int result = 0;
  switch (m_configuration.hopper_type) {
  case 0:
  case 1:
    result = m_configuration.team_configuration.minimum_team_size;
    break;
  case 2:
  case 3:
    result = m_configuration.team_configuration.maximum_team_size * m_configuration.team_configuration.team_count;
    break;
  }
  if (result >= 16)
    result = 16;
  return result;
}

#if defined(_XBOX) && !defined(__clang__)
inline long c_hopper_configuration::get_team_count() const
#else
long c_hopper_configuration::get_team_count() const
#endif
{
    long result = 0;
    switch (get_hopper_type())
    {
    case 0:
    case 1:
        result = m_configuration.team_configuration.minimum_team_size;
        break;
    case 2:
        result = m_configuration.team_configuration.team_count;
        break;
    case 3:
        result = m_configuration.team_configuration.team_count;
        break;
    }
    return result;
}

#if defined(_XBOX) && !defined(__clang__)
inline long c_hopper_configuration::get_maximum_team_size() const
#else
long c_hopper_configuration::get_maximum_team_size() const
#endif
{
    long result = 1;
    switch (get_hopper_type())
    {
    case 2:
        result = m_configuration.team_configuration.maximum_team_size;
        break;
    case 3:
        result = m_configuration.team_configuration.maximum_team_size;
        break;
    }
    return result;
}

void c_life_cycle_state_manager::set_current_state(e_life_cycle_state state, long entry_data_size, void *entry_data)
{
    if (m_current_state != state)
    {
        c_life_cycle_state_handler *from_handler = m_handlers[get_current_state()];
        c_life_cycle_state_handler *to_handler = m_handlers[state];
        from_handler->exit(to_handler);
        m_current_state = state;
        to_handler->enter(from_handler, entry_data_size, entry_data);
    }
}

static __forceinline unsigned char life_cycle_manager_online_data_type(long property_id)
{
    return (unsigned char)k_online_property_data_type_map[property_id];
}
static __forceinline void life_cycle_manager_set_integer_property(e_controller_index controller_index, long property_id, long value)
{
    s_online_data data;
    data.type = life_cycle_manager_online_data_type(property_id);
    data.value.integer = value;
    managed_user_set_property(controller_index, (e_online_property_id)property_id, &data);
}
static __forceinline void life_cycle_manager_set_real_property(e_controller_index controller_index, long property_id, float value)
{
    s_online_data data;
    data.type = life_cycle_manager_online_data_type(property_id);
    data.value.real = value;
    managed_user_set_property(controller_index, (e_online_property_id)property_id, &data);
}

// @address 0x3dd538 ?update_advertised_session@c_life_cycle_state_handler@@KAXPBUs_matchmaking_session_properties@@@Z
void c_life_cycle_state_handler::update_advertised_session(const s_matchmaking_session_properties *session_properties)
{
    e_controller_index controller_index = user_interface_controller_get_first_xbox_live_signed_in_non_guest();
    if (controller_index == k_no_controller)
        controller_index = _controller_index0;
    life_cycle_manager_set_integer_property(controller_index, 28, session_properties->hopper_identifier);
    life_cycle_manager_set_integer_property(controller_index, 29, session_properties->maximum_skill_level);
    life_cycle_manager_set_integer_property(controller_index, 30, session_properties->minimum_skill_level);
    life_cycle_manager_set_integer_property(controller_index, 31, session_properties->average_skill_level);
    life_cycle_manager_set_integer_property(controller_index, 32, session_properties->average_experience);
    life_cycle_manager_set_integer_property(controller_index, 34, (session_properties->composition_flags >> 0) & 1);
    life_cycle_manager_set_integer_property(controller_index, 35, session_properties->region);
    life_cycle_manager_set_integer_property(controller_index, 36, session_properties->language);
    life_cycle_manager_set_integer_property(controller_index, 37, session_properties->country);
    life_cycle_manager_set_integer_property(controller_index, 38, (session_properties->composition_flags >> 2) & 1);
    for (long i = 0; i < 9; i++)
        life_cycle_manager_set_integer_property(controller_index, 39 + i, (session_properties->acceptable_party_size_mask & (1 << i)) != 0);
    life_cycle_manager_set_integer_property(controller_index, 48, session_properties->worst_nat_type);
    life_cycle_manager_set_real_property(controller_index, 49, session_properties->average_mu);
}

// @address 0x3dd748 ?squad_session_host_abort_matchmaking@c_life_cycle_state_handler@@IAAXXZ
void c_life_cycle_state_handler::squad_session_host_abort_matchmaking()
{
    c_network_session *squad_session = get_manager()->get_active_squad_session();
    if (squad_session->is_host())
    {
        c_network_session_parameters *parameters = squad_session->get_session_parameters();
        bool abort_requested = true;
        parameters->m_parameters.matchmaking_abort_requested.request_change(&abort_requested);
    }
}

// @address 0x3dd4a8 ?enter@c_life_cycle_state_handler@@UAAXPAV1@JPAX@Z
void c_life_cycle_state_handler::enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data)
{
    if (!test_flag(1) && !test_flag(2))
    {
        c_network_session *group_session = get_manager()->get_group_session();
        if (!group_session->disconnected() && !group_session->leaving_session())
        {
            group_session->leave_session();
        }
    }
}

void c_life_cycle_state_handler::exit(c_life_cycle_state_handler *to_handler)
{
}

// @address 0x5bec98 ?handle_missing_required_session_parameter@c_life_cycle_state_handler@@UAAXW4e_network_session_type@@@Z
void c_life_cycle_state_handler::handle_missing_required_session_parameter(e_network_session_type session_type)
{
}

void c_life_cycle_state_handler::build_player_options(const s_network_session_player *player, unsigned long machine_valid_mask,
    const s_machine_identifier *machines, const c_game_variant *variant, long unused, long player_index, bool adjust_skill, long quality,
    game_player_options *options)
{
    options->valid = true;
    options->left_game = false;
    options->machine_identifier = machines[player->peer_index];
    options->user_index = (short)player->peer_user_index;
    options->controller_index = player->controller_index;
    memcpy(&options->player_identifier, &player->identifier, sizeof(options->player_identifier));
    memcpy(&options->configuration, &player->configuration, sizeof(options->configuration));
    if (adjust_skill)
    {
        s_player_matchmaking_properties *properties = &options->configuration.host.matchmaking;
        properties->hopper_weight = multiplayer_game_hopper_compute_quality_adjusted_skill_update_weight(properties->hopper_weight, quality);
        properties->hopper_weight = min(max(properties->hopper_weight * player->configuration.client.repeated_play_coefficient / 100, 1), 100);
    }
    if (!variant) return;
    if (variant->has_teams())
        options->configuration.host.team_index = min(max(options->configuration.host.team_index, 0), 7);
    else
        options->configuration.host.team_index = player_index;
}

// @address 0x3ddc18 ?session_has_minimum_player_count_to_start_game_in_hopper@c_life_cycle_state_handler@@KA_NPBVc_network_session@@@Z
bool c_life_cycle_state_handler::session_has_minimum_player_count_to_start_game_in_hopper(const c_network_session *session)
{
    bool result = false;
    const c_hopper_configuration *hopper_configuration = multiplayer_game_hoppers_get_current_hopper_configuration();
    const c_network_session_membership *membership = session->get_session_membership();
    result = !hopper_configuration->is_ranked() && membership->get_player_count() >= hopper_configuration->get_minimum_player_count();
    return result;
}

void c_life_cycle_state_handler::get_matchmaking_progress_for_squad_search(s_life_cycle_matchmaking_progress *progress_out)
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad_session = manager->get_active_squad_session();
    const c_network_session_parameters *squad_parameters = squad_session->get_session_parameters();
    if (squad_parameters->m_parameters.matchmaking_search_preference.get_allowed()
        && squad_parameters->m_parameters.matchmaking_progress.get_allowed()
        && squad_parameters->m_parameters.matchmaking_progress_search_criteria.get_allowed()
        && squad_parameters->m_parameters.matchmaking_hopper_list.get_allowed()
        && squad_parameters->m_parameters.matchmaking_hopper.get_allowed())
    {
        const s_replicated_life_cycle_matchmaking_progress *matchmaking_progress = squad_parameters->m_parameters.matchmaking_progress.get();
        if (matchmaking_progress->type == 1)
        {
            const e_matchmaking_search_preference *search_preference = squad_parameters->m_parameters.matchmaking_search_preference.get();
            const s_replicated_life_cycle_matchmaking_progress_search_criteria *search_criteria =
                squad_parameters->m_parameters.matchmaking_progress_search_criteria.get();
            progress_out->type = 1;
            progress_out->search_preference = *search_preference;
            progress_out->searching_for_match = matchmaking_progress->searching_for_match;
            progress_out->replicated_search_criteria = *search_criteria;
            progress_out->replicated_search_criteria.searching_skill_range_min = matchmaking_progress->searching_for_match.searching_skill_min;
            progress_out->replicated_search_criteria.searching_skill_range_max = matchmaking_progress->searching_for_match.searching_skill_max;
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
}

void c_life_cycle_state_handler::get_matchmaking_progress_for_setting_up_match(s_life_cycle_matchmaking_progress *progress_out)
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad_session = manager->get_active_squad_session();
    c_network_session *group_session = manager->get_group_session();
    const c_network_session_parameters *squad_parameters = squad_session->get_session_parameters();
    const c_network_session_parameters *group_parameters = group_session->get_session_parameters();
    if (group_parameters->m_parameters.matchmaking_progress.get_allowed()
        && squad_parameters->m_parameters.matchmaking_hopper_list.get_allowed()
        && squad_parameters->m_parameters.matchmaking_hopper.get_allowed()
        && squad_parameters->m_parameters.matchmaking_search_preference.get_allowed()
        && squad_parameters->m_parameters.matchmaking_progress_search_criteria.get_allowed())
    {
        const s_replicated_life_cycle_matchmaking_progress *matchmaking_progress = group_parameters->m_parameters.matchmaking_progress.get();
        if (matchmaking_progress->type == 3)
        {
            const e_matchmaking_search_preference *search_preference = squad_parameters->m_parameters.matchmaking_search_preference.get();
            const s_replicated_life_cycle_matchmaking_progress_search_criteria *search_criteria =
                squad_parameters->m_parameters.matchmaking_progress_search_criteria.get();
            progress_out->type = 3;
            progress_out->replicated_search_criteria = *search_criteria;
            progress_out->search_preference = *search_preference;
            const c_network_session_membership *group_membership = group_session->get_session_membership();
            const c_network_session_membership *squad_membership = squad_session->get_session_membership();
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
            progress_out->squad_player_count = squad_membership->get_player_count();
            progress_out->group_player_count = group_membership->get_player_count();
        }
    }
}

void c_life_cycle_state_handler::update_search_progress(const c_matchmaking_seeker *matchmaking_seeker, bool unused, unsigned long time)
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad_session = manager->get_active_squad_session();
    if (matchmaking_seeker->search_in_progress())
    {
        c_network_session_parameters *squad_parameters = squad_session->get_session_parameters();
        s_replicated_life_cycle_matchmaking_progress progress;
        memset(&progress, 0, sizeof(progress));
        progress.type = 1;
        progress.searching_for_match.contacted_session_count = matchmaking_seeker->get_total_sessions_contacted_count();
        progress.searching_for_match.potential_session_count = matchmaking_seeker->get_current_sessions_joinable_count();
        progress.searching_for_match.search_stage = matchmaking_seeker->get_session_search_stage();
        progress.searching_for_match.search_queries_submitted = matchmaking_seeker->get_total_session_search_count() % 63;
        matchmaking_seeker->get_current_skill_range(&progress.searching_for_match.searching_skill_min, &progress.searching_for_match.searching_skill_max);
        squad_parameters->m_parameters.matchmaking_progress.set(&progress);
        bool search_active = false;
        squad_parameters->m_parameters.matchmaking_suppress_progress.set(&search_active);
    }
}

static __forceinline bool life_cycle_manager_session_established(const c_network_session *session)
{
    return (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
}

// @address 0x3de1e0 ?all_squad_peers_established_in_group@c_life_cycle_state_handler@@IAA_NXZ
bool c_life_cycle_state_handler::all_squad_peers_established_in_group()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad_session = manager->get_active_squad_session();
    const c_network_session_membership *squad_membership = squad_session->get_session_membership();
    c_network_session *group_session = manager->get_group_session();
    const c_network_session_membership *group_membership = group_session->get_session_membership();
    bool result = false;
    if (life_cycle_manager_session_established(squad_session)
        && squad_membership->all_peers_established()
        && life_cycle_manager_session_established(group_session)
        && group_membership->all_peers_established())
    {
        result = true;
        for (long squad_peer_index = (squad_membership)->get_first_peer(); squad_peer_index != -1;
             squad_peer_index = (squad_membership)->get_next_peer(squad_peer_index))
        {
            bool found = false;
            const s_network_session_peer *squad_peer = squad_membership->get_peer(squad_peer_index);
            for (long group_peer_index = (group_membership)->get_first_peer(); group_peer_index != -1;
                 group_peer_index = (group_membership)->get_next_peer(group_peer_index))
            {
                const s_network_session_peer *group_peer = group_membership->get_peer(group_peer_index);
                if (transport_secure_address_compare(&squad_peer->secure_address, &group_peer->secure_address))
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                result = false;
                break;
            }
        }
    }
    return result;
}





// @address 0x3de348 ?get_searching_party_values@@YAXPBUs_network_session_matchmaking_composition@@PBVc_hopper_configuration@@PAJ2PA_N@Z
void get_searching_party_values(const s_network_session_matchmaking_composition *composition,
    const c_hopper_configuration *hopper_configuration, long *minimum_party_size, long *maximum_party_size, bool *party_flag)
{
    long minimum = -1;
    long maximum = -1;
    bool flag = (composition->session_properties.composition_flags & (1 << 2)) != 0;
    for (long party_size = hopper_configuration->m_configuration.minimum_party_size; party_size <=
        hopper_configuration->m_configuration.maximum_party_size; party_size++)
    {
        if (composition->session_properties.acceptable_party_size_mask & (1 << (party_size - 1)))
        {
            if (minimum == -1 || minimum > party_size)
                minimum = party_size;
            if (maximum == -1 || maximum <= party_size)
                maximum = party_size;
        }
    }
    if (minimum == -1)
        minimum = 1;
    if (maximum == -1)
        maximum = 1;
    *minimum_party_size = minimum;
    *maximum_party_size = maximum;
    *party_flag = flag;
}
// @address 0x8c447c g_network_time_globals
// @address 0x3de630 life_cycle_update_session_qos_address
extern "C" void life_cycle_update_session_qos_address(c_life_cycle_state_handler *handler, s_life_cycle_session_address_state *state)
{
    unsigned long last_update = state->last_update;
    unsigned long now = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
    if ((long)(now - last_update) >= 1000)
    {
        s_transport_session_description description;
        s_life_cycle_qos_request address;
        if (::managed_session_get_security_information(handler->get_manager()->get_group_session()->managed_session_index(), false,
                &description, 0) &&
            life_cycle_initialize_qos_request(&description, &address))
        {
            if (!state->valid)
                state->valid = true;
            state->address = address;
        }
        state->last_update = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
    }
}

// @address 0x3de720 life_cycle_is_session_peers_ready
extern "C" bool life_cycle_is_session_peers_ready(c_life_cycle_state_handler *handler, c_network_session *session)
{
    bool result = true;
    const c_network_session_membership *membership = session->get_session_membership();
    for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
        peer_index = (membership)->get_next_peer(peer_index))
    {
        const s_network_session_peer *peer = &membership->m_current_membership.peers[peer_index];
        if (peer->properties.map_id != 0x10231971)
            return false;
        if (peer->properties.map_status == 0 || peer->properties.map_status == 2)
            result = false;
    }
    return result;
}


struct s_game_context;


struct s_owner;




// @address 0x3deeb8 ?set@?$c_generic_network_session_parameter@Us_replicated_life_cycle_matchmaking_progress_search_criteria@@@@QAA_NPBUs_replicated_life_cycle_matchmaking_progress_search_criteria@@@Z
template<> bool c_generic_network_session_parameter<s_replicated_life_cycle_matchmaking_progress_search_criteria>::set(const
    s_replicated_life_cycle_matchmaking_progress_search_criteria *settings)
{
    bool success = false;
    if (this->set_allowed()) {
        if (memcmp(&m_data, settings, sizeof(m_data)) != 0 || !(m_flags & 1)) {
            m_data = *settings;
            m_flags |= 1;
            for (int i = 0; i < 16; i++)
                m_peers_updated[i] = 0;
            notify_set_update_required();
        }
        success = true;
    }
    return success;
}

// @address 0x3dedd0 ?set@?$c_generic_network_session_parameter@Us_replicated_life_cycle_matchmaking_progress@@@@QAA_NPBUs_replicated_life_cycle_matchmaking_progress@@@Z
template<> bool c_generic_network_session_parameter<s_replicated_life_cycle_matchmaking_progress>::set(const
    s_replicated_life_cycle_matchmaking_progress *settings)
{
    bool success = false;
    if (this->set_allowed()) {
        if (memcmp(&m_data, settings, sizeof(m_data)) != 0 || !(m_flags & 1)) {
            m_data = *settings;
            m_flags |= 1;
            for (int i = 0; i < 16; i++)
                m_peers_updated[i] = 0;
            notify_set_update_required();
        }
        success = true;
    }
    return success;
}

// @address 0x3ebd18 ?disband_group_session@c_life_cycle_state_handler@@IAAXXZ
void c_life_cycle_state_handler::disband_group_session()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *group_session = manager->get_group_session();
    c_network_session_parameters *parameters = group_session->get_session_parameters();
    group_session->get_session_membership_for_update()->set_slot_counts(0, 0, false);
    parameters->m_parameters.session_mode.set(_network_session_mode_matchmaking_disbanding);
}




// @address 0x3de7c0 life_cycle_precache_mainmenu_after_game
extern "C" void life_cycle_precache_mainmenu_after_game(void)
{
    bool active = false;
    game_globals_storage *globals = tls_game_globals();
    char path[128];
    if (globals != 0 && globals->game_in_progress && !globals->initializing && globals->map_active)
        active = true;
    if (active && globals->options.game_mode == 2 && globals->game_finished_state != 0) {
        internal_critical_section_enter(22);
        path[0] = 0;
        strncpy(path, g_level_globals.default_map.name, 128);
        path[127] = 0;
        internal_critical_section_leave(22);
        loading_load_map_with_progress(-1, path, 1);
    }
}


static __forceinline const c_game_variant *life_cycle_manager_get_variant(const c_network_session_parameter_game_variant *parameter)
{
    const c_game_variant *result = 0;
    if (parameter->get_allowed() && parameter->m_data.valid)
        result = &parameter->m_data.game_variant;
    return result;
}

static __forceinline void life_cycle_manager_extract_machine_identifier(const s_transport_secure_address *address, s_machine_identifier *identifier)
{
    memcpy(identifier, address->identifier, sizeof(*identifier));
}

static __forceinline bool life_cycle_manager_participants_available(long session_type, const c_network_session_parameters *parameters)
{
    bool result = false;
    if (session_type == 1 && parameters->m_parameters.initial_participants.get_allowed())
    {
        const game_machine_options *machines = 0;
        const game_player_options *players = 0;
        if (parameters->m_parameters.initial_participants.get(&machines, &players) && machines && players)
            result = true;
    }
    return result;
}

static __forceinline bool life_cycle_manager_session_is_squad(long type)
{
    unsigned char result = type == 1;
    return result;
}

// @rdata 0x820266f4..0x820266fc
extern "C" {
const long life_cycle_manager_skill_bounds[2] = { 1, 50 };
}
// @rdata-end
static __forceinline const long &life_cycle_manager_pin_skill(const long &value)
{
    const long &pinned = value > life_cycle_manager_skill_bounds[0] ? value : life_cycle_manager_skill_bounds[0];
    return pinned < life_cycle_manager_skill_bounds[1] ? pinned : life_cycle_manager_skill_bounds[1];
}



static __forceinline bool life_cycle_manager_team_based(const c_hopper_configuration *hopper)
{
    return hopper->m_configuration.hopper_type == 2 || hopper->m_configuration.hopper_type == 3;
}
static __forceinline long life_cycle_manager_minimum_team_size(const c_hopper_configuration *hopper)
{
    long result = 1;
    if (hopper->m_configuration.hopper_type == 2 || hopper->m_configuration.hopper_type == 3)
        result = hopper->m_configuration.team_configuration.minimum_team_size;
    return result;
}
static __forceinline bool life_cycle_manager_keep_parties(const c_hopper_configuration *hopper)
{
    bool result = false;
    switch (hopper->m_configuration.hopper_type)
    {
    case 1: case 3: result = false; break;
    case 0: case 2: result = true; break;
    }
    return result;
}

static __forceinline long life_cycle_manager_team_count(const c_hopper_configuration *hopper)
{
    long result = 0;
    switch (hopper->m_configuration.hopper_type)
    {
    case 0: case 1: result = hopper->m_configuration.team_configuration.minimum_team_size; break;
    case 2: case 3: result = hopper->m_configuration.team_configuration.team_count; break;
    }
    return result;
}
static __forceinline long life_cycle_manager_maximum_team_size(const c_hopper_configuration *hopper)
{
    long result = 1;
    if (hopper->m_configuration.hopper_type == 2 || hopper->m_configuration.hopper_type == 3)
        result = hopper->m_configuration.team_configuration.maximum_team_size;
    return result;
}
static __forceinline long life_cycle_manager_team_imbalance(const c_hopper_configuration *hopper)
{
    long result = 0;
    switch (hopper->m_configuration.hopper_type)
    {
    case 2: if (hopper->m_configuration.team_configuration.configuration.unranked.allow_uneven_teams) result = 15; break;
    case 3: result = hopper->m_configuration.team_configuration.configuration.ranked.maximum_team_imbalance; break;
    }
    return result;
}

static __forceinline bool life_cycle_manager_single_unranked_party(
    const c_hopper_configuration *hopper, const s_network_session_matchmaking_composition *composition)
{
    bool result = false;
    if (!hopper->is_ranked() && composition->party_count == 1)
        result = true;
    return result;
}

static __forceinline bool life_cycle_manager_not(const bool &value) { return !value; }

// @address 0x3de8a0 ?pick_teams_for_group_internal@c_life_cycle_state_handler@@AAA_N_NK@Z
bool c_life_cycle_state_handler::pick_teams_for_group_internal(bool update, unsigned long excluded_parties)
{
    bool result = false;
    c_network_session *session = get_manager()->get_group_session();
    const c_hopper_configuration *hopper = multiplayer_game_hoppers_get_hopper_configuration(
        session->get_session_parameters()->m_parameters.matchmaking_hopper.get()->hopper_identifier);
    s_network_session_matchmaking_composition composition;
    network_session_build_matchmaking_composition(session, &composition);
    long teams[16];
    memset(teams, -1, sizeof(teams));
    if (life_cycle_manager_team_based(hopper))
    {
        long player_count = 0;
        s_network_team_picker_player players[16];
        memset(players, 0, sizeof(players));
        for (long player_index = 0; player_index < 16; ++player_index)
        {
            if ((composition.session_properties.player_valid_flags[player_index >> 5] & (1 << (player_index & 31))) != 0)
            {
                const s_matchmaking_session_player_properties *properties = &composition.session_properties.players[player_index];
                const s_network_session_player_matchmaking_composition *player = &composition.player_compositions[player_index];
                if (!(excluded_parties & (1 << properties->party_index)))
                {
                    players[player_count].skill_level = player->hopper_skill;
                    players[player_count].squad_index = properties->party_index;
                    players[player_count].peer_index = properties->peer_index;
                    ++player_count;
                }
            }
        }
        if (player_count > 0)
            result = network_team_picker_pick_teams(life_cycle_manager_team_count(hopper), life_cycle_manager_minimum_team_size(hopper),
                life_cycle_manager_maximum_team_size(hopper), life_cycle_manager_team_imbalance(hopper),
                    !life_cycle_manager_keep_parties(hopper), life_cycle_manager_single_unranked_party(hopper, &composition),
                true, true, player_count, players, 0, teams);
    }
    else
    {
        long count = 0;
        for (long player_index = 0; player_index < 16; ++player_index)
        {
            if ((composition.session_properties.player_valid_flags[player_index >> 5] & (1 << (player_index & 31))) != 0)
            {
                teams[count] = count;
                ++count;
            }
        }
        result = true;
    }
    if (result && update)
    {
        c_network_session_membership *membership = session->get_session_membership_for_update();
        long count = 0;
        for (long player_index = 0; player_index < 16; ++player_index)
        {
            if (membership->is_player_valid(player_index))
                membership->m_current_membership.players[player_index].configuration.host.team_index = teams[count++];
        }
        membership->increment_update();
    }
    return result;
}






static __forceinline void life_cycle_manager_join_local_squad()
{
    g_network_join_data.group_join_result = 1;
    memset(&g_network_join_data.queued_group_join, 0, sizeof(g_network_join_data.queued_group_join));
    g_network_join_data.request_join_squad_to_group = true;
    g_network_join_data.request_join_group_to_group = false;
    g_network_join_data.queued_group_join.session_class = _network_session_class_xbox_live;
}

// @address 0x3dd7a0 ?setup_initial_participants@c_life_cycle_state_handler@@IAA_NPAVc_network_session@@@Z
bool c_life_cycle_state_handler::setup_initial_participants(c_network_session *session)
{
    bool result = false;
    if (session->m_local_state == 6)
    {
        const c_network_session_membership *membership = session->get_session_membership();
        c_network_session_parameters *parameters = session->get_session_parameters();
        do
        {
            if (session->m_session_type == 1 && parameters->m_parameters.ui_game_mode.get() && *parameters->m_parameters.ui_game_mode.get() == 4)
                break;
            if (session->m_session_type == 1 && parameters->m_parameters.initial_participants.get_allowed())
            {
                const game_machine_options *existing_machines = 0;
                const game_player_options *existing_players = 0;
                if (parameters->m_parameters.initial_participants.get(&existing_machines, &existing_players) && existing_machines && existing_players)
                    break;
            }
            const c_game_variant *variant = 0;
            unsigned long hopper = 0xffff;
            game_player_options players[16];
            game_machine_options machines;
            memset(&machines, 0, sizeof(machines));
            memset(players, 0, sizeof(players));
            if (session->m_session_type == 2)
                hopper = parameters->m_parameters.matchmaking_hopper.get()->hopper_identifier;
            if (session->m_session_type == 1 && parameters->m_parameters.game_variant.get_allowed())
            {
                variant = life_cycle_manager_get_variant(&parameters->m_parameters.game_variant);
            }
            unsigned long machine_valid_mask = membership->get_peer_valid_mask();
            machines.valid_machine_mask = machine_valid_mask;
            long peer = (membership)->get_first_peer();
            if (peer != -1)
            {
                for (; peer != -1; peer = (membership)->get_next_peer(peer))
                {
                    const s_transport_secure_address *address = membership->get_peer_address(peer);
                    life_cycle_manager_extract_machine_identifier(address, &machines.machines[peer]);
                }
                machine_valid_mask = machines.valid_machine_mask;
            }
            long quality = 0;
            bool adjust_skill = false;
            if (session->m_session_type == 2)
            {
                adjust_skill = true;
                quality = multiplayer_game_hopper_compute_match_quality(membership);
            }
            for (long player = (membership)->get_first_player(); player != -1;
                player = (membership)->get_next_player(player))
            {
                const s_network_session_player *session_player = &membership->m_current_membership.players[player];
                if (membership->m_current_membership.players[player].controller_index != -1)
                    build_player_options(session_player, machine_valid_mask, machines.machines, variant, hopper, player, adjust_skill,
                        quality, &players[player]);
            }
            parameters->m_parameters.initial_participants.set(&machines, players);
        } while (false);
        result = true;
    }
    return result;
}

// @address 0x93a80 ?update_handle_session_states@c_life_cycle_state_manager@@AAAXXZ
void c_life_cycle_state_manager::update_handle_session_states()
{
    c_network_session *group = get_group_session();
    c_network_session *squad = get_active_squad_session();
    c_life_cycle_state_handler *handler = get_current_state_handler();
    if (inl_test_flag(handler->get_handler_flags(), 5) && !online_local_xuid_is_online_enabled())
    {
        bool leaving = false;
        request_state_change(_life_cycle_state_leaving, sizeof(leaving), &leaving);
    }
    unsigned char handler_flags = handler->get_handler_flags();
    if (inl_test_flag(handler_flags, 1))
    {
        if (group->disconnected())
        {
            if (inl_test_flag(handler_flags, 4))
            {
                long squad_state = squad->m_local_state;
                if (life_cycle_manager_states_established(squad) && squad->get_session_parameters()->m_parameters.matchmaking_hopper.get_allowed())
                    life_cycle_manager_join_local_squad();
                else
                {
                    if (squad_state != 0)
                        squad->disband_and_reestablish_as_host(false);
                    else
                        network_life_cycle_create_local_squad(_network_session_class_xbox_live);
                    request_state_change(_life_cycle_state_none, 0, 0);
                    squad->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_idle);
                }
            }
            if (inl_test_flag(handler->get_handler_flags(), 3))
            {
                long squad_state = squad->m_local_state;
                if (life_cycle_manager_states_established(squad) && squad->is_host() && squad->get_session_membership()->get_peer_count() == 1)
                {
                    request_state_change(_life_cycle_state_matchmaking_start, sizeof(k_matchmaking_start_entry_data), k_matchmaking_start_entry_data);
                    squad->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_start);
                }
                else
                {
                    if (squad_state != 0)
                        squad->disband_and_reestablish_as_host(false);
                    else
                        network_life_cycle_create_local_squad(_network_session_class_xbox_live);
                    request_state_change(_life_cycle_state_none, 0, 0);
                    squad->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_idle);
                }
            }
        }
    }
    else if ((handler_flags & (1 << 2)) == 0 && !group->disconnected() && !group->leaving_session())
    {
        group->leave_session();
    }
    if (inl_test_flag(handler->get_handler_flags(), 0))
    {
        if (squad->leaving_session())
        {
            bool leaving = false;
            request_state_change(_life_cycle_state_leaving, sizeof(leaving), &leaving);
        }
        else if (squad->disconnected())
            request_state_change(_life_cycle_state_none, 0, 0);
    }
}

// @address 0x3de3f0 ?update_matchmaking_search_criteria@c_life_cycle_state_handler@@IAAX_N@Z
void c_life_cycle_state_handler::update_matchmaking_search_criteria(bool use_group)
{
    const c_hopper_configuration *hopper = multiplayer_game_hoppers_get_current_hopper_configuration();
    if (hopper)
    {
        c_life_cycle_state_manager *manager = get_manager();
        c_network_session *squad = manager->get_active_squad_session();
        c_network_session_parameters *parameters = squad->get_session_parameters();
        s_network_session_matchmaking_composition composition;
        if (network_session_build_matchmaking_composition(squad, &composition) == _network_session_matchmaking_composition_build_result_success)
        {
            s_replicated_life_cycle_matchmaking_progress_search_criteria criteria = {};
            long minimum_skill = composition.session_properties.minimum_skill_level;
            long maximum_skill = composition.session_properties.maximum_skill_level;
            criteria.party_skill_range_min = maximum_skill;
            criteria.party_skill_range_max = minimum_skill;
            criteria.searching_skill_range_min = life_cycle_manager_pin_skill(minimum_skill -
                multiplayer_game_hopper_compute_skill_match_delta(minimum_skill));
            criteria.searching_skill_range_max = life_cycle_manager_pin_skill(maximum_skill +
                multiplayer_game_hopper_compute_skill_match_delta(maximum_skill));
            if (use_group && network_session_build_matchmaking_composition(manager->get_group_session(), &composition) !=
                _network_session_matchmaking_composition_build_result_success)
            {
                criteria.searching_party_size_min = hopper->m_configuration.minimum_party_size;
                criteria.searching_party_size_max = hopper->m_configuration.maximum_party_size;
            }
            else
                get_searching_party_values(&composition, hopper, &criteria.searching_party_size_min, &criteria.searching_party_size_max,
                    &criteria.search_party_big_and_mixed);
            criteria.hopper_required_player_count = hopper->get_minimum_player_count();
            criteria.hopper_preferred_player_count = hopper->get_maximum_player_count();
            parameters->m_parameters.matchmaking_progress_search_criteria.set(&criteria);
        }
    }
}


static __forceinline unsigned short life_cycle_manager_get_current_hopper_identifier()
{
    unsigned short hopper_identifier = 0xffff;
    if (multiplayer_game_hoppers_get_current_hopper_configuration())
        hopper_identifier = multiplayer_game_hoppers_get_current_hopper_configuration()->m_configuration.hopper_identifier;
    return hopper_identifier;
}

static __forceinline void life_cycle_manager_set_ready_hopper_identifier(unsigned short hopper_identifier, e_session_game_start_error error)
{
    g_network_session_interface_globals.peer.hopper_identifier = hopper_identifier;
    g_network_session_interface_globals.peer.hopper_error = error;
}

// @address 0x3de560 ?matchmaking_update_required_files_for_hopper@c_life_cycle_state_handler@@IAAXXZ
void c_life_cycle_state_handler::matchmaking_update_required_files_for_hopper()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad = manager->get_active_squad_session();
    c_network_session_parameters *parameters = squad->get_session_parameters();
    unsigned short hopper_identifier = 0xffff;
    unsigned short current_hopper_identifier = life_cycle_manager_get_current_hopper_identifier();
    if (parameters->m_parameters.matchmaking_hopper.get_allowed())
        hopper_identifier = parameters->m_parameters.matchmaking_hopper.get()->hopper_identifier;
    if (hopper_identifier != 0xffff && hopper_identifier != current_hopper_identifier)
        multiplayer_game_hopper_set_active_hopper_and_request_game_set(hopper_identifier, false, 0);
    e_session_game_start_error error = multiplayer_game_hopper_compute_start_error(true, hopper_identifier != 0xffff);
    life_cycle_manager_set_ready_hopper_identifier(hopper_identifier, error);
}
// @address 0x93780 life_cycle_manager_handle_game_end_errors
extern "C" void life_cycle_manager_handle_game_end_errors(c_life_cycle_state_manager *manager)
{
    c_network_session *squad = manager->get_active_squad_session();
    if (life_cycle_manager_states_established(squad) && simulation_starting_up() && tls_game_globals()->options.game_mode == 3) {
        c_network_session_parameters *parameters = squad->get_session_parameters();
        s_transport_secure_identifier identifier;
        if (managed_session_get_id(squad->managed_session_index(), &identifier)) {
            manager->m_cached_identifier = identifier;
            manager->m_cached_identifier_valid = true;
        }
        if (manager->m_cached_identifier_valid) {
            if (parameters->m_parameters.matchmaking_messaging.get_allowed()) {
                const s_network_session_parameter_matchmaking_messaging *game_end = parameters->m_parameters.matchmaking_messaging.get();
                c_network_session_membership *membership = squad->get_session_membership_for_update();
                long local_peer = membership->m_local_peer_index;
                long player_sequence = 0x100000;
                for (long player = (membership)->get_first_player(); player != -1;
                    player = (membership)->get_next_player(player))
                {
                    if (membership->m_current_membership.players[player].peer_index == local_peer &&
                        player_sequence > membership->m_current_membership.players[player].player_sequence_number)
                        player_sequence = membership->m_current_membership.players[player].player_sequence_number;
                }
                if (game_end->display_cookie != manager->m_last_game_end_sequence && player_sequence < game_end->message_data) {
                    switch (game_end->message_type) {
                    case 0: g_gui_error_manager.post_error_with_custom_message(0x20046, 0, 0, k_any_controller, false, true); break;
                    case 1: g_gui_error_manager.post_error_with_custom_message(0x20030, 0, 0, k_any_controller, false, true); break;
                    case 2: g_gui_error_manager.post_error_with_custom_message(0x2007f, 0, 0, k_any_controller, false, false); break;
                    case 3: g_gui_error_manager.post_error_with_custom_message(0x20051, 0, 0, k_any_controller, false, false); break;
                    case 4:
                        if (parameters->m_parameters.request_campaign_quit.get_allowed()) {
                            const s_network_session_parameter_request_campaign_quit *campaign_quit = parameters->m_parameters.request_campaign_quit.get();
                            if (campaign_quit->quit_request_type != 0) {
                                if (campaign_quit->quit_request_type == 1) {
                                    if (campaign_quit->quit_request_peer_index != squad->get_session_membership_for_update()->m_local_peer_index)
                                        g_gui_error_manager.post_error_with_custom_message(0x20049, 0, 0, k_any_controller, false, true);
                                } else if (campaign_quit->quit_request_type == 3)
                                    g_gui_error_manager.post_error_with_custom_message(0x20048, 0, 0, k_any_controller, false, true);
                            }
                        }
                        break;
                    }
                }
                manager->m_last_game_end_sequence = game_end->display_cookie;
            } else manager->m_last_game_end_sequence = -1;
        }
    }
    e_life_cycle_state state = manager->get_current_state();
    bool matchmaking = false;
    if (state >= 11 && (state <= 14 || state == 17)) matchmaking = true;
    if (matchmaking && manager->get_group_session()->m_local_state == 10)
        g_gui_error_manager.post_error_with_custom_message(0x2007d, 0, 0, k_any_controller, false, false);
}

// @address 0x266c0 k_matchmaking_start_entry_data
