// interfacelibxbox/gui_screen_saved_film_control_pad.obj: from the records 0x4de660 0x4de698 0x4dec48 0x4deeb8 0x4defd0 0x4df458
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "interface/user_interface_device_selection.h"
#include "interface/user_interface_sound.h"
#include "game/game_globals_storage.h"
#include "interface/gui_widget_types.h"
#include "cseries/cseries.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "simulation/simulation.h"
#include "saved_games/saved_film.h"
#include "saved_games/saved_film_tick.h"
#include "interface/file_share_category_dependencies.h"
// @address 0x8a1ead g_utility_drive_mounted
#include "game/game.h"
#include "interface/c_controller.h"
#include "saved_games/content_catalogue.h"
#include "saved_games/content_catalogue_types.h"
#include "game/player_mapping_types.h"
#include "memory/tls_globals.h"
#include "interface/user_interface_messages.h"
#include "interface/c_gui_widget.h"
#include "interface/gui_data.h"
#include "interface/c_gui_screen_widget.h"
#include "game/player_configuration.h"
#include "interface/c_gui_list_widget.h"
#include "interface/user_interface_sound_effect_types.h"
#include "saved_games/saved_film_recording_types.h"
#include "saved_games/saved_film_globals.h"
#include "saved_games/saved_film_playback.h"
#include "interface/gui_widget.h"
#include <new>
#include "interface/gui_screen_menu_externs.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "saved_games/saved_film_snippet.h"
#include "interface/gui_screens/saved_film/gui_screen_saved_film_control_pad.h"
#include "saved_games/saved_film_recording_state.h"
#include "interface/c_gui_list_item_widget.h"
// @address 0x4dec48 ?refresh_button_keys@c_saved_film_control_pad@@QAAXXZ
// @address 0x4de660 ?post_initialize@c_saved_film_control_pad@@UAAXXZ
// @address 0x485560 ?post_initialize@c_gui_screen_widget@@UAAXXZ
// @address 0x1e1e68 saved_film_get_save_state
// @address 0x94e3b0 g_saved_film_timer_globals
// @address 0x483310 ??0c_load_dialog_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJJ@Z

// @address 0x8c29c0 g_saved_film_playback
// @address 0x94e3ac g_saved_film_playback_tick

typedef char saved_film_control_pad_film_offset[(offsetof(s_saved_film_timer_globals, film) == 0x40) ? 1 : -1];
typedef char saved_film_control_pad_speed_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_film_timer_globals, playback_speed) == 0x101fc))) ? 1 : -1];

static __forceinline c_gui_data *saved_film_control_pad_find_data(c_gui_screen_widget *screen, long name)
{
    c_gui_screen_widget *view = screen;
    c_gui_data *result = 0;
    for (long index = 0; index < view->m_datasource_count; index++)
    {
        if (view->m_datasource[index]->get_name() == name)
        {
            result = view->m_datasource[index];
            break;
        }
    }
    return result;
}

static __forceinline long saved_film_control_pad_current_tick()
{
    long tick = 0;
    tick = g_saved_film_timer_globals.film.get_current_tick();
    return tick;
}

// Disable film controls that are unavailable in the current playback and recording state.
void c_saved_film_control_pad::refresh_button_keys()
{
    c_gui_data *data = saved_film_control_pad_find_data(this, 0x1055e);
    if (data == 0)
        return;

    long slot_count = saved_film_get_save_state();
    data->clear_disabled_elements();
    data->set_disabled_element(0x10010, _film_control_save_secondary);
    data->set_disabled_element(0x10010, 0x10564);
    if (saved_film_control_pad_current_tick() == 0)
    {
        data->set_disabled_element(0x10010, _film_control_pause);
        data->set_disabled_element(0x10010, _film_control_play);
    }
    else
    {
        if (!(g_saved_film_timer_globals.playback_speed <= 0.0f))
            data->set_disabled_element(0x10010, _film_control_play);
        else
            data->set_disabled_element(0x10010, _film_control_pause);
        data->set_disabled_element(0x10010, _film_control_reset_speed);
    }
    switch (slot_count)
    {
    case 3:
    case 4:
        data->set_disabled_element(0x10010, _film_control_start_recording);
        data->set_disabled_element(0x10010, _film_control_set_end);
        break;
    case 6:
        data->set_disabled_element(0x10010, _film_control_start_recording);
        data->set_disabled_element(0x10010, _film_control_set_start);
        break;
    case 0:
    case 1:
    case 2:
    case 5:
    case 7:
    case 8:
    case 9:
    case _saved_film_recording_waiting_for_description:
    case _saved_film_recording_copy:
    case _saved_film_recording_waiting_for_copy:
    case _saved_film_recording_update_metadata:
    case _saved_film_recording_waiting_for_metadata:
    case _saved_film_recording_waiting_for_playback:
        data->set_disabled_element(0x10010, _film_control_set_start);
        data->set_disabled_element(0x10010, _film_control_set_end);
        break;
    default:
        return;
    }
}

