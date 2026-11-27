#include "interface/user_interface_matchmaking_progress.h"
#ifndef INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_POSTGAME_LOBBY_H
#define INTERFACE_GUI_SCREENS_PREGAME_LOBBY_GUI_SCREEN_POSTGAME_LOBBY_H

#include "cseries/cseries.h"
#include "interface/c_gui_screen_widget.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "interface/user_interface_matchmaking.h"
#include "networking/logic/network_session_interface.h"
#include "networking/logic/storage/network_http_blf_simple_downloader.h"
#include "networking/logic/storage/dynamic_matchmaking_hopper_statistics_types.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"

struct s_hopper_information;
// String ids this screen names its widgets, lists, columns, status strings and
// dialogs by; each is named after its string in the cache string table.
enum e_postgame_lobby_string_id
{
    _postgame_lobby_string_player_index = 0x10014,
    _postgame_lobby_string_status = 0x1002b,
    _postgame_lobby_string_progress = 0x1002d,
    _postgame_lobby_string_timer = 0x1003b,
    _postgame_lobby_string_player_select = 0x100af,
    _postgame_lobby_string_party_leader = 0x1033f,
    _postgame_lobby_string_party_leader_underline = 0x10340,
    _postgame_lobby_string_hopper_name = 0x10345,
    _postgame_lobby_string_roster = 0x103a7,
    _postgame_lobby_string_tips = 0x103e0,
    _postgame_lobby_string_matchmaking_postgame_status_rematch_disabled = 0x10417,
    _postgame_lobby_string_matchmaking_postgame_status_prepare_to_deploy = 0x10418,
    _postgame_lobby_string_matchmaking_postgame_status_party_has_voted_to_join = 0x10419,
    _postgame_lobby_string_matchmaking_postgame_status_press_x_to_join_ranked = 0x1041a,
    _postgame_lobby_string_matchmaking_postgame_status_press_x_to_join_unranked = 0x1041b,
    _postgame_lobby_string_matchmaking_postgame_status_party_leader_can_join = 0x1041c,
    _postgame_lobby_string_matchmaking_postgame_status_preparing_for_rematch = 0x1041d,
    _postgame_lobby_string_matchmaking_postgame_status_preparing_for_rematch_voted = 0x1041e,
    _postgame_lobby_string_matchmaking_postgame_tip_ranked_team = 0x1041f,
    _postgame_lobby_string_matchmaking_postgame_tip_ranked_ffa = 0x10420,
    _postgame_lobby_string_matchmaking_postgame_tip_unranked_team = 0x10421,
    _postgame_lobby_string_matchmaking_postgame_tip_unranked_ffa = 0x10422,
    _postgame_lobby_string_timer_waiting = 0x10423,
    _postgame_lobby_string_timer_next_match = 0x10424,
    _postgame_lobby_string_hopper_with_population = 0x10425,
    _postgame_lobby_string_preparing_for_rematch = 0x10426,
    _postgame_lobby_string_leave_postgame_lobby = 0x30003,
    _postgame_lobby_string_post_game_leave_voted_to_party_up = 0x30019,
    _postgame_lobby_string_need_to_select_storage_device_to_save = 0x3001e,
};


class c_gui_screen_postgame_lobby : public c_gui_screen_widget {
public:
 __forceinline c_gui_screen_postgame_lobby(long name) :
  c_gui_screen_widget(name),
  m_statistics_downloader(),
  previous_seconds(NONE),
  maximum_remaining(1),
  previous_requested_player_mask(0)
 {
 }
 virtual void initialize();
 virtual void post_initialize();
 virtual c_gui_list_widget *create_list_widget(const s_list_widget_block *definition);
 virtual void update(unsigned long current_milliseconds);
 virtual e_gui_location get_gui_location() const { return _gui_location_postgame_lobby; }
 virtual void initialize_datasource();
 virtual bool handle_controller_input_message(const c_controller_input_message *message);
 virtual bool handle_dialog_result(const c_dialog_result_message *message);
 virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item, c_gui_data *data);
 protected:
 virtual e_render_data_size get_render_data_size();
 public:
 __forceinline const s_matchmaking_single_hopper_statistics *get_hopper_statistics() const
 {
  const s_matchmaking_single_hopper_statistics *statistics = 0;
  if (m_hopper_identifier != 0xffff && m_hopper_identifier == network_squad_session_get_hopper())
   statistics = &m_hopper_statistics;
  return statistics;
 }
private:
 void update_status(unsigned short hopper_id, s_hopper_information *hopper, s_life_cycle_matchmaking_progress *progress);
 void update_timer(unsigned long current_time, unsigned short hopper_id, s_hopper_information *hopper_information, s_life_cycle_matchmaking_progress *progress);
 public:
 c_http_blf_simple_downloader<s_dynamic_matchmaking_hopper_statistics> m_statistics_downloader;
 s_life_cycle_matchmaking_progress m_progress;
 long previous_seconds;
 unsigned short m_hopper_identifier;
 s_matchmaking_single_hopper_statistics m_hopper_statistics;
 long maximum_remaining;
 long previous_requested_player_mask;
};


#endif
