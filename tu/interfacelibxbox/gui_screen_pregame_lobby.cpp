// interfacelibxbox/gui_screen_pregame_lobby.obj: from the records 0x4c26c0 0x4cbb48 0x4f0a18 0x4f0f00 0x4f0f90 0x4f1560 0x4f1648 0x4f1720 0x4f19f8 0x4f1a10 0x4f1a28 0x4f1f60 0x4f2520 0x4f2988 0x4f2a70 0x4f2b50 0x4f2bc8 0x4f2c28 0x4f2c30 0x4f2ca0 0x4f2e00 0x4f2e90 0x4f2f20 0x4f3010 0x4f3088 0x4f3118 0x4f3178 0x4f31d8 0x4f33e0
// @flags /TP /O2 /Oc
#include "interface/gui_render_data_size_types.h"
#include "interface/gui_location_manager.h"
#include "networking/logic/life_cycle/life_cycle_globals_access.h"
// @address 0x11401e8 g_controller_globals
// @address 0xf11308 g_network_life_cycle_globals

// @address 0x7c341c g_lobby_location_manager
#include "interface/gui_screens/boot_betrayer/gui_screen_boot_betrayer.h"
#include "cseries/cseries_unicode.h"
#include "cseries/maximum.h"
#include "interface/gui_screen_widget_text.h"
#include "saved_games/saved_game_files.h"
#include "interface/user_interface_controller.h"
#include "game/map_variant.h"
#include "game/game_engine_team.h"
#include "interface/gui_network_session_advertisement_types.h"
#include "interface/session_game_mode_types.h"
#include "interface/custom_map_image_types.h"
#include "shell/shell.h"
#include <stddef.h>
#include "game/game_types.h"
#include "game/player_types.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_widget.h"
#include "interface/gui_data.h"
#include "tag_files/tag_types.h"
#include "memory/static_arrays.h"
#include "game/game_variant.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_game_mode_types.h"
#include "interface/user_interface_sound_effect_types.h"
// @address 0x4f0f90 ?update@c_gui_screen_pregame_lobby@@UAAXK@Z
// @address 0x4f0a18 ?initialize@c_gui_screen_pregame_lobby@@UAAXXZ
// @address 0x4f0f00 ?post_initialize@c_gui_screen_pregame_lobby@@UAAXXZ
// @address 0x4f1560 gui_screen_pregame_lobby_update_start_widget
// @address 0x4f1648 gui_screen_pregame_lobby_update_count_badge
// @address 0x4f1720 ?update_widget_visiblility@c_gui_screen_pregame_lobby@@AAAXXZ
// @address 0x4f2520 ?handle_list_item_chosen@c_gui_screen_pregame_lobby@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
// @address 0x4cc938 ??0c_load_player_select_screen_message@@QAA@JW4e_controller_index@@W4e_window_index@@JPBUs_player_identifier@@2_N@Z
// @address 0x4f1a28 gui_screen_pregame_lobby_get_start_label
// @address 0x4f2988 ?handle_dialog_result@c_gui_screen_pregame_lobby@@UAA_NPBVc_dialog_result_message@@@Z
// @address 0x4f2a70 ?handle_back_out@c_gui_screen_pregame_lobby@@AAA_NW4e_controller_index@@@Z
// @address 0x4f2bc8 ?get_current_variant@c_gui_screen_pregame_lobby@@IAAPBVc_game_variant@@XZ
// @address 0x4f2c28 ?team_switching_allowed@c_gui_screen_pregame_lobby@@MAA_NXZ
// @address 0x4f2c30 ?get_current_map_id@c_gui_screen_pregame_lobby@@IAA?AW4e_map_id@@XZ
// @address 0x4f2ca0 ?load_game_options@c_gui_screen_pregame_lobby@@IAAXW4e_controller_index@@@Z
// @address 0x4f2e00 parse_xml_lobby_header
// @address 0x4f2e90 parse_xml_lobby_title
// @address 0x4f3088 ?parse_xml_lobby_start_button_name@@YA_NPAXPA_WJ@Z
// @address 0x4f2f20 parse_xml_lobby_network
// @address 0x4f3010 ?parse_xml_lobby_party_leader@@YA_NPAXPA_WJ@Z
// @address 0x4f3118 ?parse_xml_lobby_current_players@@YA_NPAXPA_WJ@Z
// @address 0x1bfeb8 ?set@?$c_static_wchar_string@$0EAA@@@QAAXPB_W@Z
// @address 0x8a3c0 ?ustrnzcpy@@YAPA_WPA_WPB_WJ@Z
#include <ppcintrinsics.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/session/session_game_start_error_types.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "interface/user_interface_networking.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/online/online_xenon.h"
#include "cseries/async.h"
#include "interface/gui_screens/game_details/gui_game_details.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"
#include "interface/gui_screens/campaign/gui_screen_campaign_options.h"
#include "interface/gui_screens/player_select/gui_player_select_screen_widget.h"
#include "interface/user_interface_utilities.h"
#include "saved_games/content_catalogue.h"
#include "interface/game_tag_parser.h"
#include "interface/gui_screens/game_options/game_engine_category_types.h"
#include "interface/ui_allocation_marker_types.h"
#include "interface/c_gui_widget.h"
#include "networking/online/online_guide_xenon.h"
#include "interface/user_interface_allocation.h"
#include "interface/gui_screen_pregame_lobby_externs.h"
#include "interface/user_interface_clock.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
enum e_session_game_start_status;
enum e_session_game_start_error;


#include "interface/user_interface_window_manager.h"


#include "interface/user_interface_messages.h"

#include "interface/controller_types.h"


#include <new>
#include "interface/user_interface_networking_allocation.h"


#include "interface/list_widget_definition.h"
#include "interface/gui_roster_list_externs.h"


#include "interface/gui_custom_bitmap_widget.h"
#include "interface/gui_roster_data.h"


#include "interface/c_gui_text_widget.h"
#include "interface/gui_selected_items.h"
#include "interface/gui_screens/player_select/player_select_screen_message.h"
#include "cseries/compiler_intrinsics.h"
#include "interface/gui_screens/game_options/gui_screen_game_options.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"

void c_gui_screen_pregame_lobby::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-title", this, parse_xml_lobby_title));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-header", this, parse_xml_lobby_header));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-network", this, parse_xml_lobby_network));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<lobby-party-leader", this, parse_xml_lobby_party_leader));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<lobby-start-button-name", this, parse_xml_lobby_start_button_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<lobby-current-players", this, parse_xml_lobby_current_players));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<lobby-max-players", this, parse_xml_lobby_max_players));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<lobby-countdown-remaining", this, parse_xml_lobby_countdown_remaining));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-privacy" /* L"<lobby-privacy" */, this, parse_lobby_privacy));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<lobby-delaying-player", this, parse_lobby_delaying_player));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-film-max-players", this,
                network_configuration_parse_lobby_film_max_players));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<lobby-film-party-leader-requirement", this,
        parse_xml_lobby_film_party_leader_requirement));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-forge-max-players", this,
                network_configuration_parse_lobby_forge_max_players));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-coop-max-players", this,
                network_configuration_parse_lobby_coop_max_players));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<lobby-percent-loaded", this, parse_xml_lobby_percent_loaded));
}


// 0x4f0f00 ?post_initialize@c_gui_screen_pregame_lobby@@UAAXXZ
void c_gui_screen_pregame_lobby::post_initialize()
{
    c_gui_screen_widget::post_initialize();
    if (g_lobby_location_manager.get_show_postgame_stats_upon_lobby_entrance()) {
        c_load_screen_message *message = 0;
        void *memory = user_interface_malloc_tracked(sizeof(c_load_screen_message), 0, 0);
        if (memory)
            message = new (memory) c_load_screen_message(_pregame_lobby_string_carnage_report,
                k_any_controller,
                _console_window,
                this->name);
        if (message)
            ::user_interface_messaging_post(message);
        g_lobby_location_manager.set_show_postgame_stats_upon_lobby_entrance(false);
    }
}


