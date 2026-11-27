// networkinglibxbox/life_cycle_handler_in_game.obj: from the records 0x3e1210 0x3e1918 0x3e1b80 0x3e1c30 0x3e1d90 0x3e1ed8 0x3e2898
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "simulation/simulation_queue_global_events.h"
#include "cseries/crt_xdk.h"
#include "interface/user_interface_session.h"
#include "game/game_types.h"
#include "networking/session/network_session_boot_reason_types.h"
#include "simulation/simulation_queue_global_event_types.h"
#include "game/game_variant.h"
#include "game/map_variant.h"
#include "game/game_options.h"
#include <string.h>
#include "cseries/cseries.h"
#include "game/game.h"
#include "networking/logic/network_session_interface.h"
#include "simulation/simulation.h"
#include "main/main_game.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_join_types.h"
#include "networking/session/network_session.h"
#include "game/game_time.h"
#include "main/levels.h"
#include "networking/session/network_session_membership.h"
#include "networking/logic/life_cycle/life_cycle_handler_pre_game.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session_parameters_session.h"
#include "saved_games/saved_film.h"
#include "networking/logic/life_cycle/life_cycle_manager.h"
#include "interface/user_interface_error_manager.h"
#include "networking/logic/life_cycle/life_cycle_handler_in_game.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
enum e_controller_index;
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "networking/logic/life_cycle_handler_in_game_services.h"
#include "simulation/simulation_globals.h"
#include "networking/network_time_types.h"
#include "networking/network_configuration_types.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/logic/life_cycle/life_cycle_runtime_globals.h"
#include "networking/logic/life_cycle/life_cycle_session.h"
#include "networking/network_configuration.h"
#include "simulation/simulation_abort.h"
#include "saved_games/saved_film_description.h"
#include "networking/session/network_session_parameter_variant_types.h"
#include "saved_games/saved_film_globals.h"
class c_network_session_parameters;
class c_network_session;
namespace {
static __forceinline bool main_game_change_in_progress() {
    return main_game_globals.game_change_in_progress;
}
static __forceinline e_network_session_mode session_mode(const c_network_session *session) {
    return session->get_session_parameters()->m_parameters.session_mode.get();
}
}

static __forceinline const s_network_session_parameter_map *life_cycle_in_game_parameter_get(
    const c_generic_network_session_parameter<s_network_session_parameter_map> *parameter) {
    const s_network_session_parameter_map *result = 0;
    if (parameter->get_allowed())
        result = &parameter->m_data;
    return result;
}

static __forceinline bool life_cycle_in_game_options_available() {
    game_globals_storage *globals = tls_game_globals();
    return globals && (globals->initializing || globals->map_active);
}
static __forceinline const game_options *life_cycle_in_game_options_get() {
    return &tls_game_globals()->options;
}
static __forceinline const game_options *life_cycle_in_game_options_try_get() {
    return life_cycle_in_game_options_available() ? life_cycle_in_game_options_get() : 0;
}

// @address 0x808560 g_simulation_globals
// @address 0xf11308 g_network_life_cycle_globals
// @address 0x948f78 main_globals
// @address 0x94e3b0 g_saved_film_timer_globals
// @address 0x8c447c g_network_time_globals
struct s_networking_join_data;
struct s_network_configuration;
static __forceinline bool life_cycle_in_game_in_squad_session(c_network_session **session) {
    bool result = false;
    if (g_network_life_cycle_globals.initialized) {
        c_network_session *current = g_network_life_cycle_globals.manager.m_active_squad_session;
        if (current->current_local_state() != 0) {
            *session = current;
            result = true;
        }
    }
    return result;
}
static __forceinline unsigned long life_cycle_in_game_network_time_get() {
    if (g_network_time_globals.enabled) return g_network_time_globals.time;
    return GetTickCount();
}
static __forceinline long life_cycle_in_game_network_time_since(unsigned long time) {
    return life_cycle_in_game_network_time_get() - time;
}

struct s_game_matchmaking_options;


