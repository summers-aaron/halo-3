// @address 0x8c42a0 ?g_camera_globals@@3Us_camera_globals@@A
// blamlibxbox/saved_film_director.obj: from the records 0x281e18
// @flags /TP /O2 /Oc
#include "game/player_control_globals.h"

#include "shell/shell.h"
#include "memory/tls_globals.h"
#include <math.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "game/game.h"
#include "game/player_control.h"
#include "interface/user_interface_controller.h"
#include "saved_games/saved_film.h"
#include "saved_games/saved_film_timing.h"
#include "saved_games/saved_film_tick.h"
// @address 0x94e3ac g_saved_film_playback_tick
// @address 0x94e3b0 g_saved_film_timer_globals
#include "camera/director.h"
#include "camera/camera.h"
#include "camera/camera_globals.h"
#include "input/input_abstraction.h"
#include "interface/user_interface_utilities.h"
#include "input/input_abstraction_types.h"
#include "game/player_control_types.h"


#include "interface/c_window_manager.h"
#include "interface/user_interface_window_manager.h"
#include "camera/camera_mode_types.h"
#include "interface/c_gui_screen_widget.h"
#include "camera/camera_types.h"
#include "camera/saved_film_director_internals.h"
#include "interface/user_interface_messages.h"
#include <new>
#include "camera/saved_film_director_storage.h"
// @address 0x11df85c g_director_camera_scripted
// @address 0x11df70c g_director_globals

// @address 0x281e18 ?update@c_saved_film_director@@UAAXM@Z
void c_saved_film_director::update(float elapsed_time)
{
    long controller = user_interface_controller_get_rumble_enabled(m_user_index);
    if (controller != -1 && !camera_responding_gate((e_controller_index)controller))
    {
        s_game_input_state *input = &input_abstraction_globals.input_states[controller];
        bool menu_open = g_window_manager.get_screen_by_name(_console_window, 0x100c2) != 0
            || g_window_manager.get_screen_by_name(_console_window, 0x1056e) != 0;
        if (game_is_film_playback_active())
        {
            float amount = input->buttons[1].down_amount();
            if (!player_control_is_old_controls_active() && !(amount <= 0.0f))
            {
                m_adjusting_playback_speed = true;
                float speed = (float)pow((double)(float)(g_camera_globals.saved_film_maximum_playback_speed + 1.0f), (double)amount) - 1.0f;
                saved_film_set_playback_game_speed(speed);
            }
            else if (m_adjusting_playback_speed)
            {
                m_adjusting_playback_speed = false;
                saved_film_set_playback_game_speed(0.0f);
            }
        }
        if (!menu_open)
        {
            bool show_menu = false;
            bool film_finished = false;
            if (input->buttons[12].down_frames() == 1)
            {
                if (game_is_film_playback_active())
                    show_menu = true;
                else
                    user_interface_play_sound(_ui_sound_error, -1);
            }
            if (game_is_film_playback_active() && g_saved_film_timer_globals.film.get_current_tick() == 0 && saved_film_get_save_state() == 0)
            {
                show_menu = true;
                film_finished = true;
            }
            if (saved_film_try_and_get_timer_elapsed(0))
                show_menu = false;
            if (show_menu)
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_screen_message), 0, 0);
                if (memory)
                {
                    c_load_screen_message *message = new (memory) c_load_screen_message(0x100c2, k_any_controller, _console_window, 0x1000c);
                    if (message)
                    {
                        long mode = saved_film_get_save_state();
                        long item;
                        if (mode == 5 || mode == 6)
                            item = 0x1056d;
                        else if (mode == 3 || mode == 1)
                            item = 0x10568;
                        else if (film_finished)
                            item = 0x10569;
                        else if (!(g_saved_film_timer_globals.playback_speed <= 0.0f))
                            item = 0x10567;
                        else
                            item = 0x10566;
                        user_interface_play_sound(_ui_sound_dialog, -1);
                        message->set_focus_on_load_by_name(0x1055f, 0x10010, item);
                        user_interface_messaging_post(message);
                        menu_open = true;
                    }
                }
            }
        }
        if (!menu_open)
        {
            if (input->buttons[10].down_frames() == 1)
            {
                if (game_is_film_playback_active())
                {
                    if (g_saved_film_timer_globals.playback_speed == 0.0f)
                    {
                        saved_film_set_playback_game_speed(1.0f);
                        m_adjusting_playback_speed = false;
                    }
                    else
                        saved_film_set_playback_game_speed(0.0f);
                }
                else
                    user_interface_play_sound(_ui_sound_error, -1);
            }
            bool input_inhibited = player_control_can_use_debug_controls() && player_control_globals->thread_active;
            if (!input_inhibited && input->buttons[2].down_amount() == 0.0f && input->buttons[3].down_amount() == 0.0f)
            {
                if (input->buttons[4].down_frames() == 1)
                {
                    if (game_is_film_playback_active() && saved_film_can_skip(1))
                        saved_film_set_pending_seek(1);
                    else
                        user_interface_play_sound(_ui_sound_error, -1);
                }
                else if (input->buttons[5].down_frames() == 1)
                {
                    if (game_is_film_playback_active() && saved_film_can_skip(2))
                        saved_film_set_pending_seek(2);
                    else
                        user_interface_play_sound(_ui_sound_error, -1);
                }
            }
        }
    }
    long target_tick = saved_film_get_target_tick();
    if (target_tick != -1 && g_saved_film_playback_tick.peek() < target_tick)
    {
        s_controllers *globals = g_director_globals;
        globals->transition = -1.0f;
        globals->transition_remaining = -1.0f;
    }
    c_observer_director::update(elapsed_time);
    if (get_camera()->get_type() == _camera_mode_authored)
    {
        s_controllers *globals = g_director_globals;
        s_director_slot *user = &globals->slots[m_user_index];
        user->perspective = director_get_effective_perspective(this);
    }
    if (*g_director_camera_scripted)
        set_camera_mode(_camera_mode_scripted, 0.0f, 0);
    else if (get_camera()->get_type() == _camera_mode_scripted)
        set_camera_mode(_camera_mode_first_person, 0.0f, 0);
}

