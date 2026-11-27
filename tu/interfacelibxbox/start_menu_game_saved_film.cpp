// interfacelibxbox/start_menu_game_saved_film.obj: from the records 0x4ab8a0 0x4adb48 0x4adc78 0x4add30 0x4ade58
// @flags /TP /O2 /Oc
// @address 0x4adb48 ?initialize_datasource@c_start_menu_game_saved_film@@UAAXXZ
#include "interface/gui_network_session_advertisement_types.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "game/game_time_types.h"
#include "interface/user_interface_networking.h"
// @address 0x4adc78 ?update@c_start_menu_game_saved_film@@UAAXK@Z
// @address 0x4add30 ?handle_dialog_result@c_start_menu_game_saved_film@@UAA_NPBVc_dialog_result_message@@@Z
// @address 0x4ade58 ?handle_list_item_chosen@c_start_menu_game_saved_film@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "networking/logic/network_life_cycle.h"
#include "interface/gui_screens/start_menu/panes/game_saved_film/start_menu_game_saved_film.h"
#include "saved_games/content_catalogue.h"
#include "interface/user_interface_messages.h"
#include <new>

#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_data.h"
#include "interface/gui_screens/start_menu/panes/game_multiplayer/start_menu_game_multiplayer.h"
#include "interface/gui_screens/start_menu/panes/game_saved_film/start_menu_game_saved_film_externs.h"
#include "saved_games/saved_film_globals.h"
// @address 0x94e3b0 g_saved_film_timer_globals
#include "saved_games/saved_film.h"
#include "game/game.h"
#include "interface/gui_screens/saved_film/gui_screen_saved_film_control_pad.h"

#include "saved_games/saved_film_playback.h"
// @address 0x8c29c0 g_saved_film_playback
#include "interface/ui_allocation_marker_types.h"
#include "interface/user_interface_allocation.h"
#include "interface/game_tag_parser.h"
#include "interface/gui_screens/boot_betrayer/gui_screen_boot_betrayer.h"

// @address 0x4ab8a0 ?initialize@c_start_menu_game_saved_film@@UAAXXZ
void c_start_menu_game_saved_film::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<lobby-privacy", this, parse_lobby_privacy));
}

void c_start_menu_game_saved_film::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    long child_count = m_datasource_count;
    long child_index = 0;
    if (child_count <= 0)
        return;
    c_gui_data **child_slot = m_datasource;
    do
    {
        if ((*child_slot)->get_name() == _start_menu_string_start_menu_game_items)
        {
            c_gui_data *matched_widget = m_datasource[child_index];
            if (matched_widget)
            {
                matched_widget->clear_disabled_elements();
                if (network_life_cycle_squad_local_peer_is_leader())
                    matched_widget->set_disabled_element(_start_menu_string_name, _start_menu_string_leave_game);
                else
                    matched_widget->set_disabled_element(_start_menu_string_name, _start_menu_string_leave_film);
                if (!network_life_cycle_squad_local_peer_is_leader())
                    matched_widget->set_disabled_element(_start_menu_string_name, _start_menu_string_lobby_privacy);
                matched_widget->set_disabled_element(_start_menu_string_name, _start_menu_string_lobby_privacy);
            }
            return;
        }
        child_index++;
        child_slot++;
    } while (child_index < child_count);
}

void c_start_menu_game_saved_film::update(unsigned long current_milliseconds)
{
    c_start_menu_pane_screen_widget::update(current_milliseconds);
    unsigned char game_paused;
    if (game_time_globals->initialized)
        game_paused = (game_time_globals->flags >> 2) & 1;
    else
        game_paused = 0;
    bool show_widget;
    if (!game_paused && g_saved_film_timer_globals.playback_speed != 0.0f)
        show_widget = false;
    else
        show_widget = true;
    c_gui_widget *film_widget = get_child_widget(_gui_group_widget, _start_menu_string_pause);
    if (film_widget)
        film_widget->set_visible(show_widget);
}