void c_saved_film_control_pad::post_initialize()
{
    refresh_button_keys();
    c_gui_screen_widget::post_initialize();
}

// @address 0x486578 ?get_render_window@c_gui_screen_widget@@QAA?AW4e_window_index@@XZ

static __forceinline void saved_film_control_pad_close(c_gui_screen_widget *screen)
{
    screen->transition_out_with_transition_type(0, _screen_transition_normal);
}

// @address 0x4deeb8 ?handle_controller_input_message@c_saved_film_control_pad@@UAA_NPBVc_controller_input_message@@@Z
bool c_saved_film_control_pad::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    bool suppress = false;
    switch (message->m_component)
    {
    case 1:
    case 2:
        if (saved_film_get_save_state() == 0)
            saved_film_control_pad_close(this);
        handled = true;
        break;
    case _controller_component_button_start:
        saved_film_control_pad_close(this);
        break;
    case _controller_component_left_stick:
    case _controller_component_right_stick:
        suppress = true;
        break;
    }
    if (!handled && !suppress)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

static __forceinline bool saved_film_control_pad_handle_storage_result(const c_dialog_result_message *message)
{
    bool handled = false;
    if (message->m_dialog_name == _film_control_need_to_select_storage_device_to_save)
    {
        if (message->m_dialog_result == _gui_dialog_choice_first)
            content_catalogue_display_device_selection_guide_interface(message->m_controller);
        handled = true;
    }
    return handled;
}

static __forceinline c_content_catalogue *saved_film_content_catalogue_get(e_controller_index controller_index)
{
    return &content_catalogues().catalogues[controller_index];
}

