// interfacelibxbox/gui_screen_postgame_lobby.obj: from the records 0x4cb9a0 0x4cbb18 0x4cbbd8 0x4cbe70 0x4cbf70 0x4cbff0 0x4cc260 0x4cc310 0x4cc598 0x4cc6e0 0x4cc770 0x4cc818 0x4cc880
// @flags /TP /O2 /Oc
#include "interface/gui_render_data_size_types.h"
#include "interface/user_interface_controller_globals.h"
#include "interface/user_interface_matchmaking_progress.h"
#include "cseries/cseries_unicode.h"
#include <stddef.h>
#include "interface/hopper_information_types.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_postgame_lobby.h"
#include "interface/user_interface_squad_session.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_sound.h"
#include "game/player_types.h"
#include "shell/shell.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_data.h"
#include "memory/static_arrays.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "interface/user_interface_matchmaking.h"
#include "interface/user_interface_networking.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/online/online_xenon.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_membership.h"
#include "saved_games/content_catalogue.h"
#include "interface/game_tag_parser.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/list_widget_definition.h"
#include "interface/gui_roster_list_externs.h"
#include "interface/gui_roster_data.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "interface/ui_allocation_marker_types.h"
#include "interface/user_interface_allocation.h"
#include "interface/gui_screens/postgame_lobby/gui_screen_postgame_lobby.h"
#include "networking/online/online_guide_xenon.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"
#include "math/integer_math_templates.h"
#include "tag_files/string_ids.h"
#include "interface/gui_location_manager.h"
#include "game/game_variant.h"
#include <new>
#include "interface/gui_roster_list_externs.h"
#include "interface/c_gui_text_widget.h"
#include "interface/gui_screens/player_select/player_select_screen_message.h"
typedef char postgame_roster_party_indicators_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_roster_list_widget, m_show_party_indicators) == 0x143))) ? 1 : -1];

class c_message;

static bool parse_xml_hopper_name(void *context, wchar_t *buffer, long count);
static bool parse_xml_hopper_population(void *context, wchar_t *buffer, long count);
static bool parse_xml_party_leader(void *context, wchar_t *buffer, long count);
enum e_user_interface_sound_effect;
// @address 0x4cb9a0 ?initialize@c_gui_screen_postgame_lobby@@UAAXXZ
// @address 0x4cbb18 ?post_initialize@c_gui_screen_postgame_lobby@@UAAXXZ
// @address 0x4cbf70 gui_screen_postgame_lobby_has_leader_rematch_request
// @address 0x4cbff0 ?update_timer@c_gui_screen_postgame_lobby@@AAAXKGPAUs_hopper_information@@PAUs_life_cycle_matchmaking_progress@@@Z
// @address 0x4cc260 ?handle_dialog_result@c_gui_screen_postgame_lobby@@UAA_NPBVc_dialog_result_message@@@Z
// @address 0x4cc598 ?handle_list_item_chosen@c_gui_screen_postgame_lobby@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
// @address 0x4cc6e0 ?parse_xml_hopper_name@@YA_NPAXPA_WJ@Z
// @address 0x4cc770 ?parse_xml_hopper_population@@YA_NPAXPA_WJ@Z
// @address 0x4cc880 gui_screen_postgame_lobby_get_session_value_text

static __forceinline wchar_t *gui_screen_postgame_lobby_ustrncpy(wchar_t *dest,const wchar_t *source,long count) {
 for(long index=0;index<=count;index++) { if(!(dest[index]=*source)) break; ++source; }
 return dest;
}


// @address 0x7c341c g_lobby_location_manager
class c_gui_screen_postgame_lobby;
template<typename t_blf_type> class c_http_blf_simple_downloader;
struct s_dynamic_matchmaking_hopper_statistics;



class c_controller_input_message;
class c_gui_list_item_widget;



class c_deletable;


void c_gui_screen_postgame_lobby::initialize()
{
 c_gui_screen_widget::initialize();
 add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<hopper-name", this, parse_xml_hopper_name));
 add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
         c_magic_string_game_tag_parser(L"<hopper-population", this, parse_xml_hopper_population));
 add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
         c_magic_string_game_tag_parser(L"<party-leader", this, parse_xml_party_leader));
 add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
         c_magic_string_game_tag_parser(L"<current-players", this, gui_screen_postgame_lobby_get_session_value_text));
}

// 0x4cbb18 ?post_initialize@c_gui_screen_postgame_lobby@@UAAXXZ
void c_gui_screen_postgame_lobby::post_initialize()
{
 c_gui_screen_widget::post_initialize();
 g_lobby_location_manager.set_show_postgame_stats_upon_lobby_entrance(false);
}

