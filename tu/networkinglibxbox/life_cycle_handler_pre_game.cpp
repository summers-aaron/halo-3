// networkinglibxbox/life_cycle_handler_pre_game.obj: from the records 0x3df030 0x3df160 0x3df3e8 0x3df4f8 0x3df5b8 0x3df6b0 0x3dfa48 0x3e0b70 0x4113f0 0x3e0068
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/logic/life_cycle/life_cycle_state_transition_types.h"
#include "saved_games/game_state_xenon_types.h"
#include "networking/logic/network_leaderboard_globals_types.h"
#include "networking/logic/network_session_interface_globals.h"
#include "networking/session/network_session_interface_types.h"
#include "shell/shell.h"
#include "main/levels_globals.h"
#include "networking/logic/network_join_types.h"
#include "game/game_variant.h"
#include "networking/session/network_game_countdown_delayed_reason_types.h"
#include "game/player_types.h"
#include "game/game_types.h"
#include "cseries/cseries.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "game/multiplayer_game_hopper.h"
#include "game/multiplayer_game_hopper_globals.h"
#include "networking/logic/network_join.h"
#include "networking/online/online_xenon.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "saved_games/saved_film.h"
#include "networking/logic/life_cycle/life_cycle_handler_pre_game.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"
#include "networking/session/network_session_types.h"
#include "interface/user_interface_error_manager.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "networking/logic/network_leaderboard.h"
#include "networking/session/network_observer.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/life_cycle_handler_pre_game_services.h"
#include "networking/session/network_session_parameter_game_types.h"
#include "saved_games/saved_film_description.h"
#include "networking/session/session_game_start_error_types.h"
#include "game/hopper_configuration.h"
#include "networking/network_time_types.h"
#include "networking/network_time_access.h"
#include "cseries/crt_xdk.h"
#include "networking/session/network_session_parameter_specializations.h"
#include "game/multiplayer_game_hopper_playability.h"
#include "networking/session/network_session_parameters_generic_set.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "networking/logic/life_cycle/life_cycle_session.h"
// @address 0x494920 ?clear_error@c_gui_error_manager@@QAAXJW4e_controller_index@@PB_W@Z

enum e_network_game_start_mode;
class c_network_session;

// s_network_session_peer_properties::map_status (network_session_interface sets 0..4; "peer-map-status" is 3 bits)
enum e_network_session_map_status
{
    _network_session_map_status_none = 0,
    _network_session_map_status_failed = 1,
    _network_session_map_status_precaching = 2,
    _network_session_map_status_precached = 3,
    _network_session_map_status_loaded = 4,
};

union s_pre_game_out_of_sync_storage {
    s_network_session_parameter_synchronous_out_of_sync parameter;
    unsigned short word;
};

// host_update_countdown callees

enum e_pre_game_hopper_type
{
    _pre_game_hopper_type_ffa_unranked = 0,
    _pre_game_hopper_type_ffa_ranked = 1,
    _pre_game_hopper_type_team_unranked = 2,
    _pre_game_hopper_type_team_ranked = 3,
};

static __forceinline bool life_cycle_handler_pre_game_level_hidden(const levels_map_datum *level)
{
    return ((level->flags >> 2) & 1) != 0;
}

// @address 0x3e0b70 ?reset_map_request@c_life_cycle_state_handler_pre_game@@AAAXXZ
void c_life_cycle_state_handler_pre_game::reset_map_request()
{
    c_network_session *state = get_manager()->get_active_squad_session();
    c_network_session_parameters::s_network_session_parameters_internal *parameters = &state->get_session_parameters()->m_parameters;
    if (!parameters->ui_game_mode.get_allowed())
        return;
    const e_gui_game_mode *allowed_data = 0;
    c_generic_network_session_parameter<e_gui_game_mode> *allowed_slot = (c_generic_network_session_parameter<e_gui_game_mode>
        *)&parameters->ui_game_mode;
    if (allowed_slot->m_flags & 1)
        allowed_data = &allowed_slot->m_data;
    if (*allowed_data != 0)
        return;
    if ((parameters->map.m_flags & 1) == 0)
        return;
    if (parameters->map.change_request_allowed() == 0)
        return;
    s_network_session_parameter_map *request_data = 0;
    c_generic_network_session_parameter<s_network_session_parameter_map> *request_slot = &parameters->map;
    if (request_slot->m_flags & 1)
        request_data = &request_slot->m_data;
    long map_id = request_data->map_id;
    s_datum level;
    if (levels_try_and_get_datum(g_level_globals.map_levels, map_id, &level) == 0)
        return;
    if (life_cycle_handler_pre_game_level_hidden(&level))
        return;
    s_network_session_parameter_map request;
    memset(&request, 0, sizeof(request));
    request.campaign_id = _campaign_id_pre_game_fallback;
    request.map_id = _map_id_pre_game_fallback;
    levels_get_map_scenario_path(_campaign_id_pre_game_fallback, _map_id_pre_game_fallback, request.scenario_path, sizeof(request.scenario_path));
    ((c_generic_network_session_parameter<s_network_session_parameter_map> *)&parameters->map)->request_change(&request);
}