bool c_start_menu_game_saved_film::handle_dialog_result(const c_dialog_result_message *incoming_message)
{
    bool handled = c_start_menu_game_multiplayer::handle_leave_game_response(this, incoming_message);
    if (!handled)
    {
        switch (incoming_message->get_dialog_name())
        {
        case _film_control_exit_dialog:
            if (incoming_message->get_dialog_result() == 0)
            {
                game_saved_film_start_timer();
                close_start_menu();
            }
            break;
        case _film_control_upload_exit_dialog:
            if (incoming_message->get_dialog_result() == 0)
            {
                saved_film_reset_playback_upload(g_saved_film_playback);
                game_saved_film_start_timer();
                close_start_menu();
            }
            handled = true;
            break;
        case _start_menu_string_lobby_privacy:
        {
            int advertisement = user_interface_networking_get_session_advertisement();
            int advertisement_choice = incoming_message->get_dialog_result();
            if ((unsigned int)advertisement_choice >= 1)
            {
                if ((unsigned int)advertisement_choice != 1)
                {
                    if ((unsigned int)advertisement_choice < 3)
                    {
                        user_interface_networking_set_session_advertisement(2);
                        handled = true;
                        break;
                    }
                }
                else
                {
                    user_interface_networking_set_session_advertisement(1);
                    handled = true;
                    break;
                }
            }
            else
                advertisement = 0;
            user_interface_networking_set_session_advertisement(advertisement);
            handled = true;
            break;
        }
        }
    }
    bool result;
    if (!handled)
    {
        result = false;
        if (incoming_message->get_dialog_name() == _start_menu_string_device_selection)
        {
            if (incoming_message->get_dialog_result() == 0)
                content_catalogue_display_device_selection_guide_interface(
                    (e_controller_index)incoming_message->get_controller());
            return true;
        }
    }
    else
        result = handled;
    return result;
}

bool c_start_menu_game_saved_film::handle_list_item_chosen(const c_controller_input_message *input, long list_name,
    c_gui_list_item_widget *chosen, c_gui_data *source)
{
 bool handled=false;
 switch(list_name) {
 case _start_menu_string_start_menu_items:
  long action=-1;
  handled=source->get_string_id_value(chosen->get_element_handle(), _start_menu_string_name, &action);
  if(handled) {
   switch(action) {
   case _start_menu_string_leave_game:start_menu_game_multiplayer_handle_leave_game(this,input->get_controller());break;
   case _start_menu_string_lobby_privacy: {
    c_load_dialog_screen_message *load=0;void *memory=::user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), (char const *)(0), 0);
    if (memory)
        load = new (memory) c_load_dialog_screen_message((e_controller_index)input->get_controller(),
            get_render_window(), get_name(), _start_menu_string_lobby_privacy, get_name());
    if(load){load->set_parent_screen_index(get_screen_index());::user_interface_messaging_post((c_message const *)load);}
    handled=true;break;
   }
   case _start_menu_string_leave_film:
    if(g_saved_film_playback->state) {
     c_load_dialog_screen_message *load=0;void *memory=::user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), (char const *)(0), 0);
     if (memory)
         load = new (memory) c_load_dialog_screen_message((e_controller_index)input->get_controller(),
             get_render_window(), get_name(), _film_control_upload_exit_dialog, get_name());
     if(load){m_exit_controller=input->get_controller();::user_interface_messaging_post((c_message const *)load);}
    }else{
     c_load_dialog_screen_message *load=0;void *memory=::user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), (char const *)(0), 0);
     if (memory)
         load = new (memory) c_load_dialog_screen_message((e_controller_index)input->get_controller(),
             get_render_window(), get_name(), _film_control_exit_dialog, get_name());
     if(load){load->set_parent_screen_index(get_screen_index());::user_interface_messaging_post((c_message const *)load);}
    }
    break;
   }
  }
  break;
 }
 if(!handled)handled=c_start_menu_pane_screen_widget::handle_list_item_chosen(
     (const c_controller_input_message *)input,list_name,(c_gui_list_item_widget *)chosen,(c_gui_data *)source);
 return handled;
}
