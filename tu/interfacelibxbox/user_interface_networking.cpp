// interfacelibxbox/user_interface_networking.obj: from the records 0x8c3d0 0x3b2110 0x478648 0x478c48 0x478fd0 0x4791e0 0x479260 0x4792b0 0x479300 0x4795f8 0x4796b0 0x479710 0x4797a8 0x4797f8 0x479b58 0x479ce8 0x479d50 0x479de0 0x479ee0 0x479f48 0x47a028 0x47a2a8 0x47a690 0x48c070 0x497c48 0x4c53b8
#include "interface/user_interface_matchmaking_progress.h"
#include "cseries/pointer_utilities.h"
#include "interface/user_interface_dialog_globals.h"
#include "game/game_results_storage_types.h"
#include "interface/gui_widget_tree.h"
#include "interface/user_interface_squad_session.h"
#include "interface/user_interface_sound.h"
#include "interface/gui_network_session_advertisement_types.h"
#include "cseries/crt_xdk.h"
#include "shell/shell.h"
#include <stddef.h>
#include "game/player_types.h"
#include "game/game_types.h"
#include "memory/static_arrays.h"
#include "game/game_results_team_statistics_types.h"
#include "game/game_results_team_data_types.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface_message_globals.h"
#include <stdlib.h>
#include <string.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "simulation/simulation.h"
#include "text/unicode.h"
#include "interface/gui_pregame_setup_manager_types.h"
#include "interface/user_interface_matchmaking.h"
#include "interface/user_interface_networking.h"
#include "memory/tls_globals.h"
#include "networking/logic/network_join.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session_membership.h"
#include "interface/gui_screens/start_menu/gui_screen_start_menu.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/c_controller.h"
#include "networking/session/network_managed_session.h"
#include "saved_games/game_state.h"
#include "interface/user_interface_sound_effect_types.h"
#include "interface/gui_game_mode_types.h"
#include "game/game_options.h"
#include "networking/session/network_session_membership_types.h"
#include "networking/online/online_managed_session_types.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "interface/c_window_manager.h"
#include "interface/user_interface_window_manager.h"
#include "networking/network_time_types.h"
#include "networking/session/network_session_class.h"
#include "game/game_results_types.h"
#include "game/game_results_damage_statistic_types.h"
#include "game/game_results_damage_type_description_types.h"
#include "networking/online/online_globals_types.h"
#include "game/game_results_player_vs_player_statistic_types.h"
#include "game/game_results_damage_breakdown_types.h"
#include "game/game_results_statistic_shared_types.h"
#include "networking/transport/transport_security_types.h"
#include "game/game_results.h"
#include "networking/network_utilities.h"
#include "game/game_results_statistics_types.h"
#include "networking/session/network_session_boot_reason_types.h"
#include "interface/hopper_information_types.h"
#include "game/game_globals_storage.h"
#include "interface/user_interface_networking_externs.h"
#include "networking/logic/life_cycle/life_cycle_runtime_globals.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/network_life_cycle_session_helpers.h"
#include "saved_games/saved_game_item_metadata.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"
#include "interface/session_game_mode_types.h"
#include "interface/user_interface_clock.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/session/network_session_interface_globals_types.h"
#include "interface/user_interface_networking_allocation.h"
#include "game/game_results_finalized.h"
#include <ppcintrinsics.h>
#include "interface/gui_widget.h"
#include "interface/gui_screens/dialog/gui_screen_dialog.h"

enum e_network_game_privacy;
enum e_session_game_start_status;
enum e_session_game_start_error;
typedef char uinet_joining_offset[
    (sizeof(void *) != 4 || ((offsetof(s_life_cycle_globals, joining) +
    offsetof(c_life_cycle_state_handler_joining, m_previous_state) == 0x9a70))) ? 1 : -1];
typedef char uinet_globals_start_state_offset[(offsetof(s_user_interface_networking_globals, start_state) == 0x14) ? 1 : -1];
typedef char uinet_globals_controller_offset[(offsetof(s_user_interface_networking_globals, controller) == 0x28) ? 1 : -1];
typedef char uinet_globals_session_id_offset[(offsetof(s_user_interface_networking_globals, session_id) == 0x50) ? 1 : -1];
typedef char uinet_globals_session_key_offset[(offsetof(s_user_interface_networking_globals, session_key) == 0x7c) ? 1 : -1];
typedef char uinet_globals_show_teams_offset[(offsetof(s_user_interface_networking_globals, show_teams) == 0x8c) ? 1 : -1];
struct s_advert_storage {
    long game_mode;
    char data[0xfa40];
};

struct s_uinl_dialog_id { long value : 32; };
struct s_transition_count_bits { unsigned long value : 32; };
typedef unsigned char (*network_join_to_remote_squad_raw_fn)(bool, unsigned char, bool,
    e_network_session_class, const s_transport_secure_identifier *, const s_transport_secure_key *,
    const s_transport_secure_address *);

// @flags /TP /O2
// @address 0x4797a8 ?user_interface_networking_get_final_game_results@@YAPBVc_game_results@@XZ
// @address 0x3b2110 ?user_interface_networking_get_closed@@YA_NXZ
// @address 0x4791e0 ?user_interface_networking_get_saved_game_controller@@YA?AW4e_controller_index@@XZ
// @address 0x479260 ?user_interface_networking_build_is_compatible@@YA_NJJJ@Z


// @address 0xf11308 g_network_life_cycle_globals

extern "C" {
// @address 0x897588 g_user_interface_networking_globals
s_user_interface_networking_globals g_user_interface_networking_globals;
// @address 0x8976a8 g_user_interface_matchmaking_progress
s_life_cycle_matchmaking_progress g_user_interface_matchmaking_progress = { 0 };
}

// match: 0x8c3d0: start_pending and start_state load at -0x110/-0x10c off the matchmaking-progress base, not +0x10/+0x14
// off the networking globals. match: 0x479de0: show_teams is read and written at -0x94 off the progress base the
// function holds in r30 for the progress copy; naming the global loads a second base and drops the r30 save.
static __forceinline s_user_interface_networking_globals *uinl_networking_globals_from_progress()
{
#if defined(__clang__)
    // The port places each global on its own, so the networking globals are not 0x120 below the progress.
    return &g_user_interface_networking_globals;
#else
    return (s_user_interface_networking_globals *)offset_pointer(&g_user_interface_matchmaking_progress, -0x120);
#endif
}


// match: 0x479b58: enum accessors add a second comparison after the unsigned test.