// @address 0x4a8e80 ?dispose@c_gui_screen_pregame_lobby@@UAAXXZ
void c_gui_screen_pregame_lobby::dispose()
{
    c_gui_screen_widget::dispose();
}

// @address 0x5bec98 ?submenu_invoked@c_gui_screen_pregame_lobby@@UAAXPAVc_gui_list_widget@@@Z
void c_gui_screen_pregame_lobby::submenu_invoked(c_gui_list_widget *submenu_widget)
{
}

// @address 0x499198 ?get_render_data_size@c_gui_screen_pregame_lobby@@MAA?AW4e_render_data_size@@XZ
e_render_data_size c_gui_screen_pregame_lobby::get_render_data_size()
{
    return k_render_data_size_large;
}

// @address 0x4f19f8 ?start_fade_during_countdown@c_gui_screen_pregame_lobby@@UAAXXZ
void c_gui_screen_pregame_lobby::start_fade_during_countdown()
{
    g_window_manager.m_fade_in = true;
}

// @address 0x4f1a10 ?stop_fade_during_countdown@c_gui_screen_pregame_lobby@@UAAXXZ
void c_gui_screen_pregame_lobby::stop_fade_during_countdown()
{
    g_window_manager.m_fade_in = false;
}


// sets the start label and the visibility of the start_game list item
extern "C" void gui_screen_pregame_lobby_update_start_widget(c_gui_screen_widget *screen)
{
    c_gui_widget *start_text = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _pregame_lobby_string_lobby_status);
    c_gui_widget *visible_widget = gui_screen_widget_find_list_item_by_value((c_gui_widget *)screen,
        _pregame_lobby_string_lobby_list, _pregame_lobby_string_name, _pregame_lobby_string_start_game);
    bool start_status_is_seven =
        (((unsigned int)::user_interface_get_session_game_start_status(0, (unsigned long *)(0))) - 7 ==
            0);
    bool is_leader = ::network_life_cycle_squad_local_peer_is_leader();
    unsigned int label = gui_screen_pregame_lobby_get_start_label(is_leader);
    screen->set_text_widget_string_id(_pregame_lobby_string_lobby_status, label);
    if (start_text != 0)
        gui_widget_set_alternate_state(start_text, start_status_is_seven);
    if (visible_widget != 0) {
        bool show;
        if (!::network_life_cycle_squad_local_peer_is_leader())
            show = false;
        else if (user_interface_squad_in_or_after_countdown())
            show = false;
        else {
            show = true;
            if (start_status_is_seven)
                show = false;
        }
        visible_widget->set_enabled(show);
    }
}

// shows the player-count badge
extern "C" void gui_screen_pregame_lobby_update_count_badge(c_gui_widget *widget, long milliseconds)
{
    c_gui_widget *count_widget = widget->get_child_widget(_gui_group_widget, _pregame_lobby_string_precaching);
    if (count_widget != 0) {
        c_network_session *session = 0;
        unsigned short count = 0;
        if (user_interface_get_session_game_mode() == _session_game_mode_pregame && user_interface_squad_try_and_get_session(&session)) {
            c_network_session_parameters::s_network_session_parameters_internal *parameters = &session->get_session_parameters()->m_parameters;
            if (parameters->game_start_status.get_allowed()) {
                const s_network_session_parameter_game_start_status *status = parameters->game_start_status.get();
                count = status->map_load_progress;
            }
        }
        count_widget->start_animation_at_time(_animation_state_custom0, milliseconds - count * 10, true);
        count_widget->set_visible(count != 0);
    }
}


class c_game_results;

// 0x4f1720 ?update_widget_visiblility@c_gui_screen_pregame_lobby@@AAAXXZ
void c_gui_screen_pregame_lobby::update_widget_visiblility()
{
    bool is_leader = ::network_life_cycle_squad_local_peer_is_leader();
    bool countdown = user_interface_squad_in_or_after_countdown();
    c_gui_widget *start_bitmap = this->get_child_widget(_gui_bitmap_widget, _pregame_lobby_string_advanced_options);
    c_gui_widget *start_text = this->get_child_widget(_gui_text_widget, _pregame_lobby_string_advanced_options);
    c_gui_widget *options_text = this->get_child_widget(_gui_text_widget, _pregame_lobby_string_change_teams);
    c_gui_widget *results_text = this->get_child_widget(_gui_text_widget, _pregame_lobby_string_postgame_stats);
    bool show_start = !countdown && advanced_options_enabled();
    if (start_text) {
        ((c_gui_text_widget *)start_text)->set_text_from_string_id(this, get_start_status_text(is_leader));
        gui_widget_set_alternate_state(start_text, !show_start);
        if (show_start && !start_visible)
            start_text->start_animation_at_time(_animation_state_custom0, 0xffffffff, false);
        else if (!show_start && start_visible)
            start_text->start_animation_at_time(_animation_state_custom1, 0xffffffff, false);
    }
    if (start_bitmap) {
        gui_widget_set_alternate_state(start_bitmap, !show_start);
        if (show_start && !start_visible)
            start_bitmap->start_animation_at_time(_animation_state_custom0, 0xffffffff, false);
        else if (!show_start && start_visible)
            start_bitmap->start_animation_at_time(_animation_state_custom1, 0xffffffff, false);
    }
    start_visible = show_start;
    if (options_text) {
        bool show_options = !countdown && team_switching_allowed();
        gui_widget_set_alternate_state(options_text, !show_options);
        if (show_options && !options_visible)
            options_text->start_animation_at_time(_animation_state_custom0, 0xffffffff, false);
        else if (!show_options && options_visible)
            options_text->start_animation_at_time(_animation_state_custom1, 0xffffffff, false);
        options_visible = show_options;
    }
    if (results_text) {
        bool show_results = !countdown && user_interface_networking_get_final_game_results() != 0;
        gui_widget_set_alternate_state(results_text, !show_results);
        if (show_results && !results_visible)
            results_text->start_animation_at_time(_animation_state_custom0, 0xffffffff, false);
        else if (!show_results && results_visible)
            results_text->start_animation_at_time(_animation_state_custom1, 0xffffffff, false);
        results_visible = show_results;
    }
}