// @address 0x4defd0 ?handle_list_item_chosen@c_saved_film_control_pad@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_saved_film_control_pad::handle_list_item_chosen(
    const c_controller_input_message *message, long list_id, c_gui_list_item_widget *list_item, c_gui_data *data)
{
    bool handled = false;
    if (list_id == 0x1055f)
    {
        long selected[4];
        selected[0] = -1;
        if (data->get_string_id_value(list_item->get_element_handle(), 0x10010, selected))
        {
            handled = true;
            switch (selected[0])
            {
            case _film_control_skip_back:
                saved_film_set_pending_seek(1);
                break;
            case _film_control_skip_forward:
                saved_film_set_pending_seek(2);
                break;
            case _film_control_save_primary:
            case _film_control_save_secondary:
                {
                    e_controller_index controller_index = message->m_controller;
                    c_controller_interface *controller = controller_get(controller_index);
                    if (content_catalogue_has_storage_device(saved_film_content_catalogue_get(controller->get_controller_index())))
                    {
                        if (content_file_share_enabled() && controller->is_signed_in_to_machine())
                        {
                            if (g_saved_film_playback && g_saved_film_playback->state == 0)
                            {
                                void *memory = ::user_interface_malloc_tracked(sizeof(c_load_screen_message), (char const *)(0), 0);
                                if (memory)
                                {
                                    e_controller_index message_controller = message->m_controller;
                                    c_load_screen_message *screen_message = new (memory)
                                        c_load_screen_message(0x10572, message_controller, get_render_window(), 0x1000c);
                                    if (screen_message)
                                    {
                                        ::user_interface_messaging_post(screen_message);
                                        saved_film_control_pad_close(this);
                                    }
                                }
                            }
                        }
                        else
                            ::user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
                    }
                    else
                        user_interface_show_device_selection_dialog(controller_index, m_name,
                            m_screen_index);
                    break;
                }
            case _film_control_start_recording:
                {
                    e_controller_index controller_index = message->m_controller;
                    if (content_file_share_enabled() && controller_get(controller_index)->is_signed_in_to_machine())
                        saved_film_handle_start_recording();
                    else
                        ::user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
                    break;
                }
            case _film_control_play:
                saved_film_set_playback_game_speed(1.0f);
                break;
            case _film_control_pause:
                saved_film_set_playback_game_speed(0.0f);
                break;
            case _film_control_reset_speed:
                game_saved_film_reset_playback_speed();
                break;
            case _film_control_set_start:
                if (saved_film_get_save_state() != 3 || !saved_film_is_recording() || g_saved_film_recording_globals.state != 3)
                    break;
                if (!saved_film_set_snippet_start_tick())
                {
                    if (!g_saved_film_timer_globals.request_pending)
                    {
                        g_saved_film_timer_globals.request_pending = 1;
                        g_saved_film_timer_globals.requested_state = 5;
                    }
                }
                else
                    saved_film_set_playback_rate(0.0f, 1);
                break;
            case _film_control_set_end:
                if (saved_film_get_save_state() != 6 || !saved_film_is_recording() || g_saved_film_recording_globals.state != 6 ||
                    g_saved_film_playback_tick.value <= saved_film_get_target_tick())
                    break;
                g_saved_film_recording_globals.state = 4;
                saved_film_set_playback_rate(0.0f, 1);
                break;
            case _film_control_exit:
                {
                    void *memory = ::user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), (char const *)(0), 0);
                    if (!memory)
                        break;
                    e_controller_index controller_index = message->m_controller;
                    long name = m_name;
                    c_load_dialog_screen_message *dialog = new (memory)
                        c_load_dialog_screen_message(controller_index, get_render_window(), name, _film_control_exit_dialog, name);
                    if (!dialog)
                        break;
                    dialog->set_parent_screen_index(m_screen_index);
                    ::user_interface_messaging_post(dialog);
                    break;
                }
            default:
                handled = false;
                break;
            }
        }
    }
    return handled;
}

// @address 0x4df458 ?handle_dialog_result@c_saved_film_control_pad@@UAA_NPBVc_dialog_result_message@@@Z
bool c_saved_film_control_pad::handle_dialog_result(const c_dialog_result_message *message)
{
    bool handled = false;
    switch (message->m_dialog_name)
    {
    case _film_control_exit_dialog:
        if (message->m_dialog_result == _gui_dialog_choice_first)
        {
            if (g_saved_film_playback && g_saved_film_playback->state != 0)
            {
                void *memory = ::user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), (char const *)(0), 0);
                if (memory)
                {
                    long name = m_name;
                    e_controller_index controller = message->m_controller;
                    c_load_dialog_screen_message *dialog = new (memory) c_load_dialog_screen_message(controller,
                        get_render_window(), m_name, _film_control_upload_exit_dialog, name);
                    if (dialog)
                        ::user_interface_messaging_post(dialog);
                }
            }
            else
                game_saved_film_start_timer();
        }
        handled = true;
        break;
    case _film_control_upload_exit_dialog:
        if (message->m_dialog_result == _gui_dialog_choice_first)
        {
            saved_film_reset_playback_upload(g_saved_film_playback);
            game_saved_film_start_timer();
        }
        handled = true;
        break;
    }
    if (!handled)
        handled = saved_film_control_pad_handle_storage_result(message);
    return handled;
}

static __forceinline s_saved_film_playback *saved_film_control_pad_playback()
{
    return g_saved_film_playback;
}