static __forceinline bool life_cycle_in_game_game_finished() {
    const game_globals_storage *game = tls_game_globals();
    return game->game_finished != 0 && game->finish_ticks == 0;
}

// @address 0x3e1210 ?update@c_life_cycle_state_handler_in_game@@UAAXXZ
void c_life_cycle_state_handler_in_game::update()
{
    bool host_end = false;
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *session = manager->get_active_squad_session();
    c_network_session_parameters *params = session->get_session_parameters();
    c_network_session_membership *membership = session->get_session_membership_for_update();
    if ((m_flags & 1) != 0) {
        if (!main_game_change_in_progress()) {
            if (!::user_interface_session_parameters_are_valid_for_ui_game_mode(
                    get_manager()->get_active_squad_session()->get_session_parameters()))
            {
                if (session->is_host())
                    host_end = true;
            }
            else if (life_cycle_in_game_initialize_options_from_session(session, 0)) {
                m_flags.set(0, false);
            }
            else {
                host_end = true;
            }
        }
    }
    else if (m_match_start_time == -1) {
        m_match_start_time = end_load_map(session);
    }
    else {
        life_cycle_precache_mainmenu_after_game(this);
        bool aborting = g_simulation_globals.initialized && g_simulation_globals.aborted;
        if (aborting) {
            if (!life_cycle_handle_simulation_abort(session, &m_x41, &m_index))
                host_end = true;
        }
        else {
            if (simulation_starting_up() && session->is_host()) {
                bool handled = false;
                bool coop = game_is_cooperative();
                float elapsed;
                if ((!coop && life_cycle_in_game_game_finished()) ||
                    (!coop && tls_game_globals()->options.game_mode == 1 && tls_game_globals()->game_finished != 0) ||
                    (saved_film_try_and_get_timer_elapsed(&elapsed) && elapsed >= 4.0f)) {
                    params->m_parameters.session_mode.set(_network_session_mode_end_game);
                    handled = true;
                }
                else if (!game_is_cooperative() && tls_game_globals()->options.game_mode == 1) {
                    bool timeout = false;
                    if (m_time == 0 || life_cycle_in_game_network_time_since(m_time) > 2000)
                        timeout = true;
                    if (timeout && params->m_parameters.request_campaign_quit.get_allowed()) {
                        const s_network_session_parameter_request_campaign_quit *request_campaign_quit = params->m_parameters.request_campaign_quit.get();
                        if (request_campaign_quit->quit_request_type != 0) {
                            bool solo = false;
                            if (game_time_is_paused() && membership->get_peer_count() == 1 && membership->get_player_count() > 1)
                                solo = true;
                            if (::simulation_in_progress() && !solo)
                                simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_main_game_lost);
                            else
                                main_globals.shutdown_requested = 1;
                            network_session_request_parameter_set_value(&params->m_parameters.matchmaking_messaging, 4);
                            m_time = life_cycle_in_game_network_time_get();
                            handled = true;
                        }
                    }
                }
                else {
                    if (params->m_parameters.end_game.get_allowed()) {
                        const bool *end_game = params->m_parameters.end_game.get();
                        handled = *end_game;
                    }
                    if (handled)
                        simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_main_game_won);
                }
                if (!handled) {
                    unsigned long peer_mask = 0;
                    long peer_index = membership->get_first_peer();
                    if (peer_index != -1) {
                        do {
                            const s_network_session_peer *peer = membership->get_peer(peer_index);
                            if (((peer->properties.status_flags.get_unsafe() >> 16) & 1) != 0 &&
                                m_match_start_time != -1 && peer->properties.id == (unsigned __int64)m_match_start_time)
                                peer_mask |= (unsigned long)(1 << peer_index);
                            peer_index = membership->get_next_peer(peer_index);
                        } while (peer_index != -1);
                        if (peer_mask != 0) {
                            for (long i = 0; i < 16; i++) {
                                if (i != membership->local_peer_index() && ((1 << i) & peer_mask) != 0)
                                    session->host_boot_machine(i, _network_session_boot_reason_post_match_left_arbitrated_session);
                            }
                        }
                    }
                }
            }
            if (simulation_starting_up()) {
                const game_globals_storage *game = tls_game_globals();
                if (game->options.game_instance == (unsigned __int64)m_match_start_time && params->m_parameters.map.get_allowed()) {
                    const s_network_session_parameter_map *map_value = life_cycle_in_game_parameter_get(&params->m_parameters.map);
                    const game_options *options = &game->options;
                    if (options->campaign_id != map_value->campaign_id || options->map_id != map_value->map_id) {
                        m_match_start_time = -1;
                        m_flags.set(0, true);
                    }
                }
            }
        }
    }
    if (session->is_host()) {
        e_campaign_id campaign_id = _campaign_id_none;
        e_map_id map_id = _map_id_none;
        const s_network_session_parameter_game_start_status *current = 0;
        if (params->m_parameters.game_start_status.get_allowed())
            current = params->m_parameters.game_start_status.get();
        s_network_session_parameter_game_start_status status;
        memset(&status, 0, sizeof(status));
        if (network_life_cycle_session_get_map(&campaign_id, &map_id) && map_id != _map_id_none) {
            long progress = 0;
            unsigned long player_mask = 0;
            switch (network_life_cycle_session_get_map_load_status(map_id, &progress, &player_mask, 0)) {
            case 1:
                status.error = 12;
                status.player_error_mask = (short)player_mask;
                break;
            case 0:
            case 2:
                status.player_error_mask = 0;
                status.error = 13;
            case 3:
            case 4:
                status.map_load_progress = (short)PIN(progress, 0, 100);
                break;
            }
        }
        if (status.error != 0)
            status.status = 7;
        if (current == 0 || memcmp(current, &status, sizeof(status)) != 0)
            params->m_parameters.game_start_status.set(&status);
    }
    if (host_end) {
        if (simulation_starting_up() && tls_game_globals()->options.game_mode == 1)
            main_globals.shutdown_requested = 1;
        if (!session->is_host())
            session->disband_and_reestablish_as_host(false);
        params->m_parameters.session_mode.set(_network_session_mode_end_game);
        manager->request_state_change(_life_cycle_state_end_game_write_stats, 0, 0);
    }
}