extern "C" unsigned int gui_screen_pregame_lobby_get_start_label(unsigned char alternate)
{
  int countdown_culprit;
  unsigned int start_status;
  unsigned int text_id;
  unsigned int start_error[2];

  start_status = ((unsigned int)::user_interface_get_session_game_start_status((e_session_game_start_error *)start_error,
      (unsigned long *)(0)));
  countdown_culprit = network_squad_session_get_countdown_delayed_culprit();
  switch(start_status) {
  case 0:
    text_id = _pregame_lobby_string_status_none;
    break;
  case 1:
    text_id = _pregame_lobby_string_status_not_pregame;
    break;
  case 2:
    if (!(alternate == '\0')) {
      text_id = _pregame_lobby_string_status_changing_settings_member;
    }
    else {
      text_id = _pregame_lobby_string_status_ready_member;
    }
    break;
  case 3:
    text_id = _pregame_lobby_string_status_join_in_progress;
    break;
  case 4:
    text_id = _pregame_lobby_string_status_loading;
    break;
  case 5:
    text_id = _pregame_lobby_string_status_ready_leader;
    break;
  case 6:
    if (!(countdown_culprit == -1)) {
      text_id = _pregame_lobby_string_status_countdown_delayed;
    }
    else {
      if (!(alternate == '\0')) {
        text_id = _pregame_lobby_string_status_countdown_leader;
      }
      else {
        text_id = _pregame_lobby_string_status_countdown_member;
      }
    }
    break;
  case 7:
    switch(start_error[0]) {
    case _session_game_start_error_none:
      text_id = _pregame_lobby_string_error_none;
      break;
    case _session_game_start_error_network_configuration_unavailable:
      text_id = _pregame_lobby_string_error_network_configuration_unavailable;
      break;
    case _session_game_start_error_banhammer_unavailable:
      text_id = _pregame_lobby_string_error_banhammer_unavailable;
      break;
    case _session_game_start_error_player_stats_unavailable:
      text_id = _pregame_lobby_string_error_player_stats_unavailable;
      break;
    case _session_game_start_error_master_hopper_file_unavailable:
      text_id = _pregame_lobby_string_match_error_master_hopper_file_unavailable;
      break;
    case _session_game_start_error_hopper_specific_file_unavailable:
      text_id = _pregame_lobby_string_match_error_hopper_specific_file_unavailable;
      break;
    case _session_game_start_error_network_configuration_pending:
      text_id = _pregame_lobby_string_error_network_configuration_pending;
      break;
    case _session_game_start_error_banhammer_pending:
      text_id = _pregame_lobby_string_error_banhammer_pending;
      break;
    case _session_game_start_error_player_stats_pending:
      text_id = _pregame_lobby_string_error_player_stats_pending;
      break;
    case _session_game_start_error_master_hopper_file_pending:
      text_id = _pregame_lobby_string_match_error_master_hopper_file_pending;
      break;
    case _session_game_start_error_hopper_specific_file_pending:
      text_id = _pregame_lobby_string_match_error_hopper_specific_file_pending;
      break;
    case _session_game_start_error_no_map_selected:
      text_id = _pregame_lobby_string_error_no_map_selected;
      break;
    case _session_game_start_error_map_load_failure:
      text_id = _pregame_lobby_string_error_map_load_failure;
      break;
    case _session_game_start_error_map_load_precaching:
      text_id = _pregame_lobby_string_error_map_load_precaching;
      break;
    case _session_game_start_error_invalid_film_selected:
      text_id = _pregame_lobby_string_error_invalid_film_selected;
      break;
    case _session_game_start_error_no_film_selected:
      text_id = _pregame_lobby_string_error_no_film_selected;
      break;
    case _session_game_start_error_too_many_teams:
      text_id = _pregame_lobby_string_error_too_many_teams;
      break;
    case _session_game_start_error_all_players_are_observers:
      text_id = _pregame_lobby_string_error_all_players_are_observers;
      break;
    case _session_game_start_error_too_many_local_players_for_coop:
      text_id = _pregame_lobby_string_error_too_many_local_players_for_coop;
      break;
    case _session_game_start_error_too_many_players_for_network_coop:
      text_id = _pregame_lobby_string_error_too_many_players_for_network_coop;
      break;
    case _session_game_start_error_players_incompatible_for_network_coop:
      text_id = _pregame_lobby_string_error_players_incompatible_for_network_coop;
      break;
    case _session_game_start_error_account_not_online_enabled:
      text_id = _pregame_lobby_string_error_account_not_online_enabled;
      break;
    case _session_game_start_error_must_have_online_enabled_profiles_for_match:
      text_id = _pregame_lobby_string_match_error_must_have_online_enabled_profiles_for_match;
      break;
    case _session_game_start_error_must_connect_to_live_for_match:
      text_id = _pregame_lobby_string_match_error_must_connect_to_live_for_match;
      break;
    case _session_game_start_error_must_have_live_for_match:
      text_id = _pregame_lobby_string_match_error_must_have_live_for_match;
      break;
    case _session_game_start_error_invalid_hopper:
      text_id = _pregame_lobby_string_match_error_invalid_hopper;
      break;
    case _session_game_start_error_squad_too_large:
      text_id = _pregame_lobby_string_match_error_squad_too_large;
      break;
    case _session_game_start_error_squad_too_small:
      text_id = _pregame_lobby_string_match_error_squad_too_small;
      break;
    case _session_game_start_error_games_played_too_low:
      text_id = _pregame_lobby_string_match_error_games_played_too_low;
      break;
    case _session_game_start_error_games_played_too_high:
      text_id = _pregame_lobby_string_match_error_games_played_too_high;
      break;
    case _session_game_start_error_experience_too_low:
      text_id = _pregame_lobby_string_match_error_experience_too_low;
      break;
    case _session_game_start_error_experience_too_high:
      text_id = _pregame_lobby_string_match_error_experience_too_high;
      break;
    case _session_game_start_error_access_bit_not_set:
      text_id = _pregame_lobby_string_match_error_access_bit_not_set;
      break;
    case _session_game_start_error_unpaid_in_paid_hopper:
      text_id = _pregame_lobby_string_match_error_unpaid_in_paid_hopper;
      break;
    case _session_game_start_error_paid_in_unpaid_hopper:
      text_id = _pregame_lobby_string_match_error_paid_in_unpaid_hopper;
      break;
    case _session_game_start_error_player_not_in_required_region:
      text_id = _pregame_lobby_string_match_error_player_not_in_required_region;
      break;
    case _session_game_start_error_player_not_in_required_country_code:
      text_id = _pregame_lobby_string_match_error_player_not_in_required_country_code;
      break;
    case _session_game_start_error_player_not_in_required_language:
      text_id = _pregame_lobby_string_match_error_player_not_in_required_language;
      break;
    case _session_game_start_error_player_not_in_required_gamerzone:
      text_id = _pregame_lobby_string_match_error_player_not_in_required_gamerzone;
      break;
    case _session_game_start_error_guest_player_when_not_allowed:
      text_id = _pregame_lobby_string_match_error_guest_player_when_not_allowed;
      break;
    case _session_game_start_error_not_beta_player_in_beta_hopper:
      text_id = _pregame_lobby_string_match_error_not_beta_player_in_beta_hopper;
      break;
    case _session_game_start_error_player_missing_content:
      text_id = _pregame_lobby_string_match_error_player_missing_content;
      break;
    case _session_game_start_error_player_missing_files:
      text_id = _pregame_lobby_string_match_error_player_missing_files;
      break;
    case _session_game_start_error_player_missing_required_maps:
      text_id = _pregame_lobby_string_match_error_player_missing_required_maps;
      break;
    case _session_game_start_error_player_banned_from_matchmaking:
      text_id = _pregame_lobby_string_match_error_player_banned_from_matchmaking;
      break;
    case _session_game_start_error_not_yet_start_time:
      text_id = _pregame_lobby_string_match_error_not_yet_start_time;
      break;
    case _session_game_start_error_end_time_has_passed:
      text_id = _pregame_lobby_string_match_error_end_time_has_passed;
      break;
    case _session_game_start_error_custom_games_are_disabled_for_alpha:
      text_id = _pregame_lobby_string_error_custom_games_are_disabled_for_alpha;
      break;
    case _session_game_start_error_maximum_multiplayer_split_screen_exceeded:
      text_id = _pregame_lobby_string_error_maximum_multiplayer_split_screen_exceeded;
      break;
    case _session_game_start_error_must_be_connected_to_live_in_live_lobby:
      text_id = _pregame_lobby_string_must_be_connected_to_live_in_live_lobby;
      break;
    case _session_game_start_error_must_be_connected_to_live_for_alpha:
      text_id = _pregame_lobby_string_must_be_connected_to_live_for_alpha;
      break;
    case _session_game_start_error_only_one_player_allowed_in_theater_for_alpha:
      text_id = _pregame_lobby_string_only_one_player_allowed_in_theater_for_alpha;
      break;
    case _session_game_start_error_film_too_many_players:
      text_id = _pregame_lobby_string_error_film_too_many_players;
      break;
    case _session_game_start_error_film_harddrive_required:
      text_id = _pregame_lobby_string_error_film_harddrive_required;
      break;
    case _session_game_start_error_film_party_leader_requirement:
      text_id = _pregame_lobby_string_error_film_party_leader_requirement;
      break;
    case _session_game_start_error_film_all_players_not_compatible:
      text_id = _pregame_lobby_string_error_film_all_players_not_compatible;
      break;
    case _session_game_start_error_forge_too_many_players:
      text_id = _pregame_lobby_string_error_forge_too_many_players;
      break;
    case _session_game_start_error_user_content_not_permitted:
      text_id = _pregame_lobby_string_error_user_content_not_permitted;
      break;
    case _session_game_start_error_coop_player_missing_hdd:
      text_id = _pregame_lobby_string_match_error_coop_player_missing_hdd;
      break;
    case _session_game_start_error_coop_player_hdd_mismatch:
      text_id = _pregame_lobby_string_match_error_coop_player_hdd_mismatch;
      break;
    case _session_game_start_error_coop_player_language_mismatch:
      text_id = _pregame_lobby_string_match_error_coop_player_language_mismatch;
      break;
    case _session_game_start_error_invalid_film_selected_language:
      text_id = _pregame_lobby_string_match_error_invalid_film_selected_language;
      break;
    case _session_game_start_error_signedin_controller_detatched:
      text_id = _pregame_lobby_string_error_signedin_controller_detatched;
      break;
    default:
      text_id = _pregame_lobby_string_error_unknown;
    }
    break;
  default:
    text_id = _pregame_lobby_string_status_unknown;
  }
  return text_id;
}


