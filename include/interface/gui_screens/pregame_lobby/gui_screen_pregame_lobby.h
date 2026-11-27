#ifndef INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_H
#define INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_H

#include "cseries/cseries.h"
#include "interface/c_gui_screen_widget.h"
#include "game/game_types.h"
#include <stddef.h>

// Lobby status texts (string ids 0x5xxxx), back-out dialogs (0x3xxxx), and screen, widget, column,
// datasource and display-group names (0x0xxxx/0x1xxxx), named after the string table.
enum e_pregame_lobby_string_id
{
    _pregame_lobby_string_status_none = 0x50000,
    _pregame_lobby_string_status_not_pregame = 0x50001,
    _pregame_lobby_string_status_join_in_progress = 0x50002,
    _pregame_lobby_string_status_loading = 0x50003,
    _pregame_lobby_string_error_none = 0x50004,
    _pregame_lobby_string_error_no_map_selected = 0x50005,
    _pregame_lobby_string_error_map_load_failure = 0x50006,
    _pregame_lobby_string_error_invalid_film_selected = 0x50007,
    _pregame_lobby_string_error_no_film_selected = 0x50008,
    _pregame_lobby_string_error_too_many_teams = 0x5000a,
    _pregame_lobby_string_error_all_players_are_observers = 0x5000b,
    _pregame_lobby_string_error_too_many_local_players_for_coop = 0x5000c,
    _pregame_lobby_string_error_too_many_players_for_network_coop = 0x5000d,
    _pregame_lobby_string_error_players_incompatible_for_network_coop = 0x5000e,
    _pregame_lobby_string_error_account_not_online_enabled = 0x5000f,
    _pregame_lobby_string_error_network_configuration_pending = 0x50010,
    _pregame_lobby_string_error_network_configuration_unavailable = 0x50011,
    _pregame_lobby_string_error_banhammer_pending = 0x50012,
    _pregame_lobby_string_error_banhammer_unavailable = 0x50013,
    _pregame_lobby_string_error_player_stats_pending = 0x50014,
    _pregame_lobby_string_error_player_stats_unavailable = 0x50015,
    _pregame_lobby_string_match_error_master_hopper_file_pending = 0x50016,
    _pregame_lobby_string_match_error_master_hopper_file_unavailable = 0x50017,
    _pregame_lobby_string_match_error_hopper_specific_file_pending = 0x50018,
    _pregame_lobby_string_match_error_hopper_specific_file_unavailable = 0x50019,
    _pregame_lobby_string_match_error_invalid_hopper = 0x5001a,
    _pregame_lobby_string_match_error_squad_too_large = 0x5001b,
    _pregame_lobby_string_match_error_squad_too_small = 0x5001c,
    _pregame_lobby_string_match_error_experience_too_low = 0x5001d,
    _pregame_lobby_string_match_error_experience_too_high = 0x5001e,
    _pregame_lobby_string_match_error_access_bit_not_set = 0x5001f,
    _pregame_lobby_string_match_error_unpaid_in_paid_hopper = 0x50020,
    _pregame_lobby_string_match_error_paid_in_unpaid_hopper = 0x50021,
    _pregame_lobby_string_match_error_player_not_in_required_region = 0x50022,
    _pregame_lobby_string_match_error_player_not_in_required_country_code = 0x50023,
    _pregame_lobby_string_match_error_player_not_in_required_language = 0x50024,
    _pregame_lobby_string_match_error_player_not_in_required_gamerzone = 0x50025,
    _pregame_lobby_string_match_error_guest_player_when_not_allowed = 0x50026,
    _pregame_lobby_string_match_error_not_beta_player_in_beta_hopper = 0x50027,
    _pregame_lobby_string_match_error_player_missing_content = 0x50028,
    _pregame_lobby_string_match_error_player_missing_files = 0x50029,
    _pregame_lobby_string_match_error_player_banned_from_matchmaking = 0x5002a,
    _pregame_lobby_string_match_error_not_yet_start_time = 0x5002b,
    _pregame_lobby_string_match_error_end_time_has_passed = 0x5002c,
    _pregame_lobby_string_error_custom_games_are_disabled_for_alpha = 0x5002d,
    _pregame_lobby_string_error_maximum_multiplayer_split_screen_exceeded = 0x5002e,
    _pregame_lobby_string_match_error_must_have_online_enabled_profiles_for_match = 0x5002f,
    _pregame_lobby_string_match_error_must_connect_to_live_for_match = 0x50030,
    _pregame_lobby_string_match_error_must_have_live_for_match = 0x50031,
    _pregame_lobby_string_must_be_connected_to_live_in_live_lobby = 0x50032,
    _pregame_lobby_string_must_be_connected_to_live_for_alpha = 0x50033,
    _pregame_lobby_string_only_one_player_allowed_in_theater_for_alpha = 0x50034,
    _pregame_lobby_string_error_film_too_many_players = 0x50035,
    _pregame_lobby_string_error_film_harddrive_required = 0x50036,
    _pregame_lobby_string_error_film_party_leader_requirement = 0x50037,
    _pregame_lobby_string_error_film_all_players_not_compatible = 0x50038,
    _pregame_lobby_string_status_unknown = 0x50039,
    _pregame_lobby_string_status_changing_settings_member = 0x5003a,
    _pregame_lobby_string_status_ready_leader = 0x5003b,
    _pregame_lobby_string_status_ready_member = 0x5003c,
    _pregame_lobby_string_status_countdown_leader = 0x5003d,
    _pregame_lobby_string_status_countdown_member = 0x5003e,
    _pregame_lobby_string_status_countdown_delayed = 0x5003f,
    _pregame_lobby_string_error_unknown = 0x50040,
    _pregame_lobby_string_match_error_player_missing_required_maps = 0x50041,
    _pregame_lobby_string_match_error_games_played_too_low = 0x50042,
    _pregame_lobby_string_match_error_games_played_too_high = 0x50043,
    _pregame_lobby_string_error_forge_too_many_players = 0x50044,
    _pregame_lobby_string_error_map_load_precaching = 0x50045,
    _pregame_lobby_string_error_user_content_not_permitted = 0x50046,
    _pregame_lobby_string_match_error_coop_player_missing_hdd = 0x50047,
    _pregame_lobby_string_match_error_coop_player_hdd_mismatch = 0x50048,
    _pregame_lobby_string_match_error_coop_player_language_mismatch = 0x50049,
    _pregame_lobby_string_match_error_invalid_film_selected_language = 0x5004a,
    _pregame_lobby_string_error_signedin_controller_detatched = 0x5004b,
    _pregame_lobby_string_back_out_lobby_local_players_with_profile = 0x3000c,
    _pregame_lobby_string_back_out_lobby_local_players_no_profile = 0x3000d,
    _pregame_lobby_string_back_out_lobby_remote_players = 0x3000e,
    _pregame_lobby_string_need_to_select_storage_device_to_save = 0x3001e,
    _pregame_lobby_string_warn = 60,
    _pregame_lobby_string_change_teams = 0x107,
    _pregame_lobby_string_not_squad_leader = 0x109,
    _pregame_lobby_string_name = 0x29c,
    _pregame_lobby_string_multiplayer = 0x333,
    _pregame_lobby_string_spinner = 0x10006,
    _pregame_lobby_string_target = 0x10011,
    _pregame_lobby_string_player_index = 0x10014,
    _pregame_lobby_string_fade = 0x10023,
    _pregame_lobby_string_precaching = 0x10041,
    _pregame_lobby_string_pregame_lobby_offline = 0x10059,
    _pregame_lobby_string_pregame_lobby_online = 0x1005a,
    _pregame_lobby_string_pregame_lobby_offline_team_change = 0x1005b,
    _pregame_lobby_string_pregame_lobby_online_team_change = 0x1005c,
    _pregame_lobby_string_matchmaking = 0x10073,
    _pregame_lobby_string_campaign = 0x10074,
    _pregame_lobby_string_mapeditor = 0x10078,
    _pregame_lobby_string_theater = 0x10079,
    _pregame_lobby_string_pregame_switch_lobby = 0x100ad,
    _pregame_lobby_string_player_select = 0x100af,
    _pregame_lobby_string_carnage_report = 0x100b9,
    _pregame_lobby_string_lobby_list = 0x102a7,
    _pregame_lobby_string_switch_lobby = 0x102a8,
    _pregame_lobby_string_start_game = 0x102b3,
    _pregame_lobby_string_lobby_status = 0x102bc,
    _pregame_lobby_string_advanced_options = 0x102bd,
    _pregame_lobby_string_postgame_stats = 0x102be,
    _pregame_lobby_string_players_in_game = 0x102c9,
    _pregame_lobby_string_players_in_game_singular = 0x102ca,
    _pregame_lobby_string_players_in_game_plural = 0x102cb,
    _pregame_lobby_string_players_in_game_singular_closed = 0x102cc,
    _pregame_lobby_string_players_in_game_plural_closed = 0x102cd,
    _pregame_lobby_string_team_change_delay = 0x102f8,
    _pregame_lobby_string_team_change_lying_time = 0x102f9,
    _pregame_lobby_string_map_image = 0x1032a,
    _pregame_lobby_string_game_options = 0x10384,
    _pregame_lobby_string_network_mode = 0x1038c,
    _pregame_lobby_string_network_mode_offline = 0x10392,
    _pregame_lobby_string_network_mode_xbox_live_advertise_anyone = 0x10394,
    _pregame_lobby_string_network_mode_xbox_live_advertise_friends = 0x10396,
    _pregame_lobby_string_network_mode_xbox_live_advertise_invite_only = 0x10398,
    _pregame_lobby_string_network_mode_system_link_advertise = 0x1039c,
    _pregame_lobby_string_roster = 0x103a7,
    _pregame_lobby_string_primary_skulls = 0x1046c,
    _pregame_lobby_string_primary_skull_state = 0x1046e,
    _pregame_lobby_string_secondary_skulls = 0x1046f,
    _pregame_lobby_string_secondary_skull_state = 0x10471,
    _pregame_lobby_string_scoring_option = 0x1047e,
};

