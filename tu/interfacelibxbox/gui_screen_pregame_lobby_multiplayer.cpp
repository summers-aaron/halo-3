#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_mapeditor.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_campaign.h"
// interfacelibxbox/gui_screen_pregame_lobby_multiplayer.obj: from the records 0x4c52c8 0x4c63e8 0x4c6408 0x4c6418 0x4c6428 0x4c6438 0x4c6530 0x4c65b0 0x4c66f8 0x4c6788 0x4c6800
// @flags /TP /O2 /Oc
#include "game/game_variant.h"
#include "interface/session_game_mode_types.h"
#include "interface/gui_widget_types.h"
#include "memory/static_arrays.h"
#include "interface/user_interface_messages.h"
#include <new>
// @address 0x4c6788 parse_xml_pregame_map_name
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "text/unicode.h"
#include "networking/logic/network_life_cycle.h"
#include "game/map_variant.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_multiplayer.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby_theater.h"
#include "interface/gui_screens/player_select/gui_player_select_screen_widget.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_data.h"

#include "interface/c_gui_screen_widget.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_allocation.h"
#include "interface/user_interface_text.h"
#include "interface/gui_selected_items.h"


enum e_pregame_selection_name
{
    _pregame_selection_game_variant = 501,
    _pregame_selection_map = 624,
};

#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"
#include "interface/gui_widget.h"



// @address 0x4c63e8 ?get_start_status_text@c_gui_screen_pregame_lobby_multiplayer@@UAAJ_N@Z
long c_gui_screen_pregame_lobby_multiplayer::get_start_status_text(bool flag) {
 return flag ? 0x102c1 : 0x102c6;
}

// @address 0x4c6408 ?get_lobby_header@c_gui_screen_pregame_lobby_multiplayer@@UAAJXZ
long c_gui_screen_pregame_lobby_multiplayer::get_lobby_header() {
 return 0x102b0;
}

// @address 0x4c6418 ?get_lobby_title@c_gui_screen_pregame_lobby_multiplayer@@UAAJXZ
long c_gui_screen_pregame_lobby_multiplayer::get_lobby_title() {
 return 0x102ab;
}

// @address 0x4c6428 ?get_start_button_name@c_gui_screen_pregame_lobby_multiplayer@@UAAJXZ
long c_gui_screen_pregame_lobby_multiplayer::get_start_button_name() {
 return 0x102b9;
}

// @address 0x4c65b0 ?handle_list_item_chosen@c_gui_screen_pregame_lobby_multiplayer@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_pregame_lobby_multiplayer::handle_list_item_chosen(const c_controller_input_message *message,
    long list_name, c_gui_list_item_widget *list_item, c_gui_data *data) {
 bool handled=false;
 if(list_name==_pregame_lobby_string_lobby_list) {
  long selected=-1;
  if(data->get_string_id_value(list_item->get_element_handle(),_pregame_lobby_string_target,&selected)) {
   e_gui_selected_item_type type=_gui_selected_item_none;
   switch (selected)
   {
   case _pregame_selection_game_variant: type = _gui_selected_item_game_variant; break;
   case _pregame_selection_map: type = _gui_selected_item_map; break;
   }
   if(type!=_gui_selected_item_none) {
    void *memory = ::user_interface_malloc_tracked(sizeof(c_load_pregame_selection_screen_message), 0, 0);
    if(memory) {
     long layer = m_name;
     e_controller_index controller = message->get_controller();
     e_window_index window = get_render_window();
     c_load_pregame_selection_screen_message *load =
         new (memory) c_load_pregame_selection_screen_message(controller, window, layer, type);
     load->set_parent_screen_index(m_screen_index);
     ::user_interface_messaging_post(load);
    }
    handled=true;
   }
  }
 }
 if(!handled) handled=c_gui_screen_pregame_lobby::handle_list_item_chosen(message,list_name,list_item,data);
 return handled;
}