static __forceinline bool gui_screen_pregame_lobby_controller_has_saved_game_state(e_controller_index controller_index)
{
    bool result = false;
    if (saved_game_files_get_personal_storage_content_item(controller_index, false) != -1)
        result = true;
    return result;
}

static __forceinline void gui_screen_pregame_lobby_set_reload_from_persistent_storage(e_controller_index controller_index)
{
    __lwsync();
    _InterlockedExchange(&g_user_interface_globals.active_controller.value, controller_index);
}

static __forceinline long gui_screen_pregame_lobby_get_name_from_gui_game_mode(long game_mode)
{
    long name;
    switch (game_mode) {
    case _gui_game_mode_campaign:
        name = _pregame_lobby_string_campaign;
        break;
    case _gui_game_mode_matchmaking:
        name = _pregame_lobby_string_matchmaking;
        break;
    case _gui_game_mode_multiplayer:
        name = _pregame_lobby_string_multiplayer;
        break;
    case _gui_game_mode_mapeditor:
        name = _pregame_lobby_string_mapeditor;
        break;
    case _gui_game_mode_theater:
        name = _pregame_lobby_string_theater;
        break;
    default:
        // match: 0x4f2520: unreachable hint omits dead branch check
        __assume(0);
    }
    return name;
}

bool c_gui_screen_pregame_lobby::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *data)
{
    bool handled = false;

    if (list_name == _pregame_lobby_string_lobby_list) {
        handled = false;
        long value = -1;
        if (data->get_string_id_value(list_item_widget->get_element_handle(), _pregame_lobby_string_target, &value)) {
            handled = false;
            if (value == _pregame_lobby_string_network_mode) {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_pregame_selection_screen_message), 0, 0);
                if (memory) {
                    long screen_name = this->name;
                    e_controller_index controller_index = message->get_controller();
                    e_window_index window = get_render_window();
                    c_load_pregame_selection_screen_message *selection_message =
                        new (memory) c_load_pregame_selection_screen_message(controller_index, window, screen_name,
                            _gui_selected_item_advertisement);
                    selection_message->set_parent_screen_index(this->m_screen_index);
                    ::user_interface_messaging_post(selection_message);
                }
                handled = true;
            }
        } else {
            long column_value = -1;
            if (data->get_string_id_value(list_item_widget->get_element_handle(), _pregame_lobby_string_name, &column_value)) {
                do {
                    if (column_value == _pregame_lobby_string_switch_lobby) {
                        long screen_name;
                        c_load_screen_message *load_message = new(0, 0, _ui_allocation_marker_dummy) c_load_screen_message(
                            _pregame_lobby_string_pregame_switch_lobby,
                                k_any_controller,
                                get_render_window(),
                                (screen_name = this->name));
                        if (load_message) {
                            load_message->set_parent_screen_index(this->m_screen_index);
                            load_message->set_focus_on_load_by_name(_pregame_lobby_string_switch_lobby, _pregame_lobby_string_name,
                                gui_screen_pregame_lobby_get_name_from_gui_game_mode(get_gui_game_mode()));
                            ::user_interface_messaging_post(load_message);
                        }
                    } else if (column_value == _pregame_lobby_string_start_game) {
                        if (::user_interface_squad_start_countdown_timer(message->get_controller(), 6, 4)) {
                            e_controller_index controller_index = user_interface_networking_get_saved_game_controller();
#ifdef HALO3_64BIT
                            long total_players = ::user_interface_squad_get_player_count();
                            if ((unsigned long)controller_index <= 3 && total_players >= 1 && total_players <= 4 &&
                                ::user_interface_squad_get_local_player_count() == total_players)
#else
                            if ((unsigned long)controller_index <= 3 && ::user_interface_squad_get_player_count() == 1 &&
                                gui_screen_pregame_lobby_controller_has_saved_game_state(controller_index))
#endif
                                gui_screen_pregame_lobby_set_reload_from_persistent_storage(controller_index);
                            else
                                gui_screen_pregame_lobby_set_reload_from_persistent_storage(_controller_none);
                        } else {
                            ::user_interface_play_sound(e_user_interface_sound_effect(0), this->m_core_definition.sound_override_index);
                        }
                    } else {
                        break;
                    }
                    handled = true;
                } while (false);
            }
        }
    } else if (list_name == _pregame_lobby_string_roster) {
        long player_index = -1;
        if (data->get_integer_value(list_item_widget->get_element_handle(), _pregame_lobby_string_player_index, &player_index)) {
            const s_player_identifier *target_player = user_interface_session_is_local_player(player_index);
            if (target_player) {
                e_controller_index controller_index = message->get_controller();
                handled = false;
                long local_player_index = ::user_interface_session_get_player_at_local_controller(controller_index);
                if (local_player_index != -1) {
                    const s_player_identifier *source_player = user_interface_session_is_local_player(local_player_index);
                    if (source_player) {
                        long screen_name;
                        c_load_player_select_screen_message *load_message = new(0,
                            0,
                            _ui_allocation_marker_dummy) c_load_player_select_screen_message(
                            _pregame_lobby_string_player_select,
                                controller_index,
                                get_render_window(),
                                (screen_name = this->name),
                                source_player,
                                target_player,
                                true);
                        if (load_message) {
                            ((c_load_screen_message *)load_message)->set_parent_screen_index(this->m_screen_index);
                            ::user_interface_messaging_post(load_message);
                            handled = true;
                        }
                    } else {
                        ::user_interface_play_sound(e_user_interface_sound_effect(0), this->m_core_definition.sound_override_index);
                    }
                }
            }
        }
    }

    if (!handled)
        handled = c_gui_screen_widget::handle_list_item_chosen(message, list_name, list_item_widget, data);

    return handled;
}

bool c_gui_screen_pregame_lobby::handle_dialog_result(const c_dialog_result_message *dialog_result_message)
{
    bool handled = false;

    switch (dialog_result_message->m_dialog_name) {
    case _pregame_lobby_string_back_out_lobby_local_players_with_profile:
        if (dialog_result_message->m_dialog_result == _gui_dialog_choice_first) {
            user_interface_leave_squad(true);
        }
        handled = true;
        break;
    case _pregame_lobby_string_back_out_lobby_local_players_no_profile:
        if (dialog_result_message->m_dialog_result == _gui_dialog_choice_first) {
            user_interface_leave_squad(true);
        } else if (dialog_result_message->m_dialog_result == _gui_dialog_choice_second) {
            g_controller_globals.controllers[dialog_result_message->m_controller].set_as_unsigned_in_user(false);
        }
        handled = true;
        break;
    case _pregame_lobby_string_back_out_lobby_remote_players:
        if (dialog_result_message->m_dialog_result == _gui_dialog_choice_first) {
            user_interface_leave_squad(false);
        }
        handled = true;
        break;
    }

    if (!handled) {
        handled = c_gui_screen_widget::handle_dialog_result(dialog_result_message);
    }

    return handled;
}