class c_game_variant;
class c_gui_roster_list_widget;

// Shared lobby storage from the owning unit. March's method inventory attests
// this base, which holds the countdown and widget visibility state.
class __declspec(novtable) c_gui_screen_pregame_lobby : public c_gui_screen_widget {
public:
    __forceinline c_gui_screen_pregame_lobby(long name) : c_gui_screen_widget(name), m_countdown_time(NONE),
        m_countdown_teams_visible(false), start_visible(false), options_visible(false), results_visible(false) {}
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void initialize();
    virtual void post_initialize();
    virtual void dispose();
    virtual void update(unsigned long milliseconds);
    virtual void initialize_datasource();
    virtual e_gui_location get_gui_location() const { return _gui_location_pregame_lobby; }
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *data);
    virtual bool handle_dialog_result(const c_dialog_result_message *dialog_result_message);
    virtual void submenu_invoked(c_gui_list_widget *submenu_widget);
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    virtual c_gui_list_widget *create_list_widget(const s_list_widget_block *definition);
protected:
    virtual e_render_data_size get_render_data_size();
    virtual bool team_switching_allowed();
public:
    virtual bool advanced_options_enabled() { return true; }
    virtual long get_lobby_header() = 0;
    virtual long get_lobby_title() = 0;
    virtual long get_start_button_name() = 0;
    virtual long get_start_status_text(bool is_leader) = 0;
    virtual long get_gui_game_mode() = 0;
    virtual void start_fade_during_countdown();
    virtual void stop_fade_during_countdown();