void c_saved_film_control_pad::update(unsigned long current_milliseconds)
{
    long state = saved_film_get_save_state();
    c_gui_data *data = saved_film_control_pad_find_data(this, 0x1055e);
    c_gui_widget *list = this->get_child_widget(_gui_list_widget, 0x1055f);
    c_gui_screen_widget::update(current_milliseconds);
    refresh_button_keys();
    if (data && list)
    {
        if (state == 0)
            list->set_enabled(true);
        else
            list->set_enabled(false);
        for (c_gui_widget *item = gui_widget_find_child_by_name(list, _gui_list_item_widget); item;
            item = gui_widget_get_next_list_item(item, true))
        {
            long handle = item->get_element_handle();
            long button_name;
            if (handle == -1 || !data->get_string_id_value(handle, 0x10010, &button_name))
                continue;
            for (c_gui_widget *bitmap = gui_widget_find_child_by_name(item, _gui_bitmap_widget); bitmap;
                bitmap = gui_widget_get_next_sibling_of_type(bitmap, _gui_bitmap_widget))
            {
                if (bitmap->m_name != _film_control_hilite)
                    bitmap->set_visible(false);
            }
            c_gui_widget *bitmap = item->get_child_widget(_gui_bitmap_widget, button_name);
            if (bitmap)
                bitmap->set_visible(true);
            switch (state)
            {
            case 0:
                if (button_name == _film_control_save_primary)
                {
                    if (saved_film_control_pad_playback() && saved_film_control_pad_playback()->state != 0)
                        item->set_enabled(false);
                    else
                        item->set_enabled(true);
                }
                break;
            case 3:
                if (button_name == _film_control_set_start)
                    item->set_enabled(true);
                else
                    item->set_enabled(false);
                break;
            case 6:
                if (button_name == _film_control_set_end && g_saved_film_playback_tick.value > saved_film_get_target_tick())
                {
                    item->set_enabled(true);
                    break;
                }
            case 1:
            case 2:
            case 4:
            case 5:
            case _saved_film_recording_waiting_for_playback:
                item->set_enabled(false);
                break;
            }
            if (button_name == _film_control_skip_back && !saved_film_can_skip(1))
                item->set_enabled(false);
            if (button_name == _film_control_skip_forward && !saved_film_can_skip(2))
                item->set_enabled(false);
            if (button_name == _film_control_start_recording)
            {
                bool enabled = simulation_starting_up() && tls_game_globals()->options.game_mode == 2 &&
                    tls_game_globals()->options.game_playback == 1 && tls_player_mapping_globals()->local_player_count == 1 &&
                    saved_film_get_target_tick() == -1 && saved_film_get_current_tick() != 0 && controller_get_first_signed_in_non_guest() != -1;
                if (!enabled)
                    item->set_enabled(false);
            }
            if (button_name == _film_control_save_primary)
            {
                bool enabled = simulation_starting_up() && tls_game_globals()->options.game_playback == 1 &&
                    tls_player_mapping_globals()->local_player_count == 1;
                if (!enabled)
                    item->set_enabled(false);
            }
            if (button_name == _film_control_reset_speed && tls_game_globals()->options.game_mode == 1)
                item->set_enabled(false);
        }
    }
    list = this->get_child_widget(_gui_list_widget, 0x1055f);
    if (list)
    {
        long datasource_name = ((c_gui_list_widget *)list)->m_datasource_name;
        set_text_widget_string_id_from_datasource(0x1002b, datasource_name, ((c_gui_list_widget *)list)->get_focused_element_handle(), 0x10010);
    }

    if (m_previous_state != 4 && state == 4)
    {
        void *memory = ::user_interface_malloc_tracked(sizeof(c_load_screen_message), (char const *)(0), 0);
        if (memory)
        {
            c_load_screen_message *message = new(memory) c_load_screen_message(0x1056e, _saved_film_no_controller, get_render_window(), 0x1000c);
            if (message)
            {
                ::user_interface_messaging_post(message);
                saved_film_control_pad_close(this);
            }
        }
    }
    if (saved_film_try_and_get_timer_elapsed(0))
        saved_film_control_pad_close(this);
    m_previous_state = state;
}
