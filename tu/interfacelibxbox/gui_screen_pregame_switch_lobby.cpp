// interfacelibxbox/gui_screen_pregame_switch_lobby.obj: from the records 0x4c74d0 0x4c7630
// @flags /TP /O2 /Oc
// @address 0x4c74d0 ?post_initialize@c_gui_screen_pregame_switch_lobby@@UAAXXZ
// @address 0x485560 ?post_initialize@c_gui_screen_widget@@UAAXXZ
// @address 0x46f190 ?get_alpha_is_internal_beta@@YA_NXZ
// @address 0x909d8 ?get_alpha_is_locked_down@@YA_NXZ
#include "interface/user_interface_messages.h"
#include <ppcintrinsics.h>
#include "interface/user_interface.h"
#include "interface/gui_pregame_setup_manager.h"
#include "main/game_preferences.h"
#include "interface/user_interface_matchmaking.h"

#include "networking/logic/network_session_interface.h"

#include "interface/gui_data.h"


#include "interface/c_gui_screen_widget.h"


#include "interface/c_gui_list_item_widget.h"

#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_switch_lobby.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"

// switches the lobby's start options for alpha builds
void c_gui_screen_pregame_switch_lobby::post_initialize()
{
    for (long index = 0; index < m_datasource_count; index++)
    {
        if (m_datasource[index]->get_name() == _pregame_lobby_string_switch_lobby)
        {
            c_gui_data *data = m_datasource[index];
            if (data != 0)
            {
                if (get_alpha_is_internal_beta())
                {
                    data->set_disabled_element(_pregame_lobby_string_name, _lobby_selection_campaign);
                    data->set_disabled_element(_pregame_lobby_string_name, _lobby_selection_forge);
                }
                if (get_alpha_custom_games_disabled())
                    data->set_disabled_element(_pregame_lobby_string_name, _lobby_selection_custom_games);
                if (get_alpha_is_locked_down())
                {
                    data->set_disabled_element(_pregame_lobby_string_name, _lobby_selection_matchmaking);
                    data->set_disabled_element(_pregame_lobby_string_name, _lobby_selection_theater);
                }
                else
                {
                    data->set_disabled_element(_pregame_lobby_string_name, 0x1fa);
                }
            }
            break;
        }
    }
    c_gui_screen_widget::post_initialize();
}

// @address 0x4c7630 ?handle_list_item_chosen@c_gui_screen_pregame_switch_lobby@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_pregame_switch_lobby::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    if (list_name == _pregame_lobby_string_switch_lobby)
    {
        long value = -1;
        if (data->get_string_id_value(item->get_element_handle(), _pregame_lobby_string_name, &value))
        {
            long game_mode;
            switch (value)
            {
            case _lobby_selection_campaign:
                game_mode = 0;
                break;
            case _lobby_selection_matchmaking:
                game_mode = 1;
                break;
            case _lobby_selection_custom_games:
                game_mode = 2;
                break;
            case _lobby_selection_forge:
                game_mode = 3;
                break;
            case _lobby_selection_theater:
                game_mode = 4;
                break;
            default:
                game_mode = -1;
                break;
            }
            if (game_mode != -1 && game_mode != ((long)::network_life_cycle_squad_session_get_ui_game_mode()))
            {
                global_preferences_set_last_menu_selection(value);
                if (game_mode == 1)
                    user_interface_matchmaking_squad_set_search_preference(0);
                gui_pregame_setup_manager_initialize_squad_for_mode(game_mode);
            }
            transition_out_with_transition_type(0, _screen_transition_normal);
            handled = true;
        }
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_list_item_chosen(message, list_name, item, data);
    return handled;
}