// @address 0x4cbb48 ?create_list_widget@c_gui_screen_postgame_lobby@@UAAPAVc_gui_list_widget@@PBUs_list_widget_block@@@Z
c_gui_list_widget *c_gui_screen_postgame_lobby::create_list_widget(const s_list_widget_block *definition)
{
    if (definition->definition.core.name == _postgame_lobby_string_roster) {
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

// @address 0x499198 ?get_render_data_size@c_gui_screen_postgame_lobby@@MAA?AW4e_render_data_size@@XZ
e_render_data_size c_gui_screen_postgame_lobby::get_render_data_size()
{
    return k_render_data_size_large;
}

// @address 0x4da478 ?initialize_datasource@c_gui_screen_postgame_lobby@@UAAXXZ
void c_gui_screen_postgame_lobby::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    void *memory = ::user_interface_malloc_tracked(sizeof(c_gui_squad_roster_data), (char const *)(0), 0);
    c_gui_data *data = memory != 0 ? new(memory) c_gui_squad_roster_data(get_driving_controller()) : 0;
    if (data != 0) {
        if (data->initialize(_postgame_lobby_string_roster))
            add_datasource(data);
        else {
            data->~c_gui_data();
            user_interface_free(data);
        }
    }
}

// @address 0x4cbbd8 ?update@c_gui_screen_postgame_lobby@@UAAXK@Z
void c_gui_screen_postgame_lobby::update(unsigned long current_milliseconds)
{
 c_gui_widget *party_list = this->get_child_widget(_gui_list_widget, _postgame_lobby_string_roster);
 if (party_list)
  ((c_gui_roster_list_widget *)party_list)->m_show_party_indicators = true;

 unsigned short hopper_identifier = network_squad_session_get_hopper();
 s_hopper_information hopper_information;
 if (user_interface_matchmaking_get_hopper_information(hopper_identifier, &hopper_information))
 {
  long player_count;
  const s_matchmaking_single_hopper_statistics *statistics = get_hopper_statistics();
  if (statistics && statistics->hopper_population > 0)
   player_count = statistics->hopper_population;
  else
   player_count = -1;

  s_life_cycle_matchmaking_progress progress;
  network_life_cycle_get_matchmaking_progress(&progress);
  g_user_interface_matchmaking_progress = progress;
  if (progress.type == 5)
   m_progress = progress;

  update_status(hopper_identifier, &hopper_information, &m_progress);
  update_timer(current_milliseconds, hopper_identifier, &hopper_information, &m_progress);

  const c_game_variant *game_variant = user_interface_game_settings_get_game_variant();
  bool teams_enabled = game_variant != 0 && game_variant->has_teams();
  long status;
  if (hopper_information.ranked && teams_enabled)
   status = _postgame_lobby_string_matchmaking_postgame_tip_ranked_team;
  else if (hopper_information.ranked)
   status = _postgame_lobby_string_matchmaking_postgame_tip_ranked_ffa;
  else if (teams_enabled)
   status = _postgame_lobby_string_matchmaking_postgame_tip_unranked_team;
  else
   status = _postgame_lobby_string_matchmaking_postgame_tip_unranked_ffa;
  set_text_widget_string_id(_postgame_lobby_string_tips, status);

  bool show_first;
  if (user_interface_squad_get_player_count() > 1)
   show_first = true;
  else
   show_first = false;
  c_gui_widget *widget = this->get_child_widget(_gui_text_widget, _postgame_lobby_string_party_leader);
  if (widget)
   widget->set_visible(show_first);

  bool show_second;
  if (user_interface_squad_get_player_count() > 1)
   show_second = true;
  else
   show_second = false;
  widget = this->get_child_widget(_gui_bitmap_widget, _postgame_lobby_string_party_leader_underline);
  if (widget)
   widget->set_visible(show_second);

  bool show_timer = m_progress.rematch_requested;
  widget = this->get_child_widget(_gui_group_widget, _postgame_lobby_string_preparing_for_rematch);
  if (widget)
   widget->set_visible(show_timer);

  long string_id;
  if (player_count == -1)
   string_id = _postgame_lobby_string_hopper_name;
  else
   string_id = _postgame_lobby_string_hopper_with_population;
  set_text_widget_string_id(_postgame_lobby_string_hopper_name, string_id);

  if (count_bits<unsigned long>(m_progress.rematch_party_player_mask) >
      count_bits<unsigned long>(previous_requested_player_mask))
  {
   ::user_interface_play_sound(_ui_sound_party_up, m_core_definition.sound_override_index);
   previous_requested_player_mask = m_progress.rematch_party_player_mask;
  }
 }

 user_interface_matchmaking_update_hopper_statistics_for_screen(
  &m_statistics_downloader,
  &m_hopper_identifier, &m_hopper_statistics);
 c_gui_screen_widget::update(current_milliseconds);
}

// c_gui_screen_widget::handle_dialog_result (inlined)
static __forceinline bool gui_screen_postgame_lobby_widget_handle_dialog_result(const c_dialog_result_message *message) {
 bool result=false;
 if(message->get_dialog_name()==_postgame_lobby_string_need_to_select_storage_device_to_save) {
  if(message->get_dialog_result()==0) content_catalogue_display_device_selection_guide_interface(message->get_controller());
  result=true;
 }
 return result;
}

// @address 0x4cc260 ?handle_dialog_result@c_gui_screen_postgame_lobby@@UAA_NPBVc_dialog_result_message@@@Z
bool c_gui_screen_postgame_lobby::handle_dialog_result(const c_dialog_result_message *message) {
 bool handled=false;
 switch(message->get_dialog_name()) {
 case _postgame_lobby_string_leave_postgame_lobby:
  if(message->get_dialog_result()==0) user_interface_leave_squad(false);
  handled=true;
  break;
 case _postgame_lobby_string_post_game_leave_voted_to_party_up:
  if(message->get_dialog_result()==0) user_interface_networking_set_pre_game_state();
  handled=true;
  break;
 }
 if(!handled) handled=gui_screen_postgame_lobby_widget_handle_dialog_result(message);
 return handled;
}

static bool parse_xml_hopper_name(void *context,wchar_t *buffer,long count) {
 bool success=false;
 unsigned short hopper=network_squad_session_get_hopper();
 if(hopper!=65535) {
  s_hopper_information info;
  if (user_interface_matchmaking_get_hopper_information(hopper, &info))
  {
   gui_screen_postgame_lobby_ustrncpy(buffer, (const wchar_t *)info.name, count);
   success = true;
  }
 }
 return success;
}

static bool parse_xml_hopper_population(void *context, wchar_t *buffer, long count)
{
 long population;
 const s_matchmaking_single_hopper_statistics *statistics = ((c_gui_screen_postgame_lobby *)context)->get_hopper_statistics();
 if (statistics && statistics->hopper_population >= 0)
  population = statistics->hopper_population;
 else
  population = 0;

 c_static_wchar_string<256> string;
 string.print(L"%d", population);
 string.copy_to(buffer, count);
 return true;
}

// @address 0x4cc818 ?parse_xml_party_leader@@YA_NPAXPA_WJ@Z
static bool parse_xml_party_leader(void *context, wchar_t *buffer, long count)
{
 bool success = false;
 if ((unsigned char)user_interface_squad_exists() != 0) {
  int index = ::user_interface_squad_get_leader_player_index();
  if (index != -1) {
   const s_player_configuration *entry = ::user_interface_squad_get_player_data(index);
   if (entry != 0) {
    ustrnzcpy(buffer, entry->host.player_name, count);
    success = true;
   }
  }
 }
 return success;
}

bool gui_screen_postgame_lobby_get_session_value_text(void *context, wchar_t *buffer, long count)
{
 bool result = false;
 int tr5 = 0;
 c_network_session *session;
 session = 0;
 if ((unsigned char)user_interface_session_get_usable_player_xuid(&session) != 0) {
  tr5 = 1;
 }
 if ((unsigned int)(tr5 & 255) != 0) {
  long value = 0;
  session = 0;
  if ((unsigned char)user_interface_session_get_usable_player_xuid(&session) != 0) {
   const c_network_session_membership *membership = session->get_session_membership();
   if (membership != 0) {
    value = membership->get_player_count();
   }
  }
  usnzprintf(buffer, count, L"%d", value);
  result = true;
 }
 return result;
}

// @address 0x4cbe70 ?update_status@c_gui_screen_postgame_lobby@@AAAXGPAUs_hopper_information@@PAUs_life_cycle_matchmaking_progress@@@Z
void c_gui_screen_postgame_lobby::update_status(unsigned short hopper_id, s_hopper_information *hopper, s_life_cycle_matchmaking_progress *progress)
{
 bool local_peer_is_leader = ::network_life_cycle_squad_local_peer_is_leader();
 bool leader_has_voted = gui_screen_postgame_lobby_has_leader_rematch_request(progress);
 long status = -1;

 if (progress->rematch_requested && leader_has_voted)
  status = _postgame_lobby_string_matchmaking_postgame_status_preparing_for_rematch_voted;
 else if (progress->rematch_requested)
  status = _postgame_lobby_string_matchmaking_postgame_status_preparing_for_rematch;
 else if (!progress->rematch_enabled)
  status = _postgame_lobby_string_matchmaking_postgame_status_rematch_disabled;
 else if (!progress->rematch_ready)
  status = _postgame_lobby_string_matchmaking_postgame_status_prepare_to_deploy;
 else if (leader_has_voted)
  status = _postgame_lobby_string_matchmaking_postgame_status_party_has_voted_to_join;
 else if (local_peer_is_leader && hopper->ranked)
  status = _postgame_lobby_string_matchmaking_postgame_status_press_x_to_join_ranked;
 else if (local_peer_is_leader)
  status = _postgame_lobby_string_matchmaking_postgame_status_press_x_to_join_unranked;
 else
  status = _postgame_lobby_string_matchmaking_postgame_status_party_leader_can_join;

 if (status != -1)
  set_text_widget_string_id(_postgame_lobby_string_status, status);
}

// 0x4cbff0 ?update_timer@c_gui_screen_postgame_lobby@@AAAXKGPAUs_hopper_information@@PAUs_life_cycle_matchmaking_progress@@@Z
void c_gui_screen_postgame_lobby::update_timer(unsigned long current_time, unsigned short hopper_id,
    s_hopper_information *hopper_information, s_life_cycle_matchmaking_progress *progress)
{
 c_gui_widget *timer = this->get_child_widget(_gui_group_widget, _postgame_lobby_string_progress);
 bool timer_active = m_progress.type == 5 && progress->countdown_started && !m_progress.rematch_requested;
 long seconds = timer_active ? progress->countdown_timer : 0;
 c_static_wchar_string<1024> text;
 text.print(L"%d", (seconds - 1) > 0 ? (seconds - 1) : 0);
 set_text_widget_string_id(_postgame_lobby_string_timer_next_match,
  timer_active ? _postgame_lobby_string_timer_next_match : _postgame_lobby_string_timer_waiting);
 c_gui_widget *widget = this->get_child_widget(_gui_text_widget, _postgame_lobby_string_timer_next_match);
 if (widget)
  widget->set_visible(timer_active);
 widget = this->get_child_widget(_gui_text_widget, _postgame_lobby_string_timer);
 if (widget)
  ((c_gui_text_widget *)widget)->set_text(text.m_string);
 widget = this->get_child_widget(_gui_text_widget, _postgame_lobby_string_timer);
 if (widget)
  widget->set_visible(timer_active);
 if (timer_active && seconds != previous_seconds)
 {
  long sound_threshold = get_constants_datasource_integer(60);
  if (seconds <= sound_threshold && seconds > 0)
   ::user_interface_play_sound((e_user_interface_sound_effect)(seconds == 1 ? 16 : 15), m_core_definition.sound_override_index);
 }
 if (timer)
 {
  if (m_progress.type == 5 && timer_active)
  {
   long maximum = m_progress.countdown_timer;
   long old_maximum = maximum_remaining;
   long remaining = maximum;
   maximum = maximum > old_maximum ? maximum : old_maximum;
   maximum_remaining = maximum;
   if (maximum == 0)
   {
    remaining = 0;
    maximum = 1;
   }
   timer->start_animation_at_time(_animation_state_custom0,
       (unsigned long)((float)current_time - (1.0f - (float)remaining / (float)maximum) * 999.0f), true);
   timer->set_visible(true);
  }
  else
   timer->set_visible(false);
 }
 previous_seconds = seconds;
}


bool gui_screen_postgame_lobby_has_leader_rematch_request(s_life_cycle_matchmaking_progress *progress)
{
 bool result = false;
 long leader_index = ::user_interface_squad_get_leader_player_index();
 if (leader_index != -1)
 {
  const s_player_identifier *identifier = user_interface_squad_get_player_identifier(leader_index);
  if (identifier != 0 && user_interface_group_exists())
  {
   long player_index = user_interface_group_get_player_index(identifier);
   if (player_index != -1)
    result = ((1 << player_index) & progress->rematch_requested_player_mask) != 0;
  }
 }
 return result;
}



// @address 0x4cc598 ?handle_list_item_chosen@c_gui_screen_postgame_lobby@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_postgame_lobby::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item, c_gui_data *data)
{
 bool handled = false;
 if (list_name == _postgame_lobby_string_roster) {
  long selected = -1;
  if (data->get_integer_value(list_item->get_element_handle(), _postgame_lobby_string_player_index, &selected)) {
   long player = ::user_interface_session_get_player_at_local_controller(message->get_controller());
   if (player != -1) {
    const s_player_identifier *local_id = user_interface_session_is_local_player(player);
    const s_player_identifier *selected_id = user_interface_session_is_local_player(selected);
    if (local_id != 0 && selected_id != 0) {
     c_load_player_select_screen_message *load = (c_load_player_select_screen_message *)user_interface_malloc_tracked(sizeof(c_load_player_select_screen_message), 0, 0);
     if (load) {
      long layer = this->name;
      long controller = message->get_controller();
      c_load_player_select_screen_message *posted = new (load) c_load_player_select_screen_message(
          _postgame_lobby_string_player_select, (e_controller_index)controller, this->get_render_window(), layer, local_id, selected_id, true);
      if (posted) {
       posted->set_parent_screen_index(this->m_screen_index);
       ::user_interface_messaging_post((c_message const *)(c_deletable *)posted);
      }
     }
    } else {
     ::user_interface_play_sound(_ui_sound_error, this->m_core_definition.sound_override_index);
    }
   }
  }
  handled = true;
 }
 if (!handled) handled = c_gui_screen_widget::handle_list_item_chosen(message, list_name, list_item, data);
 return handled;
}

class c_game_results;

// @address 0x4cc310 ?handle_controller_input_message@c_gui_screen_postgame_lobby@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_postgame_lobby::handle_controller_input_message(const c_controller_input_message *message) {
 bool handled = false;
 if (message->get_event_type() == _event_type_button_press) {
  switch (message->get_component()) {
  case _controller_component_button_b:
   if (::network_life_cycle_squad_local_peer_is_leader()) {
    if (gui_screen_postgame_lobby_has_leader_rematch_request(&this->m_progress)) {
     c_load_dialog_screen_message *dialog;
     void *memory = ::user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), (char const *)(0), 0);
     if (memory) {
      long name = this->name;
      long controller = message->get_controller();
      dialog = new (memory) c_load_dialog_screen_message((e_controller_index)controller, this->get_render_window(), name,
       _postgame_lobby_string_post_game_leave_voted_to_party_up, name);
     } else
      dialog = 0;
     if (dialog) {
      dialog->set_parent_screen_index(this->m_screen_index);
      ::user_interface_messaging_post((c_message const *)(c_deletable *)dialog);
     }
    } else
     user_interface_networking_set_pre_game_state();
    handled = true;
   } else {
    c_load_dialog_screen_message *dialog;
    void *memory = ::user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), (char const *)(0), 0);
    if (memory) {
     long name = this->name;
     long controller = message->get_controller();
     dialog = new (memory) c_load_dialog_screen_message((e_controller_index)controller, this->get_render_window(), name,
      _postgame_lobby_string_leave_postgame_lobby, name);
    } else
     dialog = 0;
    if (dialog) {
     dialog->set_parent_screen_index(this->m_screen_index);
     ::user_interface_messaging_post((c_message const *)(c_deletable *)dialog);
    }
    handled = true;
   }
   break;
  case _controller_component_button_back:
   if (user_interface_networking_get_final_game_results()) {
    c_load_screen_message *load;
    void *memory = ::user_interface_malloc_tracked(sizeof(c_load_screen_message), (char const *)(0), 0);
    if (memory) {
     long name = this->name;
     load = new (memory) c_load_screen_message(_string_id_carnage_report, k_any_controller, this->get_render_window(), name);
    } else
     load = 0;
    if (load)
     ::user_interface_messaging_post((c_message const *)(c_deletable *)load);
    handled = true;
   }
   break;
  case _controller_component_button_x:
   if (::network_life_cycle_squad_local_peer_is_leader()) {
    long controller = message->get_controller();
    s_user_interface_controller *state = &g_user_interface_controller_globals.controllers[controller];
    unsigned char old = state->desires_rematch;
    unsigned char changed = old != 1;
    state->desires_rematch = true;
    if (changed)
     user_interface_controller_update_network_properties((e_controller_index)controller);
    handled = true;
   }
   break;
  case _controller_component_button_y:
   if (::online_local_xuid_is_online_enabled())
    online_guide_show_friends_ui(message->get_controller());
   break;
  }
 }
 if (!handled)
  handled = this->c_gui_screen_widget::handle_controller_input_message(message);
 return handled;
}

typedef char gui_postgame_hopper_number_offset[(offsetof(s_hopper_information, number) == 0x4c) ? 1 : -1];
typedef char gui_postgame_progress_seconds_offset[(offsetof(s_life_cycle_matchmaking_progress, countdown_timer) == 0x54) ? 1 : -1];
typedef char gui_postgame_hopper_statistics_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_postgame_lobby, m_hopper_statistics) == 0x2bc8) )) ? 1 : -1];