// @address 0x3e1918 life_cycle_handle_simulation_abort
extern "C" bool life_cycle_handle_simulation_abort(c_network_session *session, bool *error_posted, unsigned long *start_time) {
    bool result = true;
    if (!*error_posted) {
        switch (g_simulation_globals.abort_reason) {
        case 6:
            if (simulation_starting_up() && game_is_cooperative() && !game_is_film_playback_active())
                (&g_gui_error_manager)
                    ->post_error_with_custom_message(
                        0x20059, (wchar_t const *)(0), (wchar_t const *)(0), k_any_controller, false, true);
            break;
        case 7:
            if (simulation_starting_up() && game_is_cooperative())
                (&g_gui_error_manager)
                    ->post_error_with_custom_message(
                        0x2005a, (wchar_t const *)(0), (wchar_t const *)(0), k_any_controller, false, true);
            else
                (&g_gui_error_manager)
                    ->post_error_with_custom_message(
                        0x20033, (wchar_t const *)(0), (wchar_t const *)(0), k_any_controller, false, true);
            break;
        case 9:
            if (simulation_starting_up() && game_is_cooperative())
                (&g_gui_error_manager)
                    ->post_error_with_custom_message(
                        0x2005b, (wchar_t const *)(0), (wchar_t const *)(0), k_any_controller, false, true);
            break;
        }
        *error_posted = true;
    }
    if (!gui_error_manager_find_queued_error(g_gui_error_manager.errors, k_any_controller, 0x20059, 0) &&
        !gui_error_manager_find_queued_error(g_gui_error_manager.errors, k_any_controller, 0x2005a, 0) &&
        !gui_error_manager_find_queued_error(g_gui_error_manager.errors, k_any_controller, 0x2005b, 0)) {
        gui_error_manager_find_queued_error(g_gui_error_manager.errors, k_any_controller, 0x20033, 0);
    }
    long elapsed;
    if (*start_time == ~0u) {
        *start_time = life_cycle_in_game_network_time_get();
        elapsed = 0;
    } else {
        elapsed = life_cycle_in_game_network_time_since(*start_time);
    }
    if (session->is_host()) {
        if (elapsed > g_network_configuration.simulation_abort_host_timeout) result = false;
    } else {
        bool established = (unsigned long)session->current_local_state() >= 4 && (unsigned long)session->current_local_state() < 10;
        if (established && elapsed > g_network_configuration.simulation_abort_peer_timeout) result = false;
    }
    return result;
}
// @address 0x3e1b80 ?update_for_state_transition@c_life_cycle_state_handler_in_game@@UAA?AW4e_life_cycle_state_transition_type@@XZ
e_life_cycle_state_transition_type c_life_cycle_state_handler_in_game::update_for_state_transition() {
    e_life_cycle_state_transition_type transition = _life_cycle_state_transition_none;
    c_life_cycle_state_manager *manager = get_manager();
    c_network_session *session = manager->get_active_squad_session();
    if (main_game_change_in_progress()) {
        transition = _life_cycle_state_transition_continue;
    } else {
        e_network_session_mode mode = session_mode(session);
        switch (mode) {
        case _network_session_mode_in_game:
            transition = _life_cycle_state_transition_continue;
            break;
        default:
            manager->request_state_change(_life_cycle_state_end_game_write_stats, 0, 0);
            transition = _life_cycle_state_transition_change;
            break;
        }
    }
    return transition;
}