bool user_interface_networking_get_closed(void)
{
  c_network_session *session = 0;
  bool closed = true;
  if (network_life_cycle_in_squad_session(&session) && session->established()) {
    const c_network_session_parameters *parameters = session->get_session_parameters();
    if (parameters->m_parameters.privacy_mode.m_flags & 1) {
      const s_network_session_privacy_mode *state = parameters->m_parameters.privacy_mode.get();
      closed = state->closed_mode != 0 || state->is_closed_by_user;
    }
  }
  return closed;
}


// @address 0x478fd0 user_interface_networking_is_throttled
extern "C" bool user_interface_networking_is_throttled(void)
{
    bool result = false;
    if (g_message_globals.flag)
        result = true;
    else
    {
        long last_request_time = g_user_interface_networking_globals.last_request_time;
        long now = g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
        if (now - last_request_time < 500)
            result = true;
    }
    return result;
}


// 0x4791e0
e_controller_index user_interface_networking_get_saved_game_controller(void)
{
    e_controller_index result = k_no_controller;
    if (user_interface_squad_get_player_count() == 1) {
        for (long player_index = 0; player_index < 16; player_index++) {
            if (user_interface_squad_is_player_valid(player_index) &&
                user_interface_squad_get_controller_index(player_index) != k_no_controller) {
                result = user_interface_squad_get_controller_index(player_index);
                break;
            }
        }
    }
    return result;
}

bool user_interface_networking_build_is_compatible(long type, long version, long build)
{
  long minimum_version = k_network_compatible_version;
  bool type_matches;
  switch (type) {
  case 5:
    type_matches = true;
    break;
  default:
    type_matches = false;
    break;
  }
  bool version_valid = type_matches && version >= minimum_version;
  return version_valid && build <= 11855;
}


// @address 0x4792b0 user_interface_networking_set_pre_game_state
bool user_interface_networking_set_pre_game_state(void)
{
    bool result = false;
    if (::network_life_cycle_squad_local_peer_is_leader() && network_life_cycle_set_pre_game_state())
        result = true;
    return result;
}


// @address 0x4795f8 ?user_interface_networking_get_session_advertisement@@YA?AW4e_gui_network_session_advertisement_mode@@XZ
e_gui_network_session_advertisement_mode user_interface_networking_get_session_advertisement(void)
{
    e_gui_network_session_advertisement_mode result = _network_advertise_offline;
    if (user_interface_squad_exists()) {
        e_network_session_class session_class = network_squad_session_get_session_class();
        long privacy_mode = ((long)::network_squad_session_get_privacy_mode());
        if (session_class == _network_session_class_xbox_live) {
            if (privacy_mode == 0)
                result = _network_advertise_xbox_live_open;
            else if (privacy_mode == 1)
                result = _network_advertise_xbox_live_friends_only;
            else if (privacy_mode == 2)
                result = _network_advertise_xbox_live_invite_only;
        } else if (session_class == _network_session_class_system_link && privacy_mode == 0) {
            result = _network_advertise_system_link;
        }
    }
    return result;
}


// @address 0x4796b0 ?user_interface_networking_get_current_party_size@@YAJXZ
long user_interface_networking_get_current_party_size(void)
{
    long party_size;
    if (!network_squad_session_get_membership(0, 0, 0, 0, 0, 0, 0, &party_size, 0, 0))
        party_size = 1;
    return party_size;
}

// @address 0x479710 ?user_interface_networking_set_closed@@YA_N_N@Z
bool user_interface_networking_set_closed(bool closed)
{
    bool result = false;
    c_network_session *session = 0;
    if (network_life_cycle_in_squad_session(&session)) {
        network_squad_session_set_privacy_mode(session, -1, -1, closed != 0, -1);
        result = true;
    }
    return result;
}

const c_game_results *user_interface_networking_get_final_game_results()
{
    const c_game_results *result = 0;
    const c_game_results *final_results = game_results_get_final_results();
    if (final_results && final_results->finish_valid)
        result = final_results;
    return result;
}


// @address 0x479ce8 ?user_interface_networking_notify_booted_from_session@@YAXW4e_network_session_type@@W4e_network_session_boot_reason@@@Z
void user_interface_networking_notify_booted_from_session(e_network_session_type, e_network_session_boot_reason reason)
{
    if (reason != _network_session_boot_reason_player_booted_player) {
        if (reason != _network_session_boot_reason_leader_booted)
            return;
        g_gui_error_manager.post_error_with_custom_message(_user_interface_networking_string_booted_from_session, 0, 0, k_any_controller, false, true);
    } else {
        g_gui_error_manager.post_error_with_custom_message(_user_interface_networking_string_booted_from_game, 0, 0, k_any_controller, false, true);
    }
    user_interface_leave_squad(0);
}

// @address 0x479d50 user_interface_networking_post_error_message
extern "C" void user_interface_networking_post_error_message(s_saved_game_item_metadata *error, long string_id)
{
    c_static_wchar_string<1024> global_string;
    c_static_wchar_string<1024> message;
    message.set(error->name);
    user_interface_global_string_get(string_id, (unsigned short *)global_string.m_string);
    message.append(global_string.get_string());
    g_gui_error_manager.post_error_with_custom_message(string_id, 0, message.get_string(), k_any_controller, false, false);
}

// @address 0x479ee0 user_interface_networking_is_team_switching_allowed
extern "C" int user_interface_networking_is_team_switching_allowed(void)
{
    int result = 0;
    if (!network_life_cycle_in_matchmaking() && ((long)::network_life_cycle_squad_session_get_ui_game_mode()) != 4) {
        const c_game_variant *game_variant = user_interface_game_settings_get_game_variant();
        if (game_variant != 0) {
            result = game_variant->flags & 1;
        }
    }
    return result;
}

