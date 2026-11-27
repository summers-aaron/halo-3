// interfacelibxbox/gui_location_manager.obj: from the records 0x8c028 0x8c650 0x8c728 0x4a6728 0x4a67b0
#include "interface/user_interface_matchmaking_progress.h"
#include <stddef.h>
#include "interface/user_interface_clock.h"
#include "interface/gui_game_mode_types.h"
#include "memory/tls_globals.h"
#include "shell/shell.h"
#include "game/game_globals_storage.h"
#include <ppcintrinsics.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/gui_screens/main_menu/gui_screen_main_menu.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "simulation/simulation.h"
#include "main/main_game.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/session/network_session.h"
#include "interface/user_interface_networking.h"
#include "networking/online/online_files.h"
#include "interface/c_gui_screen_widget.h"
#include "shell/shell_types.h"
#include "interface/gui_location_manager.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"
#include "interface/user_interface_window_manager.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"
#include "interface/user_interface_dialog_globals.h"
#include "cseries/pointer_utilities.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/life_cycle/life_cycle_handler_in_game.h"
#include "networking/logic/life_cycle/life_cycle_handler_in_match_internals.h"
#include "networking/logic/life_cycle/life_cycle_runtime_globals.h"
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers_item_selected.h"
#include "game/game_state_accessors.h"
#include "interface/user_interface_messages.h"
#include "tag_files/string_ids.h"
#include <new>

// @flags /TP /O2 /Oc
// @address 0x1142940 g_window_manager
// @address 0x4a6728 ?get_pregame_lobby_name@c_gui_location_manager@@CAJW4e_gui_game_mode@@@Z
// @address 0x4a67b0 ?change_location@c_gui_location_manager@@AAAXJ@Z
// @address 0x8c650 ?get_current_ui_location@c_gui_location_manager@@AAA?AW4e_gui_location@@XZ

class c_message;
// @address 0xf11308 g_network_life_cycle_globals

e_gui_location c_gui_location_manager::get_current_ui_location()
{
    e_gui_location location = _gui_location_none;
    if (game_in_progress() && !game_is_ui_shell()) {
        location = _gui_location_in_game;
    } else {
        c_gui_screen_widget *screen = get_location_screen();
        if (screen)
            location = (e_gui_location)screen->get_gui_location();
    }
    return location;
}

// @address 0x8c728 ?get_location_screen@c_gui_location_manager@@SAPAVc_gui_screen_widget@@XZ
c_gui_screen_widget *c_gui_location_manager::get_location_screen()
{
    c_gui_screen_widget *node = g_window_manager.get_topmost_screen(_console_window);
    while (node != 0) {
        if (!node->transitioning_out() && node->get_gui_location() != -1)
            break;
        node = g_window_manager.get_screen_below(_console_window, node);
    }
    return node;
}

long c_gui_location_manager::get_pregame_lobby_name(e_gui_game_mode game_mode)
{
    long result = -1;
    switch (game_mode + 1) {
    case 0: break;
    case 1: result = _string_id_pregame_lobby_campaign; break;
    case 3: result = _string_id_pregame_lobby_multiplayer; break;
    case 2: result = _string_id_pregame_lobby_matchmaking; break;
    case 4: result = _string_id_pregame_lobby_mapeditor; break;
    case 5: result = _string_id_pregame_lobby_theater; break;
    // match: 0x4a6728: __assume(0) omits the cmplwi/bgt default guard so game_mode+1 indexes the jump table unchecked.
    default: __assume(0);
    }
    return result;
}