// 0x4c6788
extern "C" bool parse_xml_pregame_map_name(void *context, wchar_t *buffer, unsigned long size)
{
 c_static_wchar_string<1024> string;
 const c_game_variant *game_variant = user_interface_game_settings_get_game_variant();
 if (game_variant)
 {
  string.set(game_variant->get_name());
 }
 string.copy_to(buffer, size);
 return true;
}






// @address 0x4c66f8 ?handle_controller_input_message@c_gui_screen_pregame_lobby_multiplayer@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_pregame_lobby_multiplayer::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    bool in_countdown = ::user_interface_squad_in_or_after_countdown();
    if (message->get_event_type() == _event_type_button_press && message->get_component() == _controller_component_button_x)
    {
        if (in_countdown)
            ::user_interface_squad_stop_countdown_timer(message->get_controller(), -1, false);
        else
            load_game_options(message->get_controller());
        handled = true;
    }
    if (!handled)
        handled = c_gui_screen_pregame_lobby::handle_controller_input_message(message);
    return handled;
}

// @address 0x4c6530 ?update@c_gui_screen_pregame_lobby_multiplayer@@UAAXK@Z
void c_gui_screen_pregame_lobby_multiplayer::update(unsigned long milliseconds)
{
    c_gui_screen_pregame_lobby::update(milliseconds);
    if (user_interface_squad_exists() && user_interface_get_session_game_mode() == _session_game_mode_pregame)
    {
        c_gui_widget *sprite = get_child_widget(_gui_bitmap_widget, _pregame_lobby_theater_string_gametype_image);
        // The current variant belongs to the session and does not depend on the widget.
        const c_game_variant *variant = ((c_gui_screen_pregame_lobby_multiplayer *)sprite)->get_current_variant();
        if (sprite != 0 && variant != 0)
            ((c_gui_bitmap_widget *)sprite)->set_sprite_frame(variant->m_game_engine_index);
    }
}

// @address 0x4c6438 ?initialize@c_gui_screen_pregame_lobby_multiplayer@@UAAXXZ
void c_gui_screen_pregame_lobby_multiplayer::initialize(void)
{
    c_gui_screen_pregame_lobby::initialize();
    set_initial_focus(_pregame_lobby_string_lobby_list, 4);
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-multiplayer-game", this, (bool (*)(void *, wchar_t *, long))parse_xml_pregame_map_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-multiplayer-map", this, (bool (*)(void *, wchar_t *, long))user_interface_text_get_map_name));
}

// @address 0x4c52c8 ?handle_controller_input_message@c_gui_screen_pregame_lobby_campaign@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_pregame_lobby_campaign::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    bool in_countdown = ::user_interface_squad_in_or_after_countdown();
    if (message->get_event_type() == _event_type_button_press && message->get_component() == _controller_component_button_x)
    {
        if (in_countdown)
        {
            ::user_interface_squad_stop_countdown_timer(message->get_controller(), -1, false);
        }
        else if (::network_life_cycle_squad_local_peer_is_leader())
        {
            c_load_screen_message *load = (c_load_screen_message *)::user_interface_malloc_tracked(sizeof(c_load_screen_message), 0, 0);
            if (load)
            {
                long layer = m_name;
                long controller = message->get_controller();
                c_load_screen_message *posted = new (load) c_load_screen_message(
                    _player_select_string_campaign_settings, (e_controller_index)controller, get_render_window(), layer);
                if (posted)
                {
                    posted->m_parent_screen_index = m_screen_index;
                    ::user_interface_messaging_post((c_message const *)posted);
                }
            }
        }
        handled = true;
    }
    if (!handled)
        handled = c_gui_screen_pregame_lobby::handle_controller_input_message(message);
    return handled;
}



// @address 0x4c6800 ?get_start_status_text@c_gui_screen_pregame_lobby_mapeditor@@UAAJ_N@Z
long c_gui_screen_pregame_lobby_mapeditor::get_start_status_text(bool is_leader)
{
    return is_leader ? 0x102c2 : 0x102c7;
}