static __forceinline bool user_interface_networking_join_is_silent()
{
    bool result = false;
    if (g_network_life_cycle_globals.initialized) {
        switch (g_network_life_cycle_globals.manager.m_current_state) {
        case _life_cycle_state_joining:
            result = g_network_life_cycle_globals.joining.m_previous_state == _life_cycle_state_post_match;
            break;
        case _life_cycle_state_post_match:
            result = (_rotl(g_network_life_cycle_globals.post_match.get_flags(), 29) & 1) != 0;
            break;
        }
    }
    return result;
}
// @address 0x47a2a8 ?user_interface_networking_join_update@@YAXXZ
void user_interface_networking_join_update(void)
{
    long join_result = network_join_is_any_join_in_progress();
    if (join_result == _life_cycle_join_result_in_progress) {
        if (!user_interface_networking_join_is_silent())
            g_user_interface_networking_globals.join_in_progress = true;
    } else if (join_result != _life_cycle_join_result_in_progress && g_user_interface_networking_globals.join_in_progress) {
        long desired_alert = -1;
        switch (join_result) {
        case _life_cycle_join_result_failed_to_find_session:
            desired_alert = _user_interface_networking_string_join_failed_failed_to_find_session;
            break;
        case _life_cycle_join_result_not_enough_space:
            desired_alert = _user_interface_networking_string_join_failed_not_enough_space;
            break;
        case _life_cycle_join_result_game_not_open:
            desired_alert = _user_interface_networking_string_join_failed_game_not_open;
            break;
        case _life_cycle_join_result_target_is_matchmaking:
            desired_alert = _user_interface_networking_string_join_failed_target_is_matchmaking;
            break;
        case _life_cycle_join_result_host_timed_out:
            desired_alert = _user_interface_networking_string_join_failed_host_timed_out;
            break;
        case _life_cycle_join_result_peer_version_too_low:
            desired_alert = _user_interface_networking_string_join_failed_peer_version_too_low;
            break;
        case _life_cycle_join_result_host_version_too_low:
            desired_alert = _user_interface_networking_string_join_failed_host_version_too_low;
            break;
        case _life_cycle_join_result_unable_to_connect_open_nat:
            desired_alert = _user_interface_networking_string_join_failed_unable_to_connect_open_nat;
            break;
        case _life_cycle_join_result_unable_to_connect_moderate_nat:
            desired_alert = _user_interface_networking_string_join_failed_unable_to_connect_moderate_nat;
            break;
        case _life_cycle_join_result_unable_to_connect_strict_nat:
            desired_alert = _user_interface_networking_string_join_failed_unable_to_connect_closed_nat;
            break;
        case _life_cycle_join_result_unable_to_connect_party_open_nat:
            desired_alert = _user_interface_networking_string_join_failed_unable_to_connect_party_strict_nat;
            break;
        case _life_cycle_join_result_unable_to_connect_party_moderate_nat:
            desired_alert = _user_interface_networking_string_join_failed_unable_to_connect_party_moderate_nat;
            break;
        case _life_cycle_join_result_unable_to_connect_party_strict_nat:
            desired_alert = _user_interface_networking_string_join_failed_unable_to_connect_party_open_nat;
            break;
        case _life_cycle_join_result_player_not_online_enabled:
            desired_alert = _user_interface_networking_string_join_failed_party_member_not_online_enabled;
            break;
        case _life_cycle_join_result_target_has_film_in_progress:
            desired_alert = _user_interface_networking_string_join_failed_film_in_progress;
            break;
        case _life_cycle_join_result_target_has_campaign:
            desired_alert = _user_interface_networking_string_join_failed_campaign_in_progress;
            break;
        case _life_cycle_join_result_user_content_not_permitted:
            desired_alert = _user_interface_networking_string_join_failed_user_content_not_permitted;
            break;
        default:
            desired_alert = _user_interface_networking_string_join_failed_generic_error;
            break;
        case _life_cycle_join_result_none:
        case _life_cycle_join_result_in_progress:
            break;
        }
        if (desired_alert != -1) {
            g_gui_error_manager.post_error_with_custom_message(desired_alert, 0, 0, k_any_controller, false, true);
            g_user_interface_networking_globals.last_join_error = desired_alert;
        }
        network_join_clear_squad_join_result();
        g_user_interface_networking_globals.join_in_progress = false;
    }
}

static __forceinline void user_interface_networking_get_last_game_setup(s_gui_game_setup_storage *last_game_setup)
{
    memcpy(last_game_setup, global_preferences_get_game_setup_storage(), sizeof(s_gui_game_setup_storage));
}

// @address 0x47a690 user_interface_networking_reset_theater_campaign
extern "C" void user_interface_networking_reset_theater_campaign(void)
{
    // match: 0x47a690: constructs the three mode settings without the outer setup constructor.
    unsigned char last_game_setup_storage[sizeof(s_gui_game_setup_storage)];
    s_gui_game_setup_storage *last_game_setup = reinterpret_cast<s_gui_game_setup_storage *>(last_game_setup_storage);
    last_game_setup->multiplayer_settings.s_gui_game_setup_storage::s_multiplayer_settings::s_multiplayer_settings();
    last_game_setup->map_editor_settings.map_variant.c_map_variant::c_map_variant();
    last_game_setup->theater_settings.options.game_options::game_options();
    user_interface_networking_get_last_game_setup(last_game_setup);
    last_game_setup->theater_settings.campaign_id = -1;
    global_preferences_set_last_game_setup(last_game_setup);
}



static __forceinline long uinl_ui_get_local_user_state(long user)
{
    return g_network_session_interface_globals.users[user].state;
}
static __forceinline void uinl_ui_set_local_user_state(long user, long state)
{
    if ((user != -1 || state != _network_session_interface_user_state_none) &&
        g_network_session_interface_globals.users[user].state != _network_session_interface_user_state_exists)
        g_network_session_interface_globals.users[user].state = state;
}
static __forceinline bool uinl_network_life_cycle_in_target_session(c_network_session **session)
{
    bool result = false;
    if (g_network_life_cycle_globals.initialized) {
        c_network_session *current = g_network_life_cycle_globals.manager.m_target_squad_session;
        if (!current->disconnected()) {
            if (session)
                *session = current;
            result = true;
        }
    }
    return result;
}
#define uinl_user_interface_target_squad_exists(result, session) \
    do { \
        result = false; \
        session = 0; \
        if (uinl_network_life_cycle_in_target_session(&session)) \
            result = session && !((const c_network_session *)session)->leaving_session() && session->session_type() == 1; \
    } while (0)

// @address 0x8c3d0 user_interface_networking_get_current_location
extern "C" long user_interface_networking_get_current_location(void)
{
    long result = -1;
    c_network_session *squad_session = 0;
    if (user_interface_squad_try_and_get_session((s_network_session **)&squad_session)) {
        switch (user_interface_get_session_game_mode()) {
        case _session_game_mode_none:
            break;
        case _session_game_mode_pregame:
            if (uinl_networking_globals_from_progress()->start_pending ||
                uinl_networking_globals_from_progress()->start_state == 2)
                result = -1;
            else
                result = 1;
            break;
        case _session_game_mode_matchmaking:
            {
                s_life_cycle_matchmaking_progress progress;
                network_life_cycle_get_matchmaking_progress(&progress);
                g_user_interface_matchmaking_progress = progress;
                switch (progress.type) {
                case 0:
                    break;
                case 1: case 2: case 3:
                    result = 2;
                    break;
                case _life_cycle_matchmaking_progress_preparing_map:
                    result = 3;
                    break;
                case 5:
                    result = 5;
                    break;
                default:
                    // match: 0x8c3d0: assume keeps the progress-type switch range-check-free; removal adds addi/cmplwi/bgt guards and wider jump tables
                    __assume(0);
                }
            }
            break;
        case _session_game_mode_in_game:
            result = 4;
            break;
        case _session_game_mode_postgame:
            result = 5;
            break;
        default:
            // match: 0x8c3d0: assume keeps the location switch range-check-free; removal adds addi/cmplwi/bgt guards and wider jump tables
            __assume(0);
        }
    } else {
        bool target_squad_exists;
        c_network_session *target_session;
        uinl_user_interface_target_squad_exists(target_squad_exists, target_session);
        if (!target_squad_exists)
            result = 0;
    }
    return result;
}