void c_gui_location_manager::change_location(long screen_name)
{
    bool can_change_location = false;
    if (m_change_location_timer >= 30)
        can_change_location = true;
    if (can_change_location)
    {
        long transition_type = 1;
        c_gui_screen_widget *location_screen = get_location_screen();
        m_change_location_timer = 0;
        if (location_screen)
        {
            if (screen_name == _string_id_matchmaking_searching)
            {
                if (location_screen->get_name() == _string_id_pregame_lobby_matchmaking)
                    transition_type = 5;
            }
            else if (screen_name == _string_id_pregame_lobby_matchmaking)
            {
                if (location_screen->get_name() == _string_id_matchmaking_searching)
                    transition_type = 5;
            }
            location_screen->transition_out_with_transition_type(0, (e_screen_transition_type)transition_type);
        }
        c_window_manager *window_manager = &g_window_manager;
        if (screen_name == _string_id_matchmaking_searching)
        {
            c_gui_screen_widget *excluded_screens[7];
            excluded_screens[0] = window_manager->get_screen_by_name(_console_window, _string_id_carnage_report);
            excluded_screens[1] = window_manager->get_screen_by_name(_console_window, _string_id_spartan_milestone_dialog);
            excluded_screens[2] = window_manager->get_screen_by_name(_console_window, _string_id_spartan_rank_dialog);
            excluded_screens[3] = window_manager->get_screen_by_name(_console_window, _string_id_gui_alert_toast);
            excluded_screens[4] = window_manager->get_screen_by_name(_console_window, _string_id_gui_alert_nonblocking);
            excluded_screens[5] = window_manager->get_screen_by_name(_console_window, _string_id_gui_alert_ingame_full);
            excluded_screens[6] = window_manager->get_screen_by_name(_console_window, _string_id_gui_alert_ingame_split);
            window_manager->close_all_screens(excluded_screens, 7);
        }
        else
        {
            c_gui_screen_widget *excluded_screens[6];
            excluded_screens[0] = window_manager->get_screen_by_name(_console_window, _string_id_spartan_milestone_dialog);
            excluded_screens[1] = window_manager->get_screen_by_name(_console_window, _string_id_spartan_rank_dialog);
            excluded_screens[2] = window_manager->get_screen_by_name(_console_window, _string_id_gui_alert_toast);
            excluded_screens[3] = window_manager->get_screen_by_name(_console_window, _string_id_gui_alert_nonblocking);
            excluded_screens[4] = window_manager->get_screen_by_name(_console_window, _string_id_gui_alert_ingame_full);
            excluded_screens[5] = window_manager->get_screen_by_name(_console_window, _string_id_gui_alert_ingame_split);
            window_manager->close_all_screens(excluded_screens, 6);
        }
        if (screen_name != -1)
        {
            void *memory = ::user_interface_malloc_tracked(sizeof(c_load_screen_message), (char const *)(0), 0);
            c_load_screen_message *message = new (memory) c_load_screen_message(screen_name, k_any_controller, _console_window, _string_id_bottom_most);
            if (message)
            {
                ::user_interface_messaging_post(message);
                message->set_transition_type((e_screen_transition_type)transition_type);
            }
        }
    }
}


#if !defined(_XBOX) || defined(__clang__)
// PC (input_abstraction.cpp, GAME-CHANGES "High-Hz UI location changes"): the console's 60 Hz frame clock. Every
// main-loop pass is one of its frames up to HALO3_FPS=60; above that the passes come at the frame rate.
bool input_abstraction_input_frame_advanced();
#endif