bool c_gui_screen_pregame_lobby::handle_back_out(e_controller_index controller_index)
{
    long dialog_name = -1;
    if (::user_interface_squad_get_local_player_count() > 1) {
        dialog_name = g_controller_globals.controllers[controller_index].is_signed_in_to_machine()
            ? _pregame_lobby_string_back_out_lobby_local_players_with_profile
            : _pregame_lobby_string_back_out_lobby_local_players_no_profile;
    } else if (network_squad_session_get_peer_count() > 1) {
        dialog_name = _pregame_lobby_string_back_out_lobby_remote_players;
    } else {
        user_interface_leave_squad(true);
    }

    if (dialog_name != -1) {
        c_load_dialog_screen_message *dialog = 0;
        void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
        if (memory) {
            dialog = new (memory) c_load_dialog_screen_message((e_controller_index)controller_index,
                (e_window_index)get_render_window(), this->name, dialog_name, this->name);
        }
        if (dialog) {
            dialog->m_parent_screen_index = this->m_screen_index;
            ::user_interface_messaging_post(dialog);
        }
    }
    return true;
}

const c_game_variant *c_gui_screen_pregame_lobby::get_current_variant()
{
    const c_game_variant *result = 0;
    if (network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_multiplayer ||
        network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_mapeditor ||
        network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_theater)
    {
        result = user_interface_game_settings_get_game_variant();
    }
    return result;
}

// 0x4f2c28 ?team_switching_allowed@c_gui_screen_pregame_lobby@@MAA_NXZ
bool c_gui_screen_pregame_lobby::team_switching_allowed()
{
    return user_interface_networking_is_team_switching_allowed();
}

// 0x4f2c30 ?get_current_map_id@c_gui_screen_pregame_lobby@@IAA?AW4e_map_id@@XZ
e_map_id c_gui_screen_pregame_lobby::get_current_map_id()
{
    e_campaign_id campaign_id;
    e_map_id map_id;
    e_map_id result = _map_id_none;
    if (network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_multiplayer ||
        network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_mapeditor ||
        network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_campaign ||
        network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_theater)
    {
        if (network_life_cycle_session_get_map(&campaign_id, &map_id))
            result = map_id;
    }
    return result;
}


// 0x4f2ca0
void c_gui_screen_pregame_lobby::load_game_options(e_controller_index controller_index)
{
    if (async_test_content_catalogue_busy())
        return;
    const c_game_variant *variant = user_interface_game_settings_get_game_variant();
    if (!variant)
        return;
    long item;
    switch (variant->m_game_engine_index) {
    case _game_engine_ctf: item = 159; break;
    case _game_engine_slayer: item = 160; break;
    case _game_engine_oddball: item = 161; break;
    case _game_engine_king: item = 162; break;
    case _game_engine_sandbox: item = 163; break;
    case _game_engine_vip: item = 168; break;
    case _game_engine_juggernaut: item = 164; break;
    case _game_engine_territories: item = 165; break;
    case _game_engine_assault: item = 166; break;
    case _game_engine_infection: item = 167; break;
    default: return;
    }
    const c_game_variant *edited = user_interface_game_settings_get_game_variant();
    if (edited) {
        void *memory = user_interface_malloc_tracked(sizeof(c_load_game_options_screen_message), 0, 0);
        c_load_game_options_screen_message *message = memory
            ? new (memory) c_load_game_options_screen_message(_pregame_lobby_string_game_options, controller_index,
                  (e_window_index)((long)get_render_window()), get_name(),
                edited, edited, _game_engine_category_none, (e_game_engine_category)item, -1, 0, 0)
            : 0;
        if (message) {
            message->m_transition_type = _screen_transition_normal;
            ::user_interface_messaging_post(message);
        }
    }
}


extern "C" bool parse_xml_lobby_header(void *context, wchar_t *output, long capacity)
{
    bool result = false;
    c_static_wchar_string<1024> string;
    c_gui_screen_pregame_lobby *screen = (c_gui_screen_pregame_lobby *)context;
    if (screen->get_string_by_string_id(screen->get_lobby_header(), &string)) {
        string.copy_to(output, capacity);
        result = true;
    }
    return result;
}

extern "C" bool parse_xml_lobby_title(void *context, wchar_t *output, long capacity)
{
    bool result = false;
    c_static_wchar_string<1024> string;
    c_gui_screen_pregame_lobby *screen = (c_gui_screen_pregame_lobby *)context;
    if (screen->get_string_by_string_id(screen->get_lobby_title(), &string)) {
        string.copy_to(output, capacity);
        result = true;
    }
    return result;
}

extern "C" bool parse_xml_lobby_network(void *context, wchar_t *output, long capacity)
{
    c_static_wchar_string<1024> string;
    c_static_wchar_string<1024> global_string;
    long string_id;
    switch (user_interface_networking_get_session_advertisement()) {
    case _network_advertise_xbox_live_open:
        string_id = _pregame_lobby_string_network_mode_xbox_live_advertise_anyone;
        break;
    case _network_advertise_xbox_live_friends_only:
        string_id = _pregame_lobby_string_network_mode_xbox_live_advertise_friends;
        break;
    case _network_advertise_xbox_live_invite_only:
        string_id = _pregame_lobby_string_network_mode_xbox_live_advertise_invite_only;
        break;
    case _network_advertise_system_link:
        string_id = _pregame_lobby_string_network_mode_system_link_advertise;
        break;
    case _network_advertise_offline:
        string_id = _pregame_lobby_string_network_mode_offline;
        break;
    default:
        // match: 0x4f2f20: unreachable hint omits dead branch check
        __assume(0);
    }
    if (user_interface_global_string_get((int)string_id, (unsigned short *)global_string.m_string))
        string.set(global_string.m_string);
    string.copy_to(output, capacity);
    return true;
}

bool parse_xml_lobby_party_leader(void *context, wchar_t *output, long capacity)
{
    bool result = false;
    if (user_interface_squad_exists()) {
        long leader_player_index = user_interface_squad_get_leader_player_index();
        if (leader_player_index != -1) {
            const s_player_configuration *leader = user_interface_squad_get_player_data(leader_player_index);
            if (leader != 0) {
                unsigned long size = capacity > 32UL ? 32UL : capacity;
                ustrnzcpy(output, leader->host.name, size);
                result = true;
            }
        }
    }
    return result;
}

// 0x4f3088 ?parse_xml_lobby_start_button_name@@YA_NPAXPA_WJ@Z
bool parse_xml_lobby_start_button_name(void *context, wchar_t *output, long capacity)
{
    bool result = false;
    c_static_wchar_string<1024> string;
    c_gui_screen_pregame_lobby *screen = (c_gui_screen_pregame_lobby *)context;
    if (screen->get_string_by_string_id(screen->get_start_button_name(), &string)) {
        string.copy_to(output, capacity);
        result = true;
    }
    return result;
}

// @address 0x4f3118 ?parse_xml_lobby_current_players@@YA_NPAXPA_WJ@Z
bool parse_xml_lobby_current_players(void *context, wchar_t *output, long capacity)
{
    bool result = false;

    if (user_interface_squad_exists())
    {
        usnzprintf(output, capacity, L"%d", ::user_interface_squad_get_player_count());
        result = true;
    }

    return result;
}


// @address 0x4f3178 ?parse_xml_lobby_max_players@@YA_NPAXPA_WJ@Z
bool parse_xml_lobby_max_players(void *context, wchar_t *output, long capacity)
{
    bool result = false;
    if (user_interface_squad_exists()) {
        usnzprintf(output, capacity, L"%d", network_squad_session_get_maximum_player_count());
        result = true;
    }
    return result;
}

// @address 0x4f31d8 ?parse_xml_lobby_countdown_remaining@@YA_NPAXPA_WJ@Z
bool parse_xml_lobby_countdown_remaining(void *context, wchar_t *output, long capacity)
{
    bool result = false;
    if (user_interface_squad_exists()) {
        usnzprintf(output, capacity, L"%d", maximum(::user_interface_squad_get_countdown_timer() - 1, 0L));
        result = true;
    }
    return result;
}