// @address 0x478c48 user_interface_networking_dialogs_handle_results
extern "C" bool user_interface_networking_dialogs_handle_results(const c_dialog_result_message *message)
{
    bool handled = false;
    bool reset_users = false;
    long next_state = _network_session_interface_user_state_none;
    switch (message->get_dialog_name()) {
    case _user_interface_networking_string_not_signed_in_bring_up_guide:
        if (message->get_dialog_result() != _gui_dialog_choice_first) {
            if (!g_user_interface_networking_globals.sign_in_required)
                reset_users = true;
        } else {
            XShowSigninUI(4, 0x80000);
            g_user_interface_networking_globals.sign_in_requested = true;
        }
        handled = true;
        break;
    case _user_interface_networking_string_not_signed_in_bring_up_guide_or_use_temporary_account:
        switch (message->get_dialog_result()) {
        case _gui_dialog_choice_first:
            XShowSigninUI(4, 0x80000);
            g_user_interface_networking_globals.sign_in_requested = true;
            break;
        case _gui_dialog_choice_second: {
            long user = controller_get_or_allocate_user_index(controller_get(message->get_controller()));
            uinl_ui_set_local_user_state(user, _network_session_interface_user_state_error_confirm_temporary);
            break;
        }
        default:
            if (!g_user_interface_networking_globals.sign_in_required)
                reset_users = true;
            break;
        }
        handled = true;
        break;
    case _user_interface_networking_string_confirm_the_use_of_a_temporary_account:
        if (message->get_dialog_result() == _gui_dialog_choice_first)
            (controller_get(message->get_controller()))->set_as_unsigned_in_user(true);
        reset_users = true;
        handled = true;
        break;
    case _user_interface_networking_string_sign_in_failed_online_permissions_1_player:
    case _user_interface_networking_string_sign_in_failed_online_permissions_2_player:
    case _user_interface_networking_string_sign_in_failed_online_permissions_3_player:
    case _user_interface_networking_string_sign_in_failed_online_permissions_4_player:
        if (message->get_dialog_result() != _gui_dialog_choice_second) {
            XShowSigninUI(4, 0x80002);
            g_user_interface_networking_globals.sign_in_requested = true;
        } else {
            user_interface_networking_set_session_advertisement(4);
            reset_users = true;
            next_state = _network_session_interface_user_state_exists;
        }
        handled = true;
        break;
    }
    if (handled == true) {
        if (reset_users) {
            for (long user = 0; user < 4; ++user) {
                if (uinl_ui_get_local_user_state(user) == g_user_interface_networking_globals.local_user_state)
                    uinl_ui_set_local_user_state(user, next_state);
            }
        }
        g_user_interface_networking_globals.dialog = -1;
        g_user_interface_networking_globals.local_user_state = _network_session_interface_user_state_none;
        g_user_interface_networking_globals.controller = -1;
        g_user_interface_networking_globals.sign_in_required = false;
    }
    return handled;
}

static __forceinline bool uinl_advert_life_cycle_in_squad_session(c_network_session **session)
{
    bool result = false;
    if (g_network_life_cycle_globals.initialized) {
        if (!g_network_life_cycle_globals.manager.m_active_squad_session->disconnected()) {
            if (session)
                *session = g_network_life_cycle_globals.manager.m_active_squad_session;
            result = true;
        }
    }
    return result;
}
static __forceinline long uinl_advert_life_cycle_mode()
{
    long result = 0;
    if (g_network_life_cycle_globals.initialized)
        result = g_network_life_cycle_globals.manager.m_current_state;
    return result;
}
static __forceinline bool uinl_advert_session_established(c_network_session *session)
{
    bool result = false;
    switch (session->current_local_state()) {
    case _network_session_state_none:
    case _network_session_state_peer_creating:
    case _network_session_state_peer_joining:
    case _network_session_state_peer_join_abort:
    case _network_session_state_election:
        result = false;
        break;
    case _network_session_state_peer_established:
    case _network_session_state_peer_leaving:
    case _network_session_state_host_established:
    case _network_session_state_host_disband:
    case _network_session_state_host_handoff:
    case _network_session_state_host_reestablish:
        result = true;
        break;
    default:
        // match: 0x479300: assume keeps the session-state switch a compare chain; removal lowers it to a range-checked jump table
        __assume(0);
    }
    return result;
}
#define uinl_advert_change_session_class(result) \
    do { \
        result = false; \
        c_network_session *session = 0; \
        if (uinl_advert_life_cycle_in_squad_session(&session)) { \
            c_network_session *advert = (c_network_session *)session; \
            if (advert->m_session_class != 0) { \
                long index = advert->m_managed_session_index; \
                advert->m_session_class = _network_session_class_offline; \
                s_online_managed_session *managed = &online_session_manager_globals.sessions[index]; \
                managed->flags |= 8; \
                ((c_network_session *)advert)->disband_and_reestablish_as_host(false); \
            } \
            result = true; \
        } \
    } while (0)
#define uinl_advert_set_privacy(result, privacy) \
    do { \
        c_network_session *session = 0; \
        result = false; \
        if (uinl_advert_life_cycle_in_squad_session(&session) && uinl_advert_session_established(session)) { \
            network_squad_session_set_privacy_mode(session, privacy, -1, -1, -1); \
            result = true; \
        } \
    } while (0)