class c_life_cycle_state_manager;
class c_network_session;
extern "C" {
// @address 0x8c447c g_network_time_globals
}
static __forceinline c_network_session_parameters::s_network_session_parameters_internal
    *life_cycle_handler_pre_game_get_session_parameters(c_network_session *session)
{
    return &session->get_session_parameters()->m_parameters;
}
static __forceinline long life_cycle_handler_pre_game_countdown_timer_get_valid_value(const c_network_session_parameter_countdown_timer *slot)
{
    long result = -1;
    if (slot->get_allowed() && slot->m_data.running)
        result = slot->m_data.countdown;
    return result;
}

static __forceinline e_network_session_mode life_cycle_handler_pre_game_session_mode_get_value(const
    c_network_session_parameter_session_mode *mode_parameter)
{
    e_network_session_mode mode = _network_session_mode_none;
    if (mode_parameter->get_allowed())
        mode = mode_parameter->m_data.mode;
    return mode;
}

static __forceinline e_network_session_mode life_cycle_handler_pre_game_squad_session_mode(const c_network_session *squad_session)
{
    return life_cycle_handler_pre_game_session_mode_get_value((const c_network_session_parameter_session_mode
        *)&squad_session->get_session_parameters()->m_parameters.session_mode);
}

// @address 0x3df030 ?update@c_life_cycle_state_handler_pre_game@@UAAXXZ
void c_life_cycle_state_handler_pre_game::update()
{
    c_network_session *squad_session = get_manager()->get_active_squad_session();
    if (((long)::network_squad_session_get_start_mode()) == 2)
    {
        if (squad_session->is_host())
            matchmaking_host_update();
        matchmaking_update_required_files_for_hopper();
    }
    if (squad_session->is_leader())
        reset_map_request();
    if (squad_session->is_host())
        squad_game_start_status_update();
    if (squad_session->get_session_parameters()->m_parameters.session_mode.get() == 1)
    {
        if (squad_session->is_leader())
            leader_update_countdown();
        if (squad_session->is_host())
            host_update_countdown();
    }
}

// @address 0x3df4f8 ?exit@c_life_cycle_state_handler_pre_game@@UAAXPAVc_life_cycle_state_handler@@@Z
void c_life_cycle_state_handler_pre_game::exit(c_life_cycle_state_handler *handler)
{
    c_network_session *state = get_manager()->get_active_squad_session();
    state->m_close_session_for_matchmaking = false;
    if (((const c_network_session *)(state))->is_host()) {
        s_network_session_parameter_game_start_status request;
        memset(&request, 0, sizeof(request));
        request.status = 1;
        state->get_session_parameters()->m_parameters.game_start_status.set(&request);
    }
    ::network_squad_session_get_matchmaking_tip(0, -1);
    g_gui_error_manager.clear_error(0x20049, k_any_controller, 0);
    g_gui_error_manager.clear_error(0x20048, k_any_controller, 0);
}

struct s_network_leaderboard_globals;
struct s_networking_join_data;

static __forceinline void life_cycle_handler_pre_game_network_leaderboard_refresh(c_network_session *session)
{
    if (session->m_local_state != 0 && session->m_session_type == 2) {
        session->m_disconnect_reestablish_state = 1;
        session->disconnect();
    }
}

static __forceinline void life_cycle_handler_pre_game_cache_file_map_clear_all_failures()
{
    network_leaderboard_globals.refresh_pending = 1;
    memset(xenon_game_state_globals.failed_maps, 0, sizeof(xenon_game_state_globals.failed_maps));
}

static __forceinline void life_cycle_handler_pre_game_session_interface_clear_peer_status_flags()
{
    ((s_network_session_interface_peer_properties *)&g_network_session_interface_globals)->flags = 0;
}

static __forceinline void life_cycle_handler_pre_game_set_join_queue_mode(e_network_join_queue_mode mode)
{
    e_network_join_queue_mode *join_queue_mode = &g_network_join_data.join_queue_mode;
    if (*join_queue_mode != mode)
        network_join_flush_join_queue();
    *join_queue_mode = mode;
}

// @address 0x3df3e8 ?enter@c_life_cycle_state_handler_pre_game@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_pre_game::enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data)
{
    unsigned short cleared_out_of_sync[2];
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad_session = manager->get_active_squad_session();
    c_life_cycle_state_handler::enter(from_handler, 0, 0);
    m_last_countdown = -1;
    m_last_countdown_update = 0;
    m_entry_state = 0;
    c_network_session *target_session = manager->get_target_session();
    life_cycle_handler_pre_game_network_leaderboard_refresh(target_session);
    life_cycle_handler_pre_game_cache_file_map_clear_all_failures();
    life_cycle_handler_pre_game_session_interface_clear_peer_status_flags();
    squad_session->m_close_session_for_matchmaking = true;
    if (squad_session->is_host()) {
        memset(cleared_out_of_sync, 0, sizeof(unsigned short));
        squad_session->get_session_parameters()->m_parameters.synchronous_out_of_sync.set((const
            s_network_session_parameter_synchronous_out_of_sync *)cleared_out_of_sync);
    }
    network_group_session_set_disable_host_election(0);
    life_cycle_handler_pre_game_set_join_queue_mode(_network_join_open_to_join_squad);
}