static __forceinline bool life_cycle_in_game_game_in_progress() {
    game_globals_storage *globals = tls_game_globals();
    bool result = false;
    if (globals && globals->game_in_progress && !globals->initializing && globals->map_active)
        result = true;
    return result;
}

// @address 0x3e1d90 ?exit@c_life_cycle_state_handler_in_game@@UAAXPAVc_life_cycle_state_handler@@@Z
void c_life_cycle_state_handler_in_game::exit(c_life_cycle_state_handler *to_handler) {
    c_network_session *session;
    if (life_cycle_in_game_in_squad_session(&session)) {
        session->set_close_session_for_matchmaking(false);
        network_squad_session_set_privacy_mode(session, -1, -1, -1, 0);
    }
    if (life_cycle_in_game_game_in_progress() && life_cycle_in_game_options_get()->game_instance == m_match_start_time)
        simulation_abort_immediate(_simulation_abort_reason_exiting_in_game);
    if (to_handler->get_state() != _life_cycle_state_end_game_write_stats && !main_game_change_in_progress())
        main_menu_launch();
    g_simulation_globals.saved_film_dispose_pending = true;
}
// @address 0x3e2898 ?end_load_map@c_life_cycle_state_handler_in_game@@SA_KPAVc_network_session@@@Z
unsigned __int64 c_life_cycle_state_handler_in_game::end_load_map(c_network_session *session) {
    unsigned __int64 result = -1;
    if (!main_game_change_in_progress()) {
        c_network_session_parameters *parameters = session->get_session_parameters();
        const s_network_session_parameter_map *map = parameters->m_parameters.map.get();
        const game_options *options = life_cycle_in_game_options_try_get();
        if (map && options && options->campaign_id == map->campaign_id && options->map_id == map->map_id) {
            result = life_cycle_in_game_options_get()->game_instance;
            bool special = options->game_simulation == 4 || options->game_simulation == 5;
            if (options->game_playback)
                special = false;
            session->set_close_session_for_matchmaking(special);
        } else {
            ((c_network_session *)(session))->disband_and_reestablish_as_host(session->session_type() == 2);
            parameters->m_parameters.session_mode.set(_network_session_mode_end_game);
        }
    }
    return result;
}
// @address 0x3e1c30 ?enter@c_life_cycle_state_handler_in_game@@UAAXPAVc_life_cycle_state_handler@@JPAX@Z
void c_life_cycle_state_handler_in_game::enter(c_life_cycle_state_handler *from_handler, long entry_data_size, void *entry_data) {
    c_life_cycle_state_handler::enter(from_handler, 0, 0);
    m_match_start_time = -1;
    m_flags.clear();
    m_flags.set(0, true);
    m_x41 = false;
    m_time = 0;
    m_index = -1;
    c_network_session *session;
    if (life_cycle_in_game_in_squad_session(&session)) {
        bool valid = false;
        session->set_close_session_for_matchmaking(false);
        c_network_session *squad = 0;
        if (g_network_life_cycle_globals.initialized) {
            c_network_session *current;
            current = g_network_life_cycle_globals.manager.m_active_squad_session;
            if (current->current_local_state() != 0) {
                squad = current;
                valid = true;
            }
        }
        if (valid) {
            const c_network_session_parameter_ui_game_mode *parameter =
                &squad->get_session_parameters()->m_parameters.ui_game_mode;
            if (parameter->get_allowed()) {
                const e_gui_game_mode *mode = parameter->get();
                if (mode != 0 && *mode == 3)
                    network_squad_session_set_privacy_mode(
                        session, -1, -1, -1, g_network_configuration.maximum_map_editor_player_count);
            }
        }
    }
    network_group_session_set_disable_host_election(0);
    if (g_network_join_data.join_queue_mode != _network_join_open_to_join_squad)
        network_join_flush_join_queue();
    g_network_join_data.join_queue_mode = _network_join_open_to_join_squad;
}