// 0x4f2b50 ?commit_team_change@c_gui_screen_pregame_lobby@@AAAXJPAVc_gui_roster_list_widget@@J@Z
// @address 0x4f2b50 gui_pregame_lobby_commit_team_change
extern "C" void gui_pregame_lobby_commit_team_change(long player_index, c_gui_roster_list_widget *roster_list, long timestamp)
{
    long controller_index = user_interface_session_get_controller_index(player_index);
    if ((unsigned long)controller_index <= 3) {
        long team_index = roster_list->m_teams[player_index].color;
        unsigned long milliseconds = g_user_interface_globals.milliseconds.peek();
        if (roster_list->m_teams[player_index].display_until >= milliseconds)
            team_index = roster_list->m_teams[player_index].color;
        network_session_interface_set_controller_desired_team(controller_index, team_index);
        user_interface_controller_update_network_properties((e_controller_index)controller_index);
        roster_list->m_teams[player_index].display_until = (unsigned long)timestamp;
        roster_list->m_teams[player_index].active = false;
        roster_list->m_teams[player_index].change_start_time = 0;
    }
}

// 0x4cbb48 ?create_list_widget@c_gui_screen_pregame_lobby@@UAAPAVc_gui_list_widget@@PBUs_list_widget_block@@@Z
// @address 0x4cbb48 ?create_list_widget@c_gui_screen_pregame_lobby@@UAAPAVc_gui_list_widget@@PBUs_list_widget_block@@@Z
c_gui_list_widget *c_gui_screen_pregame_lobby::create_list_widget(const s_list_widget_block *definition)
{
    if (definition->definition.core.name == _pregame_lobby_string_roster) {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_roster_list_widget), 0, 0);
        if (memory)
            return new (memory) c_gui_roster_list_widget(true, false);
    } else {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_list_widget), 0, 0);
        if (memory)
            return new (memory) c_gui_list_widget;
    }
    return 0;
}

// @address 0x4d7500 ?create_bitmap_widget@c_gui_screen_pregame_lobby@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
// PC build only: on Xbox the image holds one body for this name, c_gui_screen_pregame_selection::create_bitmap_widget.
#if !defined(_XBOX) || defined(__clang__)
c_gui_bitmap_widget *c_gui_screen_pregame_lobby::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _pregame_lobby_string_map_image) {
        return new(0, 0, _ui_allocation_marker_dummy) c_gui_custom_bitmap_widget();
    }
    return new(0, 0, _ui_allocation_marker_dummy) c_gui_bitmap_widget();
}
#endif

// @address 0x4da478 ?initialize_datasource@c_gui_screen_pregame_lobby@@UAAXXZ
// PC build only: on Xbox the image holds one body for this name, c_gui_screen_matchmaking_searching::initialize_datasource.
#if !defined(_XBOX) || defined(__clang__)
void c_gui_screen_pregame_lobby::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    void *memory = ::user_interface_malloc_tracked(sizeof(c_gui_squad_roster_data), (char const *)(0), 0);
    c_gui_data *data = memory != 0 ? new(memory) c_gui_squad_roster_data(get_driving_controller()) : 0;
    if (data != 0) {
        if (data->initialize(_pregame_lobby_string_roster))
            add_datasource(data);
        else {
            data->~c_gui_data();
            user_interface_free(data);
        }
    }
}
#endif


// 0x4f0f90 ?update@c_gui_screen_pregame_lobby@@UAAXK@Z
// @address 0x4f2bc8 gui_pregame_lobby_get_current_variant
extern "C" const c_game_variant *gui_pregame_lobby_get_current_variant()
{
    const c_game_variant *result = 0;
    if (network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_multiplayer ||
        network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_mapeditor ||
        network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_theater)
    {
        result = user_interface_game_settings_get_game_variant();
    }
    return result;
}

// @address 0x4f2c30 gui_pregame_lobby_get_current_map_id
extern "C" e_map_id gui_pregame_lobby_get_current_map_id()
{
    e_campaign_id campaign_id;
    e_map_id map_id;
    e_map_id result = _map_id_none;
    if (network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_multiplayer ||
        network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_mapeditor ||
        network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_campaign ||
        network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_theater)
    {
        if (network_life_cycle_session_get_map(&campaign_id, &map_id))
            result = map_id;
    }
    return result;
}