static __forceinline s_networking_join_data *life_cycle_handler_pre_game_join_data() { return (s_networking_join_data *)(&g_network_join_data); }
// @address 0x3df5b8 ?leader_update_countdown@c_life_cycle_state_handler_pre_game@@AAAXXZ
void c_life_cycle_state_handler_pre_game::leader_update_countdown()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad_session = manager->get_active_squad_session();
    c_network_session_parameters::s_network_session_parameters_internal *parameters =
        life_cycle_handler_pre_game_get_session_parameters(squad_session);
    long countdown = life_cycle_handler_pre_game_countdown_timer_get_valid_value((c_network_session_parameter_countdown_timer
        *)&parameters->countdown_timer);
    if (countdown != m_last_countdown) {
        m_last_countdown_update = network_time_get();
        m_last_countdown = countdown;
    }
    if (countdown >= 0) {
        unsigned long last_update = m_last_countdown_update;
        if (network_time_since_inline(last_update) >= 1000 && countdown > 0) {
            unsigned long now = network_time_get();
            long next = countdown - 1;
            m_last_countdown_update = now;
            m_last_countdown = next;
            ((c_network_session_parameter_countdown_timer *)&parameters->countdown_timer)->request_change(true, next,
                _network_game_countdown_delayed_reason_none,
                0);
        }
    }
}

// @address 0x3df6b0 ?host_update_countdown@c_life_cycle_state_handler_pre_game@@AAAXXZ
void c_life_cycle_state_handler_pre_game::host_update_countdown()
{
    long countdown_value = -1;
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad_session = manager->get_active_squad_session();
    c_network_session_parameters::s_network_session_parameters_internal *parameters =
        life_cycle_handler_pre_game_get_session_parameters(squad_session);
    c_network_session_parameter_countdown_timer *countdown = (c_network_session_parameter_countdown_timer *)&parameters->countdown_timer;
    bool proceed = false;
    c_network_session_membership *membership = squad_session->get_session_membership_for_update();
    long valid_value = life_cycle_handler_pre_game_countdown_timer_get_valid_value(countdown);
    countdown_value = valid_value;
    if (valid_value != -1) {
        if (!parameters->game_start_status.get_allowed()) proceed = true;
        else {
            const s_network_session_parameter_game_start_status *slot_values = parameters->game_start_status.get();
            if (slot_values->error != 0 || slot_values->status == 7) proceed = true;
        }
    }
    if (!proceed && countdown_value != -1) {
        if (!parameters->start_mode.get_allowed()) proceed = true;
        else if (*parameters->start_mode.get() == 0) proceed = true;
    }
    if (!proceed && countdown_value == 0)
    {
        const e_network_game_start_mode *mode_ptr = parameters->start_mode.get();
        switch (*mode_ptr) {
        case 1: {
            bool map_ready = false;
            long map_ids[2];
            map_ids[0] = -1;
            map_ids[1] = -1;
            if (membership->m_current_membership.update_number != -1 && membership->m_current_membership.host_peer_index != -1)
                map_ready = (membership->m_local_peer_index == membership->m_current_membership.host_peer_index);
            if (map_ready && membership->m_current_membership.player_count == 1) {
                if (network_life_cycle_session_get_map((e_campaign_id *)&map_ids[0], (e_map_id *)&map_ids[1]) &&
                    map_ids[0] != -1 &&
                    !membership->is_player_valid(0)) {
                    unsigned long *valid_mask = membership->m_current_membership.valid_player_mask.get_writeable_bits_direct();
                    long player_index = (membership)->get_first_player();
                    s_network_session_player *first_player = &membership->m_current_membership.players[0];
                    memcpy(first_player, &membership->m_current_membership.players[player_index], sizeof(*first_player));
                    memset(&membership->m_current_membership.players[player_index], 0, sizeof(s_network_session_player));
                    long mask_index = player_index >> 5;
                    valid_mask[mask_index] = valid_mask[mask_index] & ~(1u << (player_index & 0x1f));
                    *valid_mask = *valid_mask | 1;
                    membership->m_current_membership.peers[first_player->peer_index].user_player_indices[first_player->peer_user_index] = 0;
                }
            }
            squad_session->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_setup);
            break;
        }
        default: {
                const s_saved_film_description *film = (const s_saved_film_description *)parameters->saved_film.get();
                if (!saved_film_can_load(film->controller_index, film->file_path)) {
                    s_saved_film_description film_desc;
                    memset(&film_desc, 0, sizeof(film_desc));
                    film_desc.film_type = 6;
                    film_desc.difficulty = _campaign_difficulty_level_normal;
                    film_desc.controller_index = -1;
                    network_squad_session_set_film(&film_desc);
                    proceed = true;
                    break;
                }
                if (life_cycle_handler_pre_game_join_data()->join_queue_mode != _network_join_closed_to_all_joins)
                    network_join_flush_join_queue();
                life_cycle_handler_pre_game_join_data()->join_queue_mode = _network_join_closed_to_all_joins;
                squad_session->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_setup);
                break;
        }
        case 2: {
            ((c_network_session_parameter_countdown_timer *)countdown)->set(false, 0, _network_game_countdown_delayed_reason_none, NULL);
            if (squad_session->m_session_class != 2)
                break;
            unsigned short hopper_id = 0xffff;
            if (parameters->matchmaking_hopper.get_allowed()) {
                const unsigned short *hopper = &parameters->matchmaking_hopper.get()->hopper_identifier;
                hopper_id = *hopper;
            }
            if (multiplayer_game_is_playable(hopper_id, true, true, membership, NULL) != _session_game_start_error_none)
                break;
            squad_session->get_session_parameters()->m_parameters.session_mode.set(_network_session_mode_matchmaking_start);
            break;
        }
        }
    }
    if (proceed)
        ((c_network_session_parameter_countdown_timer *)countdown)->request_change(false, 0, _network_game_countdown_delayed_reason_none, NULL);
}