static __forceinline bool life_cycle_in_game_load_players_get(
    const c_network_session_parameter_initial_participants *parameter,
    const game_machine_options **out_machines, const game_player_options **out_players) {
    *out_machines = 0;
    *out_players = 0;
    bool result = false;
    if (parameter->get_allowed() && parameter->m_data.valid) {
        *out_machines = &parameter->m_data.machine_options;
        *out_players = parameter->m_data.player_options;
        result = true;
    }
    return result;
}
static __forceinline const s_saved_film_description *life_cycle_in_game_load_film_get(
    const c_generic_network_session_parameter<s_network_session_parameter_saved_film> *parameter) {
    const s_saved_film_description *result = 0;
    if (parameter->get_allowed())
        result = (const s_saved_film_description *)&parameter->m_data;
    return result;
}
static __forceinline const s_network_session_parameter_saved_film_game_options *life_cycle_in_game_load_film_options_get(
    const c_network_session_parameter_saved_film_game_options *parameter) {
    const s_network_session_parameter_saved_film_game_options *result = 0;
    if (parameter->get_allowed())
        result = &parameter->m_data;
    return result;
}
struct s_map_variant_validation;

static __forceinline void life_cycle_in_game_extract_identifier(const s_network_session_peer *peer, unsigned char *identifier)
{
    memcpy(identifier, peer->secure_address.identifier, sizeof(peer->secure_address.identifier));
}