// @address 0x479300 user_interface_networking_set_session_advertisement
extern "C" bool user_interface_networking_set_session_advertisement(long advertisement)
{
    bool success = false;
    if (user_interface_squad_exists() && ::network_life_cycle_squad_local_peer_is_leader()) {
        long old_class = network_squad_session_get_session_class();
        long new_class = -1;
        long old_privacy = ((long)::network_squad_session_get_privacy_mode());
        long new_privacy = 0;
        success = true;
        switch (advertisement) {
        case _network_advertise_xbox_live_open: new_class = 2; new_privacy = 0; break;
        case _network_advertise_xbox_live_friends_only: new_class = 2; new_privacy = 1; break;
        case _network_advertise_xbox_live_invite_only: new_class = 2; new_privacy = 2; break;
        case _network_advertise_system_link: new_class = 1; new_privacy = 0; break;
        case _network_advertise_offline: new_class = 0; new_privacy = 0; break;
        }
        if (success && new_class != old_class) {
            s_advert_storage storage;
            storage.game_mode = ((long)::network_life_cycle_squad_session_get_ui_game_mode());
            if (storage.game_mode != -1) {
                long life_cycle_mode = uinl_advert_life_cycle_mode();
                if (new_class == 0 && (life_cycle_mode == 3 || life_cycle_mode == 2)) {
                    bool changed;
                    uinl_advert_change_session_class(changed);
                    success = changed;
                } else {
                    long maximum_players = network_squad_session_get_maximum_player_count();
                    gui_pregame_setup_manager_save_current_setup(storage.game_mode, global_preferences_get_game_setup_storage(), &storage);
                    ::network_life_cycle_end();
                    success = network_life_cycle_create_local_squad((e_network_session_class)new_class);
                    if (success) {
                        if ((unsigned long)(maximum_players - 1) <= 15)
                            network_squad_session_set_maximum_player_count(maximum_players);
                        g_pregame_setup_manager.set_pending_setup(&storage);
                    }
                }
            } else {
                ::network_life_cycle_end();
            }
            if (success)
                old_privacy = ((long)::network_squad_session_get_privacy_mode());
        }
        if (success && new_privacy != old_privacy) {
            bool privacy_changed;
            uinl_advert_set_privacy(privacy_changed, new_privacy);
            success &= privacy_changed;
        }
    } else {
        success = false;
    }
    if (!success)
        ::user_interface_play_sound(_ui_sound_error, -1);
    return success;
}

static __forceinline void uinl_matchmaking_get_matchmaking_progress(s_life_cycle_matchmaking_progress *p)
{
    network_life_cycle_get_matchmaking_progress(p);
    g_user_interface_matchmaking_progress = *p;
}
// @address 0x479de0 ?user_interface_networking_should_show_teams@@YA_NXZ
bool user_interface_networking_should_show_teams(void)
{
    bool result = false;
    if (network_life_cycle_in_matchmaking()) {
        s_life_cycle_matchmaking_progress progress;
        uinl_matchmaking_get_matchmaking_progress(&progress);
        if (progress.type == 0)
            result = uinl_networking_globals_from_progress()->show_teams;
        else if (progress.teams) {
            result = (progress.type == 4);
            result |= (progress.type == 5 && progress.game_found);
        }
        uinl_networking_globals_from_progress()->show_teams = result;
    } else {
        e_gui_game_mode game_mode = ::network_life_cycle_squad_session_get_ui_game_mode();
        if (game_mode == _gui_game_mode_multiplayer || game_mode == _gui_game_mode_mapeditor) {
            const c_game_variant *variant = user_interface_game_settings_get_game_variant();
            result = variant && (variant->flags & 1) != 0;
        }
    }
    return result;
}

// @address 0x479f48 user_interface_networking_has_found_game
extern "C" int user_interface_networking_has_found_game(void)
{
    int result = 0;
    if (network_life_cycle_in_matchmaking()) {
        s_life_cycle_matchmaking_progress progress;
        uinl_matchmaking_get_matchmaking_progress(&progress);
        result = progress.game_found;
    } else if (user_interface_squad_exists() && ((long)::network_life_cycle_squad_session_get_ui_game_mode()) == 1) {
        unsigned short hopper = network_squad_session_get_hopper();
        unsigned char ranked_hopper;
        if (hopper != 0xffff) {
            s_hopper_information information;
            if (::user_interface_matchmaking_get_hopper_information(hopper, &information) && information.ranked != 0)
                ranked_hopper = 1;
            else
                ranked_hopper = 0;
        } else {
            ranked_hopper = 0;
        }
        result = ranked_hopper;
    }
    return result;
}

// 0x48c070: first child of a widget whose type (m_type, 0x08) matches.
// @address 0x48c070 gui_widget_find_child_by_name
extern "C" c_gui_widget *gui_widget_find_child_by_name(c_gui_widget *widget, e_gui_widget_type type)
{
    c_gui_widget *child;

    for (child = widget->get_children(); child != 0 && child->m_type != type;
         child = child->get_next()) {
    }
    return child;
}

// @address 0x4c53b8 user_interface_initialize_coop_game_settings
extern "C" void user_interface_initialize_coop_game_settings(int unused_context, int controller_index)
{
    game_options options;
    __lwsync();
    _InterlockedExchange(&g_user_interface_globals.active_controller.value, -1);
    if (::game_state_read_game_options_from_persistent_storage_blocking((e_controller_index)controller_index, &options)
        && user_interface_squad_set_map(options.campaign_id, options.map_id)
        && network_squad_session_set_campaign_difficulty((e_campaign_difficulty_level)options.campaign_difficulty)
        && network_squad_session_set_primary_skulls(options.primary_skulls)
        && network_squad_session_set_secondary_skulls(options.secondary_skulls)
        && network_squad_session_set_campaign_metagame_scoring(options.campaign_metagame_scoring)
        && network_squad_session_set_campaign_insertion_point(options.campaign_insertion_point)) {
        __lwsync();
        _InterlockedExchange(&g_user_interface_globals.active_controller.value, controller_index);
    }
}


static __forceinline s_uinl_dialog_id uinl_dialog_id_copy(long value)
{
    s_uinl_dialog_id result;
    result.value = value;
    return result;
}


static __forceinline bool uinl_join_no_remote_players()
{
    long local_player_count = user_interface_squad_get_local_player_count();
    return user_interface_squad_get_player_count() <= local_player_count;
}
static __forceinline bool uinl_join_leader_with_same_class(bool no_remote_players, e_network_session_class session_class)
{
    return !no_remote_players && ::network_life_cycle_squad_local_peer_is_leader() &&
        session_class == network_squad_session_get_session_class();
}