// @address 0x4f0f90 ?update@c_gui_screen_pregame_lobby@@UAAXK@Z
void c_gui_screen_pregame_lobby::update(unsigned long milliseconds)
{
    if (user_interface_squad_exists() && user_interface_get_session_game_mode() == _session_game_mode_pregame) {
        bool is_leader = ::network_life_cycle_squad_local_peer_is_leader();
        bool in_countdown = ::user_interface_squad_in_or_after_countdown();
        long countdown_timer = user_interface_squad_get_countdown_timer();
        c_gui_widget *player_list = this->get_child_widget(_gui_list_widget, _pregame_lobby_string_lobby_list);
        c_gui_widget *roster_list = this->get_child_widget(_gui_list_widget, _pregame_lobby_string_roster);
        c_gui_widget *map_image = this->get_child_widget(_gui_bitmap_widget, _pregame_lobby_string_map_image);
        e_map_id map_id = gui_pregame_lobby_get_current_map_id();
        bool teams_changed = false;
        if (user_interface_networking_get_closed())
            set_text_widget_string_id(_pregame_lobby_string_players_in_game, user_interface_squad_get_player_count() == 1
                    ? _pregame_lobby_string_players_in_game_singular_closed
                    : _pregame_lobby_string_players_in_game_plural_closed);
        else
            set_text_widget_string_id(_pregame_lobby_string_players_in_game, user_interface_squad_get_player_count() == 1
                    ? _pregame_lobby_string_players_in_game_singular
                    : _pregame_lobby_string_players_in_game_plural);
        bool list_enabled;
        if (is_leader && !::user_interface_squad_in_or_after_countdown())
            list_enabled = true;
        else
            list_enabled = false;
        c_gui_widget *start_list = this->get_child_widget(_gui_list_widget, _pregame_lobby_string_lobby_list);
        if (start_list != 0)
            start_list->set_enabled(list_enabled);
        c_gui_widget *start_text = this->get_child_widget(_gui_list_widget, _pregame_lobby_string_lobby_list);
        if (start_text != 0)
            start_text->set_visible(is_leader);
        c_gui_widget *options_text = this->get_child_widget(_gui_text_widget, _pregame_lobby_string_not_squad_leader);
        if (options_text != 0)
            options_text->set_visible(!is_leader);
        gui_screen_pregame_lobby_update_start_widget(this);
        gui_screen_pregame_lobby_update_count_badge(this, milliseconds);
        update_widget_visiblility();
        if (map_image != 0)
            ((c_gui_custom_bitmap_widget *)map_image)->set_map_image(_custom_map_image_type_normal, map_id, false);
        if (player_list != 0 && roster_list != 0 && !is_leader) {
            c_gui_widget *focus_list;
            long focused_handle;
            do {
                if (in_countdown && roster_list->within_focus_chain()) {
                    focused_handle = ((c_gui_list_widget *)player_list)->get_focused_element_handle();
                    focus_list = player_list;
                } else if (!in_countdown && player_list->within_focus_chain()) {
                    focused_handle = ((c_gui_list_widget *)roster_list)->get_focused_element_handle();
                    focus_list = roster_list;
                } else {
                    break;
                }
                transfer_focus_to_list((c_gui_list_widget *)focus_list, focused_handle, true, true);
            } while (false);
        }
        c_gui_widget *roster_options = this->get_child_widget(_gui_list_widget, _pregame_lobby_string_roster);
        if (roster_options != 0)
            roster_options->set_enabled(!in_countdown);
        if (roster_list != 0) {
            const c_game_variant *variant = gui_pregame_lobby_get_current_variant();
            if (team_switching_allowed() && variant != 0 && map_id != _map_id_none) {
                long period = get_constants_datasource_integer(_pregame_lobby_string_team_change_delay);
                long timeout = get_constants_datasource_integer(_pregame_lobby_string_team_change_lying_time);
                long max_teams = game_engine_variant_get_maximum_team_count(variant, map_id);
                bool show_teams = false;
                if (network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_multiplayer ||
                    network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_mapeditor)
                    show_teams = ::user_interface_networking_should_show_teams();
                if (!show_teams)
                    max_teams = 1;
                else if (max_teams == 0)
                    max_teams = 8;
                long i = 0;
                s_roster_team *entry = ((c_gui_roster_list_widget *)roster_list)->m_teams;
                for (; i < 16; i++, entry++) {
                    if (entry->active) {
                        long team_index = entry->color;
                        unsigned long now = g_user_interface_globals.milliseconds.peek();
                        if (entry->display_until >= now)
                            team_index = entry->color;
                        teams_changed = true;
                        if (team_index < 0 || team_index >= max_teams) {
                            long clamped = team_index > 0 ? team_index : 0;
                            if (clamped < max_teams - 1) {
                                if (team_index <= 0)
                                    team_index = 0;
                            } else {
                                team_index = max_teams - 1;
                            }
                            entry->active = true;
                            entry->color = team_index;
                            entry->change_start_time = milliseconds;
                        }
                        if ((unsigned long)(milliseconds - entry->change_start_time) >= (unsigned long)period) {
                            gui_pregame_lobby_commit_team_change(i, (c_gui_roster_list_widget *)roster_list, timeout + milliseconds);
                        } else if (in_countdown) {
                            entry->active = false;
                            entry->change_start_time = 0;
                            entry->display_until = 0;
                        }
                    }
                }
            }
        }
        long group_id;
        if (teams_changed) {
            if (::online_local_xuid_is_online_enabled())
                group_id = _pregame_lobby_string_pregame_lobby_online_team_change;
            else
                group_id = _pregame_lobby_string_pregame_lobby_offline_team_change;
        } else if (::online_local_xuid_is_online_enabled()) {
            group_id = _pregame_lobby_string_pregame_lobby_online;
        } else {
            group_id = _pregame_lobby_string_pregame_lobby_offline;
        }
        gui_screen_widget_set_display_group(this, 1, group_id);
        if (in_countdown && countdown_timer != -1 && countdown_timer != 0 && countdown_timer != m_countdown_time) {
            long max_time = get_constants_datasource_integer(_pregame_lobby_string_fade);
            if (countdown_timer <= get_constants_datasource_integer(_pregame_lobby_string_warn) + 1) {
                if (network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_matchmaking)
                    ::user_interface_play_sound((e_user_interface_sound_effect)((countdown_timer == 1) + 15),
                        m_core_definition.sound_override_index);
                else
                    ::user_interface_play_sound((e_user_interface_sound_effect)((countdown_timer == 1) + 13),
                        m_core_definition.sound_override_index);
            }
            if (m_countdown_teams_visible && countdown_timer > max_time) {
                stop_fade_during_countdown();
                m_countdown_teams_visible = false;
            } else if (countdown_timer == max_time) {
                start_fade_during_countdown();
                m_countdown_teams_visible = true;
            }
            m_countdown_time = countdown_timer;
        } else if (!in_countdown && m_countdown_teams_visible) {
            stop_fade_during_countdown();
            m_countdown_teams_visible = false;
            m_countdown_time = 0;
        }
    }
    
    c_gui_screen_widget::update(milliseconds);
}

static __forceinline c_gui_data *campaign_settings_get_list_data(c_gui_list_widget *list)
{
    long datasource_index = -1;
    c_gui_screen_widget *screen = list->get_parent_screen();
    c_gui_data *result;
    if (screen == 0)
        result = 0;
    else
        result = screen->get_data(list->m_datasource_name, &datasource_index);
    return result;
}

static __forceinline void campaign_settings_update_skull_items(c_gui_list_widget *list, c_gui_data *data, long value_name)
{
    if (list == 0 || data == 0)
        return;
    for (c_gui_widget *item = gui_widget_find_child_by_name(list, _gui_list_item_widget);
        item != 0; item = gui_widget_get_next_list_item(item, true))
    {
        long value;
        gui_widget_set_alternate_state(item,
            (data->get_integer_value(item->get_element_handle(), value_name, &value) &&
                value != 2 && value != 0)
                ? false
                : true);
    }
}

// @address 0x4c26c0 ?update@c_gui_screen_campaign_settings@@UAAXK@Z
void c_gui_screen_campaign_settings::update(unsigned long current_milliseconds)
{
    c_gui_list_widget *primary_list =
        static_cast<c_gui_list_widget *>(get_child_widget(_gui_list_widget, _player_select_string_primary_skulls));
    c_gui_list_widget *secondary_list =
        static_cast<c_gui_list_widget *>(get_child_widget(_gui_list_widget, _player_select_string_secondary_skulls));
    c_gui_data *primary_data = 0;
    c_gui_data *secondary_data = 0;
    c_gui_screen_widget::update(current_milliseconds);
    if (primary_list != 0)
        primary_data = primary_list->get_data();
    if (secondary_list != 0)
        secondary_data = secondary_list->get_data();
    campaign_settings_update_skull_items(primary_list, primary_data, _player_select_string_primary_skull_state);
    campaign_settings_update_skull_items(secondary_list, secondary_data, _player_select_string_secondary_skull_state);

    c_gui_list_widget *scoring_list =
        static_cast<c_gui_list_widget *>(get_child_widget(_gui_list_widget, _player_select_string_scoring_option));
    c_gui_list_widget *spinner =
        static_cast<c_gui_list_widget *>(get_child_widget(_gui_list_widget, _player_select_string_spinner));
    c_gui_data *scoring_data = scoring_list != 0 ? campaign_settings_get_list_data(scoring_list) : 0;
    long variant_index = network_life_cycle_session_get_game_variant();
    if (spinner != 0 && spinner->m_visible)
    {
        scoring_list->set_focused_element_handle(spinner->get_focused_element_handle(), false);
        return;
    }
    if (scoring_list != 0 && scoring_data != 0 && variant_index >= 0 && variant_index < scoring_data->get_current_item_count())
        scoring_list->set_focused_element_handle(variant_index, false);
}


static __forceinline bool gui_screen_pregame_lobby_session_in_game(const c_network_session *session)
{
    {
        bool range_value = false;
        switch (session->m_local_state)
        {
        case _network_session_state_none:
        case _network_session_state_peer_creating:
        case _network_session_state_peer_joining:
        case _network_session_state_peer_join_abort:
        case _network_session_state_election:
            range_value = false;
            break;
        case _network_session_state_peer_established:
        case _network_session_state_peer_leaving:
        case _network_session_state_host_established:
        case _network_session_state_host_disband:
        case _network_session_state_host_handoff:
        case _network_session_state_host_reestablish:
            range_value = true;
            break;
        default:
            // match: 0x4f33e0: unreachable hint omits dead branch check
            __assume(0);
        }
        return range_value;
    }
}


class c_game_results;