// @address 0x3e1ed8 life_cycle_in_game_initialize_options_from_session
extern "C" bool life_cycle_in_game_initialize_options_from_session(c_network_session *session, s_game_matchmaking_options *matchmaking_options)
{
    bool valid = true;
    c_network_session_parameters::s_network_session_parameters_internal *parameters = &session->get_session_parameters()->m_parameters;
    bool host = false;
    const unsigned long *random_seed = 0;
    const long *determinism_version = 0;
    const unsigned __int64 *game_instance = 0;
    const e_campaign_difficulty_level *campaign_difficulty = 0;
    const long *language = 0;
    const short *metagame_scoring = 0;
    const c_game_variant *game_variant = 0;
    const short *insertion_point = 0;
    unsigned long session_class = session->session_class();
    const long *primary_skulls = 0;
    const c_map_variant *map_variant = 0;
    const long *secondary_skulls = 0;
    const e_network_game_simulation_protocol *simulation_protocol = 0;
    const game_machine_options *machines = 0;
    const game_player_options *players = 0;
    long player_count = 0;
    long peer_count = 0;
    const s_saved_film_description *film = 0;
    game_options options;
    const e_gui_game_mode *game_mode = parameters->ui_game_mode.get();
    if (!game_mode)
        valid = false;
    const s_network_session_parameter_map *map = parameters->map.get();
    if (!map)
        valid = false;
    if (game_mode) {
        if (*game_mode != 4) {
            game_instance = parameters->game_instance.get();
            if (!game_instance) valid = false;
            random_seed = parameters->random_seed.get();
            if (!random_seed) valid = false;
            simulation_protocol = parameters->simulation_protocol.get();
            if (!simulation_protocol) valid = false;
            else if ((long)*simulation_protocol < 0) valid = false;
            else if ((long)*simulation_protocol >= 3) valid = false;
            const game_machine_options *initial_machines = 0;
            const game_player_options *initial_players = 0;
            bool players_available = life_cycle_in_game_load_players_get(&parameters->initial_participants, &initial_machines, &initial_players);
            machines = initial_machines;
            players = initial_players;
            if (!players_available) valid = false;
            if (initial_machines && initial_players) {
                unsigned long player_mask = 0;
                long player = 0;
                const game_player_options *entry = initial_players;
                do {
                    if (entry->valid)
                        player_mask |= 1 << player;
                    ++player;
                    ++entry;
                } while (player < 16);
                if (!initial_machines->valid_machine_mask || !player_mask)
                    valid = false;
            } else valid = false;
        }
        if (*game_mode == 0) {
            language = parameters->language.get();
            determinism_version = parameters->determinism_version.get();
            campaign_difficulty = parameters->campaign_difficulty.get();
            metagame_scoring = parameters->campaign_metagame_scoring.get();
            insertion_point = parameters->campaign_insertion_point.get();
            primary_skulls = parameters->campaign_active_primary_skulls.get();
            secondary_skulls = parameters->campaign_active_secondary_skulls.get();
            if (!language) valid = false;
            else if (!determinism_version) valid = false;
            else if (*determinism_version == -1) valid = false;
            else if (!campaign_difficulty) valid = false;
            else if (!primary_skulls) valid = false;
            else if (!secondary_skulls) valid = false;
            else if (!metagame_scoring) valid = false;
            else if (!insertion_point) valid = false;
        }
    }
    if (parameters->game_variant.get_allowed())
        game_variant = parameters->game_variant.get();
    if (parameters->map_variant.get_allowed())
        map_variant = parameters->map_variant.get();
    if (valid && map->map_id == _map_id_none) valid = false;
    if (valid && !map->scenario_path[0]) valid = false;
    unsigned char local_identifier[6];
    if (valid) {
        const c_network_session_membership *membership = session->get_session_membership();
        host = membership->is_host();
        life_cycle_in_game_extract_identifier(membership->get_local_peer(), local_identifier);
        player_count = membership->get_player_count();
        peer_count = membership->get_peer_count();
    }
    if (valid && game_mode) {
        if (*game_mode == 0) {
            if (player_count > 4) valid = false;
        } else if (*game_mode != 4 && !game_variant) valid = false;
    }
    if (valid && matchmaking_options) {
        if (matchmaking_options->team_count < 0) valid = false;
        if (valid && matchmaking_options->player_count < 0) valid = false;
        if (valid && matchmaking_options->xlast_index >= 32) valid = false;
    }
    if (valid && game_mode) {
        if (*game_mode == 0)
            network_group_session_set_disable_host_election(1);
        if (*game_mode == 4) {
            if (parameters->saved_film.get_allowed()) {
                film = life_cycle_in_game_load_film_get(&parameters->saved_film);
                if (host) {
                    if (!saved_film_initialize_from_name(film->controller_index, film->file_path)) valid = false;
                    if (valid) network_group_session_set_disable_host_election(2);
                }
            } else valid = false;
        }
    }
    if (valid) {
        if (film) {
            short playback_type;
            if (host) playback_type = (1 != peer_count) + 1;
            else playback_type = 3;
            if (parameters->saved_film_game_options.get_allowed()) {
                const s_network_session_parameter_saved_film_game_options *saved
                    = life_cycle_in_game_load_film_options_get(&parameters->saved_film_game_options);
                options = saved->options;
                options.game_playback = playback_type;
                options.record_saved_film = false;
                options.playback_length_in_ticks = saved->length_in_ticks;
                options.playback_start_in_ticks = saved->start_ticks;
                if (host && (g_saved_film_timer_globals.film.get_mode() || !game_options_match(&options, g_saved_film_timer_globals.film.get_options()) ||
                    options.game_instance != g_saved_film_timer_globals.film.get_options()->game_instance
                    || options.random_seed != g_saved_film_timer_globals.film.get_options()->random_seed))
                    valid = false;
            } else valid = false;
        } else {
            long protocol;
            switch (*simulation_protocol) {
            case 0: protocol = 1; break;
            case 1: protocol = host ? 3 : 2; break;
            default: protocol = host ? 5 : 4; break;
            }
            game_options_initialize_default(&options);
            options.campaign_id = map->campaign_id;
            options.map_id = map->map_id;
            strncpy(options.scenario_path, map->scenario_path, sizeof(options.scenario_path));
            options.scenario_path[sizeof(options.scenario_path) - 1] = 0;
            options.game_simulation = protocol;
            switch (session_class) {
            case 0: options.game_network_type = 1; break;
            case 1: options.game_network_type = 2; break;
            case 2: options.game_network_type = 3; break;
            }
            options.game_instance = *game_instance;
            options.random_seed = *random_seed;
            if (*game_mode == 0) {
                options.game_mode = 1;
                short difficulty = (short)*campaign_difficulty;
                short scoring = *metagame_scoring;
                options.survival_mode_enabled = false;
                long lives = *language;
                options.campaign_difficulty = difficulty;
                options.campaign_metagame_scoring = scoring;
                options.language = lives;
                options.determinism_version = *determinism_version;
                options.primary_skulls = *primary_skulls;
                options.secondary_skulls = *secondary_skulls;
                options.campaign_insertion_point = *insertion_point;
                memcpy(&options.campaign_armaments, map->campaign_players, sizeof(options.campaign_armaments));
                options.campaign_metagame_enabled = scoring != 0;
                levels_insertion_datum insertion;
                if (levels_try_and_get_insertion(options.map_id, &insertion) && options.campaign_insertion_point >= 0 &&
                    options.campaign_insertion_point < insertion.insertion_count)
                {
                    long zone_set = (unsigned char)insertion.zone_sets[options.campaign_insertion_point];
                    if (zone_set != -1)
                        options.initial_zone_set_index = (unsigned char)zone_set;
                }
            } else {
                options.game_mode = 2;
                if (game_variant)
                    options.game_variant.copy_from_and_validate(game_variant);
                if (map_variant) {
                    options.map_variant = *map_variant;
                    if (!map_variant_is_valid((const s_map_variant_validation *)&options.map_variant))
                        options.map_variant.create_default((e_map_id)options.map_variant.m_map_id);
                }
                if (matchmaking_options) {
                    options.matchmade_game = true;
                    options.matchmaking_options = *matchmaking_options;
                } else {
                    options.matchmade_game = false;
                    options.matchmaking_options.hopper_identifier = (unsigned short)-1;
                }
            }
            options.record_saved_film = saved_film_can_record(&options);
            options.machine_options = *machines;
            memcpy(options.players, players, sizeof(options.players));
            options.machine_options.local_machine_exists = true;
            memcpy(&options.machine_options.local_machine, local_identifier, sizeof(local_identifier));
        }
    }
    char error_string[512];
    if (valid && game_options_verify(&options, error_string, sizeof(error_string))) {
        game_options_validate_variants(&options);
        main_game_set_pending_options(&options);
        return true;
    }
    return false;
}