// @address 0x4797f8 ?user_interface_join_remote_session@@YAX_NW4e_network_session_class@@PBUs_transport_secure_identifier@@PBUs_transport_secure_address@@PBUs_transport_secure_key@@@Z
void user_interface_join_remote_session(bool from_title, e_network_session_class session_class,
    const s_transport_secure_identifier *session_id, const s_transport_secure_address *host_address,
    const s_transport_secure_key *key)
{
    s_user_interface_networking_globals &uinl_join_globals = g_user_interface_networking_globals;
    bool display_dialog = true;
    long dialog_name = -1;
    if (user_interface_squad_exists()) {
        bool no_remote_players = uinl_join_no_remote_players();
        bool leader_with_same_class = !no_remote_players && ::network_life_cycle_squad_local_peer_is_leader() &&
            session_class == network_squad_session_get_session_class();
        switch (user_interface_get_session_game_mode()) {
        case _session_game_mode_matchmaking:
        case _session_game_mode_pregame:
        case _session_game_mode_postgame:
            {
                if (user_interface_squad_get_player_count() - user_interface_squad_get_local_player_count() > 0) {
                    if (leader_with_same_class)
                        dialog_name = _user_interface_networking_string_join_party_from_party_leader;
                    else if (!no_remote_players)
                        dialog_name = _user_interface_networking_string_join_party_from_party_member;
                }
            }
            break;
        case _session_game_mode_in_game:
            switch (::network_life_cycle_squad_session_get_ui_game_mode()) {
            case _gui_game_mode_campaign:
                if (no_remote_players)
                    dialog_name = _user_interface_networking_string_join_party_while_in_local_campaign;
                else if (leader_with_same_class)
                    dialog_name = _user_interface_networking_string_join_party_from_party_leader_in_campaign;
                else
                    dialog_name = _user_interface_networking_string_join_party_from_party_member_in_campaign;
                break;
            case _gui_game_mode_matchmaking:
                {
                    s_hopper_information information;
                    if (::user_interface_matchmaking_get_hopper_information(network_squad_session_get_hopper(), &information)) {
                        unsigned char special_hopper = information.ranked;
                        if (special_hopper != 0 && information.b3 != 0)
                            dialog_name = leader_with_same_class
                                ? _user_interface_networking_string_join_party_from_party_leader_in_match_lose_skill_and_xp
                                : _user_interface_networking_string_join_party_from_party_member_in_match_lose_skill_and_xp;
                        else if (special_hopper != 0)
                            dialog_name = leader_with_same_class
                                ? _user_interface_networking_string_join_party_from_party_leader_in_match_lose_skill
                                : _user_interface_networking_string_join_party_from_party_member_in_match_lose_skill;
                        else if (information.b3 != 0)
                            dialog_name = leader_with_same_class
                                ? _user_interface_networking_string_join_party_from_party_leader_in_match_lose_xp
                                : _user_interface_networking_string_join_party_from_party_member_in_match_lose_xp;
                        else
                            dialog_name = leader_with_same_class
                                ? _user_interface_networking_string_join_party_from_party_leader_in_match
                                : _user_interface_networking_string_join_party_from_party_member_in_match;
                    }
                }
                break;
            case _gui_game_mode_multiplayer:
            case _gui_game_mode_mapeditor:
                if (no_remote_players)
                    dialog_name = _user_interface_networking_string_join_party_while_in_local_game;
                else if (leader_with_same_class)
                    dialog_name = _user_interface_networking_string_join_party_from_party_leader;
                else
                    dialog_name = _user_interface_networking_string_join_party_from_party_member;
                break;
            case _gui_game_mode_theater:
                if (no_remote_players)
                    dialog_name = _user_interface_networking_string_join_party_while_in_local_theater;
                else if (leader_with_same_class)
                    dialog_name = _user_interface_networking_string_join_party_from_party_leader_in_theater;
                else
                    dialog_name = _user_interface_networking_string_join_party_from_party_member_in_theater;
                break;
            }
            break;
        }
    }
    if (dialog_name != -1) {
        void *memory = ::user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), (char const *)(0), 0);
        c_load_dialog_screen_message *message = memory ? new (memory, s_user_interface_networking_placement()) c_load_dialog_screen_message(
            k_any_controller, _console_window, 0x1000c, dialog_name, 0x100d0) : 0;
        if (message != 0) {
            uinl_join_globals.dialog_name = dialog_name;
            uinl_join_globals.join_as_friend = from_title;
            uinl_join_globals.remote_session_class = session_class;
            memcpy(&uinl_join_globals.session_id, session_id, sizeof(uinl_join_globals.session_id));
            memcpy(&uinl_join_globals.host_address, host_address, sizeof(uinl_join_globals.host_address));
            memcpy(&uinl_join_globals.session_key, key, sizeof(uinl_join_globals.session_key));
            ::user_interface_messaging_post(message);
        }
    } else {
        if (user_interface_get_session_game_mode() == _session_game_mode_matchmaking) {
            network_life_cycle_abort_matchmaking();
            display_dialog = false;
        }
        network_join_to_remote_squad(!::network_life_cycle_squad_local_peer_is_leader(), from_title, true, session_class, session_id, key, host_address);
    }
    if (display_dialog)
        user_interface_networking_reset_theater_campaign();
}


// @address 0x479b58 ?user_interface_networking_handle_join_dialog_result@@YA_NPBVc_dialog_result_message@@@Z
bool user_interface_networking_handle_join_dialog_result(const c_dialog_result_message *message)
{
    s_user_interface_networking_globals &uinl_join_globals = g_user_interface_networking_globals;
    bool result = false;
    const c_dialog_result_message *join_data = message;
    long join_kind = join_data->m_dialog_name - _user_interface_networking_string_join_party_from_party_leader;
    switch (join_kind) {
    case 0:
    case _user_interface_networking_string_join_party_from_party_leader_in_campaign
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_from_party_leader_in_theater
        - _user_interface_networking_string_join_party_from_party_leader:
        if (!((unsigned long)join_data->m_dialog_result < 1)) {
            if ((unsigned long)join_data->m_dialog_result == 1) {
            ((network_join_to_remote_squad_raw_fn)network_join_to_remote_squad)(true, uinl_join_globals.join_as_friend, true,
                (e_network_session_class)uinl_join_globals.remote_session_class,
                &uinl_join_globals.session_id,
                &uinl_join_globals.session_key,
                &uinl_join_globals.host_address);
            }
        } else {
            ((network_join_to_remote_squad_raw_fn)network_join_to_remote_squad)(false, uinl_join_globals.join_as_friend, true,
                (e_network_session_class)uinl_join_globals.remote_session_class,
                &uinl_join_globals.session_id,
                &uinl_join_globals.session_key,
                &uinl_join_globals.host_address);
        }
        result = true;
        break;
    case 1:
    case _user_interface_networking_string_join_party_from_party_member_in_campaign
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_from_party_leader_in_match
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_from_party_member_in_match
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_from_party_leader_in_match_lose_skill
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_from_party_member_in_match_lose_skill
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_from_party_leader_in_match_lose_skill_and_xp
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_from_party_member_in_match_lose_skill_and_xp
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_from_party_leader_in_match_lose_xp
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_from_party_member_in_match_lose_xp
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_from_party_member_in_theater
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_while_in_local_game
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_while_in_local_campaign
        - _user_interface_networking_string_join_party_from_party_leader:
    case _user_interface_networking_string_join_party_while_in_local_theater
        - _user_interface_networking_string_join_party_from_party_leader:
        if (join_data->m_dialog_result == 0) {
            if (user_interface_get_session_game_mode() == _session_game_mode_matchmaking)
                network_life_cycle_abort_matchmaking();
            ((network_join_to_remote_squad_raw_fn)network_join_to_remote_squad)(true, uinl_join_globals.join_as_friend, true,
                (e_network_session_class)uinl_join_globals.remote_session_class,
                &uinl_join_globals.session_id,
                &uinl_join_globals.session_key,
                &uinl_join_globals.host_address);
        }
        result = true;
        break;
    default:
        break;
    }
    return result;
}