// @address 0x8c028 ?update@c_gui_location_manager@@QAAXXZ
void c_gui_location_manager::update()
{
    if (!m_initialized)
        return;
    long network_location = user_interface_networking_get_current_location();
    unsigned long time = g_user_interface_globals.milliseconds.peek();
    if (m_last_update_time != time)
    {
        long timer = m_change_location_timer;
#if !defined(_XBOX) || defined(__clang__)
        // tmp/probe (GAME-CHANGES "High-Hz UI location changes"): change_location waits 30 of the console's 60 Hz
        // frames (500 ms, 15 game ticks) between two location screens. Counted per pass, at 240 Hz it waited 125 ms
        // (3-4 ticks), and the main menu's camera script started inside the previous screen's sleep_ui: the two
        // mainmenu.hsc camera scripts then stopped each other and the menu camera froze. Count the console's frames.
        if (timer < 30 && input_abstraction_input_frame_advanced())
#else
        if (timer < 30)
#endif
            m_change_location_timer = timer + 1;
    }
    m_last_update_time = time;
    if (network_location == -1)
        return;
#if defined(__clang__)
    // The port places each global on its own, so the networking globals are not 0x120 below the progress.
    if (g_user_interface_networking_globals.start_pending)
#else
    // match: 0x8c028: start_pending is tested at -0x120 off the matchmaking-progress base the function keeps for the later
    // progress copy; naming the global loads a second base and moves the save range from r26 to r27.
    if (((s_user_interface_networking_globals *)offset_pointer(&g_user_interface_matchmaking_progress, -0x120))->start_pending)
#endif
        return;
    long current_location = get_current_ui_location();
    c_gui_screen_widget *location_screen = get_location_screen();
    if (m_squad_type == 0 || network_location != 0)
        m_squad_type = (!user_interface_squad_exists()) + 1;
    c_network_session *session[4];
    bool in_session = network_life_cycle_in_session(session);
    do
    {
        if (simulation_starting_up() && tls_game_globals()->options.game_mode == 3 && network_location == 4)
        {
            bool map_load_pending = false;
            if (g_network_life_cycle_globals.initialized)
            {
                long state = g_network_life_cycle_globals.manager.m_current_state;
                if (state == _life_cycle_state_in_game || state == _life_cycle_state_in_match)
                {
                    // The in-game and in-match handlers keep their pending flags at the same offset.
                    if (static_cast<c_life_cycle_state_handler_in_game *>(g_network_life_cycle_globals.manager.m_handlers[state])->is_map_load_pending())
                        map_load_pending = true;
                }
            }
            if (!map_load_pending && (!in_session || session[0]->is_host()))
            {
                network_life_cycle_end();
                break;
            }
        }
        if (current_location != network_location)
        {
            bool skip_change = false;
            if (network_location == 0 && current_location == 4 && m_squad_type == 2)
            {
                skip_change = true;
            }
            else if (network_life_cycle_in_matchmaking())
            {
                s_life_cycle_matchmaking_progress progress;
                network_life_cycle_get_matchmaking_progress(&progress);
                g_user_interface_matchmaking_progress = progress;
                skip_change = progress.type == 0;
            }
            if (!skip_change)
            {
                if (tls_game_globals()->options.game_mode != 3 && (current_location == 4 || network_location == 1))
                {
                    main_menu_launch();
                    m_main_menu_launch_pending = true;
                }
                else
                {
                    if ((unsigned long)network_location >= 2)
                    {
                        g_online_file_manager.stop();
                    }
                    else if (g_online_file_manager.m_buffer != 0 && !g_online_file_manager.m_active)
                    {
                        g_online_file_manager.m_active = true;
                        g_online_file_manager.m_refresh_queue = true;
                        g_online_file_manager.m_active_transfer_tracker.m_status_dirty = true;
                        ++g_online_file_manager.m_active_transfer_tracker.m_refresh_count;
                    }
                    long screen_name = -1;
                    switch (network_location)
                    {
                    case 0: screen_name = _main_menu_string_main_menu; break;
                    case 1:
                        {
                            long lobby_game_mode = (long)network_life_cycle_squad_session_get_ui_game_mode();
                            screen_name = get_pregame_lobby_name((e_gui_game_mode)lobby_game_mode);
                        }
                        break;
                    case 2: screen_name = _string_id_matchmaking_searching; break;
                    case 3: screen_name = _string_id_matchmaking_match_found; break;
                    case 4: break;
                    case 5: screen_name = _string_id_postgame_lobby; break;
                    // match: 0x8c028: __assume(0) omits the cmplwi/bgt default guard so network_location indexes the jump table unchecked.
                    default: __assume(0);
                    }
                    change_location(screen_name);
                }
            }
        }
        else if (current_location == 1)
        {
            long game_mode = (long)network_life_cycle_squad_session_get_ui_game_mode();
            if (game_mode != -1 &&
                static_cast<c_gui_screen_pregame_lobby *>(location_screen)->get_gui_game_mode() != game_mode)
            {
                change_location(get_pregame_lobby_name((e_gui_game_mode)game_mode));
            }
        }
    } while (false);
    if (location_screen != 0)
    {
        c_gui_screen_widget *screen = g_window_manager.get_screen_below(_console_window, location_screen);
        while (screen != 0)
        {
            if (screen->transitioning_out())
                return;
            screen->transition_out_with_transition_type(0, _screen_transition_normal);
            screen = g_window_manager.get_screen_below(_console_window, screen);
        }
    }
}

typedef char gui_location_manager_size[(sizeof(c_gui_location_manager) == 0x14) ? 1 : -1];