protected:
    const c_game_variant *get_current_variant();
    e_map_id get_current_map_id();
    void load_game_options(e_controller_index controller_index);
private:
    bool handle_back_out(e_controller_index controller_index);
    void update_widget_visiblility();
    long m_countdown_time;
    bool m_countdown_teams_visible;
    bool start_visible;
    bool options_visible;
    bool results_visible;
};

typedef char pregame_lobby_base_size[(sizeof(void *) != 4 || sizeof(c_gui_screen_pregame_lobby) == 0x1a90) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/gui_screen_pregame_lobby.cpp.

class c_gui_screen_widget;
class c_gui_widget;

extern "C" void gui_screen_pregame_lobby_update_start_widget(c_gui_screen_widget *screen);
extern "C" void gui_screen_pregame_lobby_update_count_badge(c_gui_widget *widget, long milliseconds);
extern "C" unsigned int gui_screen_pregame_lobby_get_start_label(unsigned char alternate);
extern "C" const c_game_variant *gui_pregame_lobby_get_current_variant();
extern "C" bool parse_xml_lobby_header(void *context, wchar_t *output, long capacity);
extern "C" bool parse_xml_lobby_title(void *context, wchar_t *output, long capacity);
extern "C" bool parse_xml_lobby_network(void *context, wchar_t *output, long capacity);
bool parse_xml_lobby_party_leader(void *context, wchar_t *output, long capacity);
bool parse_xml_lobby_start_button_name(void *context, wchar_t *output, long capacity);
bool parse_xml_lobby_current_players(void *context, wchar_t *output, long capacity);
bool parse_xml_lobby_max_players(void *context, wchar_t *output, long capacity);
bool parse_xml_lobby_countdown_remaining(void *context, wchar_t *output, long capacity);
extern "C" bool parse_xml_lobby_percent_loaded(void *context, wchar_t *output, long capacity);

#endif // INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_PREGAME_LOBBY_H