// @address 0x4f1f60 ?handle_controller_input_message@c_gui_screen_pregame_lobby@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_pregame_lobby::handle_controller_input_message(const c_controller_input_message *message)
{
    bool bypass_base = false;
    bool handled = false;
    bool in_countdown = ::user_interface_squad_in_or_after_countdown();
    if (message->m_event_type == 2 || message->m_event_type == 4) {
        if (!in_countdown) {
            c_gui_widget *player_list = this->get_child_widget(_gui_list_widget, _pregame_lobby_string_lobby_list);
            c_gui_widget *roster_list = this->get_child_widget(_gui_list_widget, _pregame_lobby_string_roster);
            const c_gui_screen_widget *view = this;
            c_gui_widget *focused; if (view->m_suppress_focus) focused = 0; else focused = view->m_focused_widget;
            if (player_list != 0 && roster_list != 0 && focused != 0 && focused->m_type == _gui_list_item_widget) {
                bool in_player_list = gui_widget_get_parent_list_widget(focused) == player_list;
                long handle;
                if (message->m_event_type == 4 && in_player_list) {
                    handle = ((c_gui_list_widget *)roster_list)->get_focused_element_handle();
                    transfer_focus_to_list((c_gui_list_widget *)roster_list, handle, true, true);
                    handled = true;
                } else if (message->m_event_type == 2 && !in_player_list && ::network_life_cycle_squad_local_peer_is_leader()) {
                    handle = ((c_gui_list_widget *)player_list)->get_focused_element_handle();
                    transfer_focus_to_list((c_gui_list_widget *)player_list, handle, true, true);
                    handled = true;
                }
            }
        }
    } else if (message->m_event_type == _event_type_button_press) {
        switch (message->m_component) {
        case 0:
            if (in_countdown) {
                ::user_interface_squad_start_countdown_timer(message->m_controller, 6, 4);
                bypass_base = true;
            } else if (team_switching_allowed()) {
                c_gui_widget *roster = this->get_child_widget(_gui_list_widget, _pregame_lobby_string_roster);
                if (roster != 0) {
                    long player = ::user_interface_session_get_player_at_local_controller(message->m_controller);
                    if (player != -1 && ((c_gui_roster_list_widget *)roster)->m_teams[player].active) {
                        long timeout = get_constants_datasource_integer(_pregame_lobby_string_team_change_lying_time);
                        // match: 0x4f1f60: volatile pointer cast preserves memory access order
                        unsigned long now = g_user_interface_globals.milliseconds.peek();
                        gui_pregame_lobby_commit_team_change(player, (c_gui_roster_list_widget *)roster, now + timeout);
                        handled = true;
                    }
                }
            }
            break;
        case 1:
            if (!in_countdown && team_switching_allowed()) {
                c_gui_widget *roster = this->get_child_widget(_gui_list_widget, _pregame_lobby_string_roster);
                if (roster != 0) {
                    long player = ::user_interface_session_get_player_at_local_controller(message->m_controller);
                    if (player != -1 && ((c_gui_roster_list_widget *)roster)->m_teams[player].active) {
                        ((c_gui_roster_list_widget *)roster)->m_teams[player].active = false;
                        handled = true;
                        ((c_gui_roster_list_widget *)roster)->m_teams[player].change_start_time = 0;
                        ((c_gui_roster_list_widget *)roster)->m_teams[player].display_until = 0;
                    }
                }
            }
            if (!handled) {
                if (in_countdown && ::network_life_cycle_squad_local_peer_is_leader())
                    ::user_interface_squad_stop_countdown_timer(message->m_controller, -1, false);
                else
                    handle_back_out(message->m_controller);
                handled = true;
            }
            break;
        case 2:
            if (in_countdown)
                ::user_interface_squad_stop_countdown_timer(message->m_controller, -1, false);
            handled = true;
            break;
        case 3:
            if (::online_local_xuid_is_online_enabled())
                online_guide_show_friends_ui(message->m_controller);
            handled = true;
            break;
        case _controller_component_button_back:
            if (!in_countdown && user_interface_networking_get_final_game_results() != 0) {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_screen_message), 0, 0);
                if (memory != 0) {
                    long name = this->name;
                    c_load_screen_message *load = new (memory, s_user_interface_networking_placement()) c_load_screen_message(
                        _pregame_lobby_string_carnage_report, k_any_controller, this->get_render_window(), name);
                    if (load != 0)
                        ::user_interface_messaging_post(load);
                }
                handled = true;
            }
            break;
        case 4:
        case 5:
            if (!in_countdown && team_switching_allowed()) {
                c_gui_widget *roster = this->get_child_widget(_gui_list_widget, _pregame_lobby_string_roster);
                const c_game_variant *variant = gui_pregame_lobby_get_current_variant();
                e_map_id map_id = gui_pregame_lobby_get_current_map_id();
                if (roster != 0 && variant != 0 && map_id != _map_id_none) {
                    long player = ::user_interface_session_get_player_at_local_controller(message->m_controller);
                    if (player != -1) {
                        const s_player_configuration *configuration = user_interface_session_get_player_data(player);
                        long max_teams = game_engine_variant_get_maximum_team_count(variant, map_id);
                        bool show_teams = false;
                        if (network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_multiplayer ||
                            network_life_cycle_squad_session_get_ui_game_mode() == _gui_game_mode_mapeditor)
                            show_teams = ::user_interface_networking_should_show_teams();
                        if (!show_teams) max_teams = 1;
                        else if (max_teams == 0) max_teams = 8;
                        long team_index;
                        {
                            unsigned long now;
                            // match: 0x4f1f60: volatile pointer cast preserves memory access order
                            if (!((c_gui_roster_list_widget *)roster)->m_teams[player].active
                                && (now = g_user_interface_globals.milliseconds.value,
                                    __lwsync(),
                                    ((c_gui_roster_list_widget *)roster)->m_teams[player].display_until < now))
                                team_index = configuration->host.team_index;
                            else {
                                team_index = ((c_gui_roster_list_widget *)roster)->m_teams[player].color;
                                // match: 0x4f1f60: volatile pointer cast preserves memory access order
                                now = g_user_interface_globals.milliseconds.peek();
                                if (((c_gui_roster_list_widget *)roster)->m_teams[player].display_until >= now)
                                    team_index = ((c_gui_roster_list_widget *)roster)->m_teams[player].color;
                            }
                        }
                        long selected_team = (team_index > 0 ? team_index : 0) < max_teams - 1 ? (team_index > 0 ? team_index : 0) : max_teams - 1;
                        long delta = message->m_component == 5 ? -1 : 1;
                        long desired_team = delta + selected_team;
                        if (desired_team == -1) desired_team = max_teams - 1;
                        else if (desired_team == max_teams) desired_team = 0;
                        // match: 0x4f1f60: volatile pointer cast preserves memory access order
                        unsigned long now = g_user_interface_globals.milliseconds.peek();
                        ((c_gui_roster_list_widget *)roster)->m_teams[player].active = true;
                        ((c_gui_roster_list_widget *)roster)->m_teams[player].color = desired_team;
                        ((c_gui_roster_list_widget *)roster)->m_teams[player].change_start_time = now;
                        handled = true;
                    }
                }
            }
            break;
        }
    }
    if (!handled && !bypass_base)
        return c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

// @address 0x4f33e0 parse_xml_lobby_percent_loaded
extern "C" bool parse_xml_lobby_percent_loaded(void *context, wchar_t *output, long capacity)
{
    bool result = false;
    if (user_interface_squad_exists()) {
        c_network_session *session = 0;
        bool in_squad_session = false;
        if (g_network_life_cycle_globals.initialized) {
            c_network_session *current = g_network_life_cycle_globals.manager.m_active_squad_session;
            if (long state_value = current->m_local_state) {
                session = current;
                in_squad_session = true;
            }
        }
        if (in_squad_session && gui_screen_pregame_lobby_session_in_game(session)) {
            const s_network_session_parameter_game_start_status *value = session->get_session_parameters()->m_parameters.game_start_status.get();
            if (value && value->error == 13) {
                long percent = value->map_load_progress;
                if (percent != -1) {
                    if (percent == 0)
                        usnzprintf(output, capacity, L"0");
                    else {
                        percent = (percent > 0 ? percent : 0) < 100 ? (percent > 0 ? percent : 0) : 100;
                        usnzprintf(output, capacity, L"%d", percent);
                    }
                    result = true;
                }
            }
        }
    }
    return result;
}

typedef char pregame_roster_team_index_offset[(offsetof(s_roster_team, color) == 4) ? 1 : -1];
typedef char pregame_roster_team_size[(sizeof(s_roster_team) == 0x10) ? 1 : -1];