// @address 0x4113f0 ?set@c_network_session_parameter_game_start_status@@QAA_NPBUs_network_session_parameter_game_start_status@@@Z
// match: 0x4113f0: __restrict lets the m_flags load (lwz 0x28) schedule before the 12-byte m_data copy instead of reloading after it
bool c_network_session_parameter_game_start_status::set(const s_network_session_parameter_game_start_status *__restrict start_status)
{
    bool result = false;
    if (set_allowed()) {
        if (memcmp(start_status, &m_data, sizeof(m_data)) != 0 || !get_allowed()) {
            m_data = *start_status;
            set_update_required();
        }
        result = true;
    }
    return result;
}

// @address 0x3df160 ?update_for_state_transition@c_life_cycle_state_handler_pre_game@@UAA?AW4e_life_cycle_state_transition_type@@XZ
e_life_cycle_state_transition_type c_life_cycle_state_handler_pre_game::update_for_state_transition()
{
    e_life_cycle_state_transition_type transition = _life_cycle_state_transition_none;
    e_network_session_mode session_mode;
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad_session = manager->get_active_squad_session();
    session_mode = life_cycle_handler_pre_game_squad_session_mode(squad_session);
    switch (session_mode)
    {
    case _network_session_mode_end_game:
        manager->request_state_change(_life_cycle_state_end_game_write_stats, 0, NULL);
        transition = _life_cycle_state_transition_change;
        break;
    case _network_session_mode_setup:
    case _network_session_mode_in_game:
    {
        const c_network_session_parameters::s_network_session_parameters_internal *parameters =
            &squad_session->get_session_parameters()->m_parameters;
        if (parameters->map.get_allowed())
        {
        const s_network_session_parameter_map *lobby = parameters->map.get();
        bool disconnect = false;
        if (lobby->map_id == NONE)
        {
            disconnect = true;
        }
        else if (!lobby->scenario_path[0])
        {
            disconnect = true;
        }
        else
        {
            bool joining = false;
            bool failed = false;
            const s_network_session_peer *local_peer = squad_session->get_session_membership()->get_local_peer();
            if (local_peer->properties.map_id == lobby->map_id)
            {
                switch (local_peer->properties.map_status)
                {
                case _network_session_map_status_none:
                case _network_session_map_status_precaching:
                    break;
                case _network_session_map_status_precached:
                case _network_session_map_status_loaded:
                    joining = true;
                    break;
                default:
                    failed = true;
                    break;
                }
            }
            if (joining && render_is_loading_complete(1))
            {
                transition = _life_cycle_state_transition_change;
                manager->request_state_change(_life_cycle_state_start_game, 0, NULL);
            }
            else if (failed)
            {
                disconnect = true;
            }
            else
            {
                transition = _life_cycle_state_transition_continue;
            }
        }
        if (disconnect)
            squad_session->initiate_leave_protocol(false);
        }
        else
        {
            transition = _life_cycle_state_transition_continue;
        }
        break;
    }
    case _network_session_mode_idle:
        transition = _life_cycle_state_transition_continue;
        break;
    case _network_session_mode_matchmaking_start:
    case _network_session_mode_matchmaking_searching:
    case _network_session_mode_matchmaking_slave:
        manager->request_state_change(_life_cycle_state_matchmaking_start, sizeof(k_matchmaking_start_entry_data),
            k_matchmaking_start_entry_data);
        transition = _life_cycle_state_transition_change;
        break;
    }
    return transition;
}

static __forceinline const short *life_cycle_handler_pre_game_requested_hopper_get(const c_generic_network_session_parameter<short> *parameter)
{
    const short *result = NULL;
    if (parameter->get_allowed())
        result = parameter->get();
    return result;
}

static __forceinline unsigned short life_cycle_handler_pre_game_get_current_hopper_identifier()
{
    unsigned short result = 0xFFFF;
    if (g_multiplayer_game_hopper_globals.m_current_configuration != NULL)
        result = g_multiplayer_game_hopper_globals.m_current_configuration->get_hopper_identifier();
    return result;
}

static __forceinline const c_hopper_configuration *life_cycle_handler_pre_game_get_current_hopper_configuration()
{
    return g_multiplayer_game_hopper_globals.m_current_configuration;
}

static __forceinline bool life_cycle_handler_pre_game_all_player_experience_ratings_are_set(const c_network_session_membership *membership)
{
    bool result = true;
    for (long player_index = (membership)->get_first_player(); player_index != NONE;
        player_index = (membership)->get_next_player(player_index))
    {
        if (!membership->m_current_membership.players[player_index].configuration.host.global_stats_valid)
        {
            result = false;
            break;
        }
    }
    return result;
}

static __forceinline const s_network_session_matchmaking_hopper_category *life_cycle_handler_pre_game_get_category_from_index(long
    category_index)
{
    const s_network_session_matchmaking_hopper_category *result = NULL;
    if (category_index < g_multiplayer_game_hopper_globals.m_configuration.category_count)
        result = (const s_network_session_matchmaking_hopper_category *)&g_multiplayer_game_hopper_globals.m_configuration.categories[category_index];
    return result;
}