static __forceinline bool uinl_start_game_peers_established(void)
{
    bool result = false;
    c_network_session *session = 0;
    if (user_interface_squad_try_and_get_session((s_network_session **)&session)) {
        // match: 0x47a028: assume keeps the globals ref in r30; removal swaps it with rlwinm temps into r31 (15 reg-only insns)
        __assume(session != 0);
        result = ((c_network_session *)session)->get_session_membership()->all_peers_established();
    }
    return result;
}
static __forceinline bool uinl_game_globals_game_in_progress(game_globals_storage *globals)
{
    bool result = false;
    if (globals && globals->game_in_progress && !globals->initializing && globals->map_active)
        result = true;
    return result;
}

// @address 0x47a028 ?user_interface_networking_start_game_when_ready_update@@YAXXZ
void user_interface_networking_start_game_when_ready_update(void)
{
    s_user_interface_networking_globals &uinl_start_globals = g_user_interface_networking_globals;
    if (uinl_start_globals.start_pending) {
        bool ready = true;
        ready = ready && (((unsigned char (*)(void))user_interface_squad_exists)() != 0);
        ready = ready && uinl_start_game_peers_established();
        bool countdown = ready && user_interface_get_session_game_mode() == _session_game_mode_pregame;
        e_session_game_mode mode;
        bool leaving = (_rotl(_CountLeadingZeros((unsigned long)((unsigned char (*)(void))user_interface_squad_exists)()), 27) & 1) ||
            ((mode = user_interface_get_session_game_mode()) != _session_game_mode_none && mode != _session_game_mode_pregame);
        if (countdown) {
            long start_error;
            unsigned long peer_count;
            long status = ((long)::user_interface_get_session_game_start_status(
                (e_session_game_start_error *)&start_error, (unsigned long *)(&peer_count)));
            if (status == 5) {
                ::user_interface_squad_start_countdown_timer((e_controller_index)uinl_start_globals.start_controller, 0, 0);
            } else if (status == 7 && start_error != 13) {
                g_gui_error_manager.post_error_with_custom_message(
                    _user_interface_networking_string_campaign_start_failed_unknown_reason, 0, 0,
                    (e_controller_index)uinl_start_globals.start_controller, false, true);
                user_interface_leave_squad(0);
                uinl_start_globals.start_pending = 0;
                g_window_manager.reset_screens();
            }
        }
        if (leaving)
            uinl_start_globals.start_pending = 0;
    }
    game_globals_storage *globals = (game_globals_storage *)tls_game_globals();
    bool in_progress = uinl_game_globals_game_in_progress(globals);
    if (!in_progress || globals->options.game_mode != 3) {
        bool cancelled = uinl_start_globals.start_cancelled;
        if (!cancelled)
            uinl_start_globals.start_pending = cancelled;
    }
    long start_state = uinl_start_globals.start_state;
    if (start_state == 1) {
        if (simulation_starting_up() && globals->options.game_mode != 3)
            uinl_start_globals.start_state = 2;
    } else if (start_state == 2 && simulation_starting_up() && globals->options.game_mode == 3) {
        uinl_start_globals.start_state = 0;
        user_interface_leave_squad(1);
    }
}


// @address 0x497c48 ?process_message@c_start_menu_screen_widget@@UAA_NPBVc_message@@@Z
bool c_start_menu_screen_widget::process_message(const c_message *input_message)
{
    c_start_menu_screen_widget *self = this;
    const c_xenon_message *message = (const c_xenon_message *)input_message;
    bool result = false;
    if (message->m_message_type == 5) {
        unsigned long state = (unsigned long)message->m_xenon_message_type;
        if (state >= 1) {
            if (state != 1) {
                if (state < 3) {
                    self->transition_out_with_transition_type(0, _screen_transition_normal);
                    result = 1;
                }
            } else {
                long pane_name = self->m_pane;
                long pane_index = message->m_event_value;
                c_gui_widget *widget = 0;
                // default
                float translation;
                if (pane_name != -1) {
                    widget = self->get_child_widget(_gui_screen_widget, pane_name);
                }
                translation = widget && start_menu_pane_is_valid_type(widget) != 0
                    ? static_cast<c_start_menu_pane_screen_widget *>(widget)->m_translation
                    : 0.0f;
                float rotation = widget && start_menu_pane_is_valid_type(widget) != 0
                    ? static_cast<c_start_menu_pane_screen_widget *>(widget)->m_rotation
                    : 0.0f;
                bool clear = self->m_root_pane_shown && self->m_breadcrumbs.count() == 1;
                if (!self->m_breadcrumbs.empty())
                    self->m_breadcrumbs.pop();
                self->load_pane(pane_index, clear, _screen_transition_start_menu_back, -1, -1, translation, rotation);
                result = 1;
            }
        } else {
            self->back_out_current_pane();
            result = 1;
        }
    }
    if (!result)
        result = c_gui_screen_widget::process_message(input_message);
    return result;
}


static __forceinline bool user_interface_networking_dialog_game_in_progress()
{
    game_globals_storage *globals = tls_game_globals();
    bool result = false;
    if (globals && globals->in_progress && !globals->initializing && globals->map_active)
        result = true;
    return result;
}
static __forceinline unsigned long user_interface_networking_dialog_time()
{
    return g_network_time_globals.enabled ? g_network_time_globals.time : GetTickCount();
}
static __forceinline bool user_interface_networking_dialog_is_active(s_user_interface_networking_globals &globals)
{
    bool result = false;
    c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(_console_window, globals.screen_name);
    long dialog = globals.dialog;
    if (screen && static_cast<c_gui_dialog_screen_widget *>(screen)->get_dialog_name() == dialog) {
        result = true;
    } else {
        const c_message *message = 0;
        while (user_interface_messaging_get_next_message(-1, k_any_controller, k_any_window, &message)) {
            const c_dialog_result_message *load = (const c_dialog_result_message *)message;
            if (load->m_message_type == 6 && load->m_dialog_name == dialog) {
                result = true;
                break;
            }
        }
    }
    return result;
}