static __forceinline unsigned short life_cycle_handler_pre_game_get_hopper_identifier(long hopper_index)
{
    unsigned short result = 0xFFFF;
    if (hopper_index < g_multiplayer_game_hopper_globals.m_configuration.count)
    {
        result = g_multiplayer_game_hopper_globals.m_configuration.entries[hopper_index].get_hopper_identifier();
        if (result == 0xFFFF)
            result = 0xFFFF;
    }
    return result;
}

template <typename t_type>
static __forceinline const t_type &life_cycle_handler_pre_game_min(const t_type &left, const t_type &right)
{
    return (right < left) ? right : left;
}

// @address 0x3dfa48 ?matchmaking_host_update@c_life_cycle_state_handler_pre_game@@AAAXXZ
void c_life_cycle_state_handler_pre_game::matchmaking_host_update()
{
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *squad_session = manager->get_active_squad_session();
    const c_network_session_membership *membership = squad_session->get_session_membership();
    c_network_session_parameters::s_network_session_parameters_internal *parameters = &squad_session->get_session_parameters()->m_parameters;
    const s_network_session_matchmaking_hopper *session_hopper = NULL;
    const s_network_session_matchmaking_hopper_list *session_hopper_list = NULL;
    unsigned short requested_hopper_identifier = 0xFFFF;

    if (parameters->matchmaking_hopper.get_allowed())
        session_hopper = parameters->matchmaking_hopper.get();
    if (parameters->matchmaking_hopper_list.get_allowed())
        session_hopper_list = parameters->matchmaking_hopper_list.get();
    if (parameters->matchmaking_requested_hopper.get_allowed())
        requested_hopper_identifier = (unsigned short)*life_cycle_handler_pre_game_requested_hopper_get(&parameters->matchmaking_requested_hopper);

    unsigned short current_hopper_identifier = life_cycle_handler_pre_game_get_current_hopper_identifier();
    long load_status = multiplayer_game_hopper_get_load_status();

    if ((current_hopper_identifier == 0xFFFF || (requested_hopper_identifier != 0xFFFF
        && requested_hopper_identifier != current_hopper_identifier)) &&
        load_status == 2)
    {
        short none_hopper = NONE;
        parameters->matchmaking_requested_hopper.set(&none_hopper);
        multiplayer_game_hopper_set_active_hopper_and_request_game_set(requested_hopper_identifier, true, membership);
        requested_hopper_identifier = life_cycle_handler_pre_game_get_current_hopper_identifier();
    }

    if (session_hopper == NULL || session_hopper->hopper_identifier != requested_hopper_identifier)
    {
        const c_hopper_configuration *hopper = life_cycle_handler_pre_game_get_current_hopper_configuration();
        s_network_session_matchmaking_hopper hopper_data;
        memset(&hopper_data, 0, sizeof(hopper_data));
        hopper_data.system_seconds = (unsigned long)_time64(NULL);
        if (hopper != NULL)
        {
            hopper_data.hopper_identifier = hopper->get_hopper_identifier();
            hopper_data.xlast_index = (unsigned char)hopper->get_xlast_index();
            hopper_data.is_ranked = hopper->is_ranked();
            hopper_data.veto_allowed = hopper->is_veto_enabled();
            hopper_data.rematch_allowed = hopper->get_rematch_group_formation() != 0;
            hopper_data.rematch_timer = hopper->get_rematch_countdown_timer() + 1;
            hopper_data.team_game = hopper->is_team_based();
            hopper_data.trueskill_draw_probability = hopper->get_trueskill_draw_probability();
            hopper_data.trueskill_beta_performance_variation = hopper->get_scaled_trueskill_beta_performance_variation();
            hopper_data.trueskill_tau_dynamics_factor = hopper->get_scaled_trueskill_tau_dynamics_factor();
            hopper_data.experience_base_increment = hopper->get_experience_points_per_win();
            hopper_data.experience_penalty_decrement = hopper->get_experience_penalty_per_drop();
            hopper_data.require_hosts_on_multiple_teams = hopper->require_hosts_on_multiple_teams();
            hopper_data.maximum_party_size = hopper->get_maximum_party_size();
            hopper_data.repeated_opponents_to_consider_for_penalty = hopper->get_repeated_opponents_to_consider_for_penalty();
            hopper_data.repeated_opponents_experience_threshold = hopper->get_repeated_opponents_experience_threshold();
            hopper_data.repeated_opponents_skill_throttle_start = hopper->get_repeated_opponents_skill_throttle_start();
            hopper_data.repeated_opponents_skill_throttle_stop = hopper->get_repeated_opponents_skill_throttle_stop();
            hopper_data.stats_write = hopper->m_configuration.runtime_stats_write;
            hopper_data.language_filter = hopper->m_configuration.runtime_language_filter;
            hopper_data.country_code_filter = hopper->m_configuration.runtime_country_code_filter;
            hopper_data.gamerzone_filter = hopper->m_configuration.runtime_gamerzone_filter;
        }
        else
        {
            hopper_data.hopper_identifier = 0xFFFF;
            hopper_data.xlast_index = 0;
            hopper_data.is_ranked = false;
            hopper_data.veto_allowed = false;
            hopper_data.rematch_allowed = false;
            hopper_data.team_game = false;
            hopper_data.trueskill_draw_probability = 0;
            hopper_data.trueskill_beta_performance_variation = 0.0f;
            hopper_data.trueskill_tau_dynamics_factor = 0.0f;
            hopper_data.experience_base_increment = 0;
            hopper_data.experience_penalty_decrement = 0;
            hopper_data.require_hosts_on_multiple_teams = false;
            hopper_data.maximum_party_size = 1;
            hopper_data.rematch_timer = 0;
            hopper_data.repeated_opponents_to_consider_for_penalty = 0;
            hopper_data.repeated_opponents_experience_threshold = 0;
            hopper_data.repeated_opponents_skill_throttle_start = 0;
            hopper_data.repeated_opponents_skill_throttle_stop = 0;
        }
        parameters->matchmaking_hopper.set(&hopper_data);
    }

    if (load_status == 2)
    {
        if (!life_cycle_handler_pre_game_all_player_experience_ratings_are_set(membership))
            load_status = 1;
    }

    if (session_hopper_list == NULL ||
        session_hopper_list->hopper_load_status != load_status ||
        session_hopper_list->membership_update_number != membership->m_current_membership.update_number)
    {
        s_network_session_matchmaking_hopper_list hopper_list;
        memset(&hopper_list, 0, sizeof(hopper_list));
        hopper_list.hopper_load_status = load_status;
        hopper_list.membership_update_number = membership->m_current_membership.update_number;
        if (load_status == 2)
        {
            s_network_session_matchmaking_hopper_description *hopper_entry = hopper_list.hoppers;
            const c_network_session_membership *session_membership = squad_session->get_session_membership();
            s_network_session_matchmaking_hopper_category *category_entry = hopper_list.categories;
            for (long category_index = 0; category_index < NUMBEROF(hopper_list.categories); category_index++)
            {
                const s_network_session_matchmaking_hopper_category *category =
                    life_cycle_handler_pre_game_get_category_from_index(category_index);
                if (category == NULL)
                    break;
                category_entry->identifier = category->identifier;
                memcpy(category_entry->name, category->name,
                    life_cycle_handler_pre_game_min<unsigned int>(sizeof(category_entry->name), strlen(category->name) + 1));
                category_entry->hopper_count = category->hopper_count;
                category_entry++;
                hopper_list.category_count++;
            }
            for (long hopper_index = 0; hopper_index < NUMBEROF(hopper_list.hoppers); hopper_index++)
            {
                unsigned short hopper_identifier = life_cycle_handler_pre_game_get_hopper_identifier(hopper_index);
                if (hopper_identifier == 0xFFFF)
                    break;
                const c_hopper_configuration *hopper_configuration = multiplayer_game_hoppers_get_hopper_configuration(hopper_identifier);
                long hopper_category = hopper_configuration->get_hopper_category();
                hopper_entry->identifier = hopper_identifier;
                hopper_entry->category_identifier = (short)hopper_category;
                hopper_entry->rich_presence_context_id = hopper_configuration->get_rich_presence_context_id();
                hopper_entry->image_index = hopper_configuration->get_image_index();
                hopper_entry->is_visible = multiplayer_game_hopper_is_hopper_visible(hopper_identifier, session_membership);
                hopper_entry->is_playable = multiplayer_game_is_playable(hopper_identifier, true, true, session_membership, NULL) ==
                    _session_game_start_error_none;
                hopper_entry->is_ranked = hopper_configuration->is_ranked();
                hopper_entry->experience_penalty_enabled = hopper_configuration->get_experience_penalty_per_drop() > 0;
                memcpy(hopper_entry->name, hopper_configuration->get_name(),
                    ((strlen(hopper_configuration->get_name()) + 1) > 32) ? 32 : (strlen(hopper_configuration->get_name()) + 1));
                if (hopper_entry->is_visible)
                    hopper_list.category_populated_mask |= FLAG(hopper_category);
                hopper_entry++;
                hopper_list.hopper_count++;
            }
        }
        parameters->matchmaking_hopper_list.set(&hopper_list);
    }
}

static __forceinline void life_cycle_handler_pre_game_add_peer_players(const s_network_session_peer *peer, unsigned short *player_mask)
{
    for (long user_index = 0; user_index < 4; user_index++)
    {
        if (peer->user_player_indices[user_index] != NONE)
            *player_mask |= FLAG(peer->user_player_indices[user_index]);
    }
}

// @address 0x3e0068 ?squad_game_start_status_update@c_life_cycle_state_handler_pre_game@@AAAXXZ
void c_life_cycle_state_handler_pre_game::squad_game_start_status_update()
{
    c_network_session *squad_session = get_manager()->get_active_squad_session();
    const c_network_session_membership *membership = squad_session->get_session_membership();
    c_network_session_parameters::s_network_session_parameters_internal *parameters = &squad_session->get_session_parameters()->m_parameters;
    long host_peer_index = membership->m_current_membership.host_peer_index;
    long leader_peer_index = membership->m_current_membership.leader_peer_index;
    bool is_live = squad_session->m_session_class == 2;
    bool is_matchmaking = network_squad_session_get_start_mode() == 2;
    bool map_precaching = false;
    unsigned short hopper_identifier = 0xFFFF;
    e_campaign_id campaign_id = _campaign_id_none;
    e_map_id map_id = _map_id_none;
    const s_network_session_parameter_game_start_status *current_status = NULL;
    unsigned long peer_mismatch_mask = 0;
    unsigned long host_player_mask = 0;
    unsigned long guest_mask = 0;
    unsigned long non_content_creator_mask = 0;

    if (parameters->game_start_status.get_allowed())
        current_status = parameters->game_start_status.get();

    for (long player_index = 0; player_index < 16; player_index++)
    {
        if (!membership->is_player_valid(player_index))
            continue;
        const s_network_session_player *player = &membership->m_current_membership.players[player_index];
        if (!network_observer_is_peer_joining() && !membership->m_current_membership.peers[player->peer_index].properties.has_gamestate_bonus)
            peer_mismatch_mask |= FLAG(player_index);
        if (!network_observer_is_peer_joining() && !player->configuration.client.is_content_creator)
            non_content_creator_mask |= FLAG(player_index);
        if (player->peer_index == host_peer_index && player->peer_index != leader_peer_index && !host_player_mask)
            host_player_mask = FLAG(player_index);
        if (!player->configuration.client.guest)
            guest_mask |= FLAG(player_index);
    }

    s_network_session_parameter_game_start_status status;
    memset(&status, 0, sizeof(status));
    if (is_matchmaking)
        hopper_identifier = life_cycle_handler_pre_game_get_current_hopper_identifier();
    if (is_live && !online_local_xuid_is_online_enabled())
        status.error = 49;

    if (non_content_creator_mask)
    {
        status.error = 62;
        status.player_error_mask = (unsigned short)non_content_creator_mask;
    }
    else if (status.error == 0)
    {
        status.error = multiplayer_game_is_playable(hopper_identifier, is_live, is_matchmaking, membership, &status.player_error_mask);
        if (status.error == 0)
        {
            const s_network_session_matchmaking_hopper_list *hopper_list;
            if (is_live && is_matchmaking && ((hopper_list = parameters->matchmaking_hopper_list.get()) == NULL ||
                hopper_list->hopper_load_status != 2))
            {
                status.error = 10;
            }
            else if (network_squad_session_get_start_mode() == 0)
            {
                status.error = 11;
            }
            else if (network_squad_session_get_start_mode() == 3)
            {
                const s_saved_film_description *film = user_interface_game_settings_get_film();
                long maximum_theater_player_count = g_network_configuration.maximum_theater_player_count;
                if (!network_observer_is_peer_joining() && guest_mask)
                {
                    status.error = 57;
                    status.player_error_mask = (unsigned short)guest_mask;
                }
                else if (membership->m_current_membership.peer_count > 1
                    && membership->m_current_membership.player_count > maximum_theater_player_count)
                {
                    status.error = 52;
                }
                else if (!network_observer_is_peer_joining() && peer_mismatch_mask)
                {
                    status.error = 53;
                    status.player_error_mask = (unsigned short)peer_mismatch_mask;
                }
                else if (host_peer_index != leader_peer_index)
                {
                    status.error = 54;
                    status.player_error_mask = (unsigned short)host_player_mask;
                }
                else
                {
                    if (film != NULL && film->film_type != 0)
                    {
                        if (film->film_type == 6 ||
                            !user_interface_session_parameters_are_valid_for_ui_game_mode(squad_session->get_session_parameters()))
                            status.error = 14;
                    }
                    else
                    {
                        status.error = 15;
                    }
                    if (status.error == 0 && film->campaign_id != _campaign_id_none)
                    {
                        for (long peer_index = membership->get_first_peer(); peer_index != NONE; peer_index =
                            membership->get_next_peer(peer_index))
                        {
                            const s_network_session_peer *peer = &membership->m_current_membership.peers[peer_index];
                            if (peer->player_count > LIFE_CYCLE_PRE_GAME_MAXIMUM_CAMPAIGN_LOCAL_PLAYERS)
                            {
                                status.error = 18;
                                life_cycle_handler_pre_game_add_peer_players(peer, &status.player_error_mask);
                            }
                        }
                    }
                    if (status.error == 0 && !network_observer_is_peer_joining() && parameters->saved_film_game_options.get_allowed())
                    {
                        long determinism_version = ((const
                            c_generic_network_session_parameter<s_network_session_parameter_saved_film_game_options>
                                *)&parameters->saved_film_game_options)->get()->options.determinism_version;
                        unsigned long incompatible_mask = 0;
                        for (long player_index = membership->get_first_player(); player_index != NONE; player_index =
                            membership->get_next_player(player_index))
                        {
                            const s_network_session_peer *peer =
                                &membership->m_current_membership.peers[membership->m_current_membership.players[player_index].peer_index];
                            if (determinism_version < peer->properties.determinism_compatible_version ||
                                determinism_version > peer->properties.determinism_version)
                                incompatible_mask |= FLAG(player_index);
                        }
                        if (incompatible_mask)
                        {
                            status.error = 55;
                            status.player_error_mask = (unsigned short)incompatible_mask;
                        }
                    }
                }
            }
            else if (!is_matchmaking)
            {
                if (!network_life_cycle_session_get_map(&campaign_id, &map_id))
                {
                    status.error = 11;
                }
                else
                {
                    if (guest_mask)
                    {
                        const c_map_variant *ui_map_variant = (const c_map_variant *)user_interface_game_settings_try_and_get_map_variant();
                        if (ui_map_variant != NULL && !ui_map_variant->m_built_in)
                        {
                            status.error = 57;
                            status.player_error_mask = (unsigned short)guest_mask;
                        }
                    }
                    if (status.error == 0)
                    {
                        if (campaign_id != _campaign_id_none)
                        {
                            if (!network_observer_is_peer_joining() && membership->m_current_membership.peer_count > 1 &&
                                !g_network_configuration.allow_mismatched_peer_properties)
                            {
                                for (long peer_index = membership->get_first_peer(); peer_index != NONE; peer_index =
                                    membership->get_next_peer(peer_index))
                                {
                                    const s_network_session_peer *peer = &membership->m_current_membership.peers[peer_index];
                                    if (!peer->properties.has_gamestate_bonus)
                                    {
                                        status.error = 58;
                                        life_cycle_handler_pre_game_add_peer_players(peer, &status.player_error_mask);
                                    }
                                }
                            }
                            if (status.error == 0 && !network_observer_is_peer_joining() &&
                                membership->m_current_membership.peer_count > 1 && g_network_configuration.require_matching_peer_properties)
                            {
                                unsigned char host_property =
                                    membership->m_current_membership.peers[membership->m_current_membership.host_peer_index]
                                        .properties.has_gamestate_bonus;
                                for (long peer_index = membership->get_first_peer(); peer_index != NONE; peer_index =
                                    membership->get_next_peer(peer_index))
                                {
                                    const s_network_session_peer *peer = &membership->m_current_membership.peers[peer_index];
                                    if (peer->properties.has_gamestate_bonus != host_property)
                                    {
                                        status.error = 59;
                                        life_cycle_handler_pre_game_add_peer_players(peer, &status.player_error_mask);
                                    }
                                }
                            }
                            long player_count = membership->m_current_membership.player_count;
                            for (long peer_index = membership->get_first_peer(); peer_index != NONE; peer_index =
                                membership->get_next_peer(peer_index))
                            {
                                const s_network_session_peer *peer = &membership->m_current_membership.peers[peer_index];
                                if (peer->player_count > LIFE_CYCLE_PRE_GAME_MAXIMUM_CAMPAIGN_LOCAL_PLAYERS)
                                {
                                    status.error = 18;
                                    life_cycle_handler_pre_game_add_peer_players(peer, &status.player_error_mask);
                                }
                            }
                            if (status.error == 0)
                            {
                                if (player_count < 0 || player_count > g_network_configuration.maximum_player_count)
                                    status.error = 19;
                                else if (!network_observer_is_peer_joining() &&
                                    network_session_membership_get_common_determinism_version(membership) == NONE)
                                    status.error = 20;
                            }
                        }
                        else
                        {
                            const c_game_variant *game_variant = user_interface_game_settings_get_game_variant();
                            if (game_variant != NULL && TEST_BIT(game_variant->flags, 0))
                            {
                                unsigned long team_mask = 0;
                                long team_count = 0;
                                long maximum_teams = 8;
                                s_datum level;
                                if (levels_try_and_get_datum(g_level_globals.multiplayer_levels, map_id, &level))
                                {
                                    long game_engine_index = game_variant->m_game_engine_index;
                                    if ((game_engine_index >= 1 ? game_engine_index <= 10 : false))
                                    {
                                        maximum_teams = level.maximum_teams[game_engine_index];
                                        if (maximum_teams <= 1)
                                            maximum_teams = 1;
                                    }
                                }
                                for (long player_index = membership->get_first_player(); player_index != NONE;
                                    player_index = membership->get_next_player(player_index))
                                {
                                    long team_index = membership->m_current_membership.players[player_index].configuration.host.team_index;
                                    if ((team_index >= 0 ? team_index < 16 : false) && (team_mask & FLAG(team_index)) == 0)
                                    {
                                        team_mask |= FLAG(team_index);
                                        team_count++;
                                    }
                                }
                                if (team_count > maximum_teams)
                                    status.error = 16;
                            }
                            if (parameters->ui_game_mode.get_allowed() && *parameters->ui_game_mode.get() == 3 &&
                                membership->m_current_membership.player_count > g_network_configuration.maximum_map_editor_player_count)
                            {
                                status.error = 56;
                            }
                        }
                    }
                }
            }
        }
    }

    if (!is_matchmaking && status.error == 0 && !network_observer_is_peer_joining())
    {
        if (!network_life_cycle_session_get_map(&campaign_id, &map_id))
        {
            status.error = 11;
        }
        else if (map_id == _map_id_none)
        {
            status.error = 11;
        }
        else
        {
            long progress = 0;
            unsigned long player_mask = 0;
            switch (network_life_cycle_session_get_map_load_status(map_id, &progress, &player_mask, 0))
            {
            case 1:
                status.error = 12;
                status.player_error_mask = (unsigned short)player_mask;
                break;
            case 0:
            case 2:
                map_precaching = true;
                status.player_error_mask = 0;
                status.error = 13;
            case 3:
            case 4:
                status.map_load_progress = (short)PIN(progress, 0, 100);
                break;
            }
        }
    }

    if (status.error != 0)
        status.status = 7;
    else if (map_precaching)
        status.status = 4;
    else if (network_observer_is_peer_joining())
        status.status = 3;
    else
        status.status = network_squad_session_get_countdown_timer() >= 0 ? 6 : 5;

    if (current_status == NULL || memcmp(current_status, &status, sizeof(status)) != 0)
        parameters->game_start_status.set(&status);
}