// @address 0x478648 ?user_interface_network_dialogs_update@@YAXXZ
void user_interface_network_dialogs_update()
{
    s_user_interface_networking_globals &globals = g_user_interface_networking_globals;
    network_session_interface_globals &session_interface = g_network_session_interface_globals;
    long error = -1;
    long dialog = -1;
    if (globals.error != -1 &&
        !gui_error_manager_find_queued_error(g_gui_error_manager.errors, k_any_controller, globals.error, 0)) {
        for (long user_index = 0; user_index < k_number_of_controllers; ++user_index) {
            if (session_interface.users[user_index].state >= _network_session_interface_user_state_error_first)
                session_interface.users[user_index].state = _network_session_interface_user_state_zombie;
        }
        globals.error = -1;
        globals.local_user_state = _network_session_interface_user_state_none;
        globals.controller = -1;
    }
    if (globals.dialog != -1 &&
        !user_interface_networking_dialog_is_active(globals)) {
        globals.dialog = -1;
        globals.local_user_state = _network_session_interface_user_state_none;
        globals.controller = -1;
        globals.screen_name = -1;
    }
    long state = _network_session_interface_user_state_none;
    long active_controller_count = 0;
    e_controller_index controller_index = k_no_controller;
    long user_index = 0;
    do {
        long current_state = session_interface.users[user_index].state;
        if (current_state != _network_session_interface_user_state_none &&
            current_state != _network_session_interface_user_state_zombie)
            ++active_controller_count;
        if (current_state == state) {
            controller_index = k_any_controller;
        } else if (current_state > state) {
            state = current_state;
            controller_index = k_any_controller;
        }
    } while (++user_index < k_number_of_controllers);
    if (!user_interface_networking_is_throttled() && active_controller_count == 0 && state < _network_session_interface_user_state_error_everybody_signed_out &&
        (network_life_cycle_in_squad_session(0)
            || globals.error == _user_interface_networking_string_no_profile_sent_to_main_menu)
        && user_interface_networking_dialog_game_in_progress()) {
        if (tls_game_globals()->options.game_mode == 3)
            state = _network_session_interface_user_state_error_everybody_signed_out;
        else
            user_interface_leave_squad(0);
    }
    if (state >= _network_session_interface_user_state_error_first) {
        if (state >= globals.local_user_state) {
            switch (state) {
            case _network_session_interface_user_state_error_not_signed_in:
                dialog = network_squad_session_get_session_class() == _network_session_class_xbox_live
                    ? _user_interface_networking_string_not_signed_in_bring_up_guide
                    : _user_interface_networking_string_not_signed_in_bring_up_guide_or_use_temporary_account;
                break;
            case _network_session_interface_user_state_error_online_permissions:
                dialog = _user_interface_networking_string_sign_in_failed_online_permissions_1_player;
                break;
            case _network_session_interface_user_state_error_user_content_not_permitted:
                error = _user_interface_networking_string_sign_in_user_content_not_permitted;
                break;
            case _network_session_interface_user_state_error_generic:
                error = _user_interface_networking_string_sign_in_failed_generic;
                break;
            case _network_session_interface_user_state_error_game_in_matchmaking:
                error = _user_interface_networking_string_sign_in_game_in_matchmaking;
                break;
            case _network_session_interface_user_state_error_game_in_campaign:
                error = _user_interface_networking_string_sign_in_game_in_campaign;
                break;
            case _network_session_interface_user_state_error_game_in_theater:
                error = _user_interface_networking_string_sign_in_game_in_theater;
                break;
            case _network_session_interface_user_state_error_game_full:
                error = _user_interface_networking_string_sign_in_failed_game_full;
                break;
            case _network_session_interface_user_state_error_no_split_screen_on_alpha:
                error = _user_interface_networking_string_alpha_sign_in_failed_no_split_screen;
                break;
            case _network_session_interface_user_state_error_everybody_signed_out:
                error = _user_interface_networking_string_no_profile_sent_to_main_menu;
                break;
            case _network_session_interface_user_state_error_confirm_temporary:
                dialog = _user_interface_networking_string_confirm_the_use_of_a_temporary_account;
                break;
            }
        } else {
            dialog = globals.dialog;
            error = globals.error;
        }
    }
    if (g_message_globals.flag) {
        dialog = -1;
        error = -1;
        globals.sign_in_requested = false;
        globals.sign_in_lock = true;
        globals.last_request_time = user_interface_networking_dialog_time();
    } else {
        if (globals.sign_in_lock) {
            globals.last_request_time = user_interface_networking_dialog_time();
            globals.sign_in_lock = false;
            dialog = -1;
            error = -1;
        } else {
            long last_time = globals.last_request_time;
            if ((long)(user_interface_networking_dialog_time() - last_time) < 500) {
                dialog = -1;
                error = -1;
            }
        }
        if (globals.sign_in_requested) {
            dialog = -1;
            error = -1;
        }
    }
    if (globals.error != -1 && (globals.error != error || globals.controller != controller_index)) {
        g_gui_error_manager.clear_error(globals.error, (e_controller_index)globals.controller, 0);
        globals.error = -1;
        globals.local_user_state = _network_session_interface_user_state_none;
        globals.controller = -1;
    }
    if (globals.dialog != -1 && (uinl_dialog_id_copy(globals.dialog).value != dialog || globals.controller != controller_index)) {
        c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(_console_window, globals.screen_name);
        if (screen) {
            screen->transition_out_with_transition_type(0, _screen_transition_normal);
            globals.sign_in_required = true;
            globals.dialog = -1;
            globals.local_user_state = _network_session_interface_user_state_none;
            globals.controller = -1;
            globals.screen_name = -1;
        }
    }
    if (globals.error == -1 && globals.dialog == -1) {
        if (error != -1 || dialog != -1) {
            if (error != -1) {
                g_gui_error_manager.post_error_with_custom_message(error, 0, 0, controller_index, false, true);
                if (error == _user_interface_networking_string_no_profile_sent_to_main_menu)
                    network_life_cycle_end();
            } else {
                void *memory = ::user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), (char const *)(0), 0);
                c_load_dialog_screen_message *message = memory
                    ? new (memory, s_user_interface_networking_placement())
                          c_load_dialog_screen_message(controller_index, _console_window, 0x1000c, dialog, 0x10021)
                    : 0;
                if (message) {
                    globals.screen_name = message->get_screen_name();
                    ::user_interface_messaging_post(message);
                } else {
                    state = _network_session_interface_user_state_none;
                    controller_index = k_no_controller;
                    dialog = -1;
                }
            }
            globals.local_user_state = state;
            globals.controller = controller_index;
            globals.error = error;
            globals.dialog = dialog;
        }
    }
}
