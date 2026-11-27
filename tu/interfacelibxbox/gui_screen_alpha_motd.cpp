// interfacelibxbox/gui_screen_alpha_motd.obj: from the records 0x4d66d8 0x4d6760 0x4d68c0 0x4d6950 0x4d6a38 0x4d6ac0 0x4d6af8 0x4d6b80
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "memory/tls_globals.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_text_widget.h"
#include "interface/user_interface_messages.h"
#include "bink/bink_playback.h"
#include "interface/gui_screens/start_menu/panes/game_editor/start_menu_game_editor.h"

#include "bink/bink_playback_types.h"

#include "interface/user_interface_window_manager.h"



#include "interface/c_gui_screen_widget.h"
#include "game/game_engine_scoring_globals.h"

#include "interface/gui_screens/alpha/gui_screen_alpha_legal.h"
#include "interface/gui_screens/dialog/gui_screen_dialog.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "interface/user_interface.h"
#include "tag_files/string_ids.h"

#include "interface/gui_screens/alpha/gui_screen_alpha_motd.h"


// @address 0x4d66d8 ?start_timeout@c_gui_screen_alpha_legal@@QAAXXZ
void c_gui_screen_alpha_legal::start_timeout()
{
    m_timeout_time = GetTickCount() + 2000;
    m_bink_active = bink_globals.active && bink_globals.enabled;
    if (m_bink_active) {
        g_window_manager.m_fade_in = true;
        g_window_manager.m_fade = 1.0f;
    }
}

// @address 0x4d6760 ?update@c_gui_screen_alpha_legal@@UAAXK@Z
void c_gui_screen_alpha_legal::update(unsigned long current_milliseconds)
{
    long now = GetTickCount();
    c_gui_screen_widget::update(current_milliseconds);
    long zero = 0;
    unsigned long motion_active = bink_globals.active != 0 && bink_globals.enabled != 0;
    if ((unsigned char)motion_active != 0) {
        m_timeout_time = now + 2000;
        m_bink_active = 1;
    } else if (m_bink_active != 0) {
        g_window_manager.m_fade_in = false;
        m_bink_active = zero;
    }
    if (now >= m_timeout_time) {
        c_gui_widget *spinner = get_child_widget(_gui_group_widget, _string_id_in_progress);
        if (spinner != 0)
            spinner->set_visible(false);
        gui_screen_widget_set_display_group(this, _display_group_type_button_key, _gui_dialog_string_default_button_key);
    } else {
        c_gui_widget *spinner = get_child_widget(_gui_group_widget, _string_id_in_progress);
        if (spinner != 0)
            spinner->set_visible(true);
        gui_screen_widget_set_display_group(this, _display_group_type_button_key, _start_menu_string_empty);
    }
    if (m_dismissed != 0) {
        unsigned long scale_ready = g_window_manager.m_fade == 1.0f;
        if ((unsigned char)scale_ready != 0) {
            transition_out_with_transition_type(0, _screen_transition_normal);
            m_dismissed = zero;
            g_eula_accepted = 1;
        }
    }
}

// @address 0x4d68c0 ?handle_controller_input_message@c_gui_screen_alpha_legal@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_alpha_legal::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    long now = (long)GetTickCount();
    if (!m_dismissed && message->get_event_type() == 13 &&
        (unsigned long)message->get_component() < 2 && now >= m_timeout_time) {
        handled = true;
        g_window_manager.m_fade_in = true;
        m_dismissed = true;
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}


static __forceinline void gui_screen_alpha_motd_ascii_to_wchar(const char *source, wchar_t *destination, long maximum_count)
{
    const char *read = source;
    wchar_t *write = destination;
    long remaining = maximum_count;
    while (remaining > 0) {
        unsigned char character = (unsigned char)*read;
        if (remaining == 1)
            *write = 0;
        else if (character <= 127)
            *write = (wchar_t)(signed char)*read;
        else
            *write = 0x25a1;
        if (*read == 0)
            break;
        ++write;
        ++read;
        --remaining;
    }
}

// @address 0x4d6950 ?post_initialize@c_gui_screen_alpha_motd@@UAAXXZ
void c_gui_screen_alpha_motd::post_initialize()
{
    c_gui_screen_widget::post_initialize();

    c_gui_text_widget *text_widget = get_child_text_widget(0x10005);
    if (text_widget != 0) {
        wchar_t text[4096];
        text[0] = 0;
        if (m_text_length >= 0 && (unsigned long)m_text_length < 4096)
            m_text[m_text_length] = 0;
        gui_screen_alpha_motd_ascii_to_wchar(m_text, text, 4096);
        text_widget->set_text(text);
    }
}

// @address 0x4d6a38 ?handle_controller_input_message@c_gui_screen_alpha_motd@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_alpha_motd::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (message->get_event_type() == _event_type_button_press && message->get_component() == 0) {
        transition_out_with_transition_type(0, _screen_transition_normal);
        handled = true;
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

// @address 0x4d6ac0 ?post_initialize@c_gui_screen_alpha_locked_down@@UAAXXZ
void c_gui_screen_alpha_locked_down::post_initialize()
{
    m_timeout_time = GetTickCount() + 2000;
}

// @address 0x4d6af8 ?handle_controller_input_message@c_gui_screen_alpha_locked_down@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_alpha_locked_down::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (message->get_event_type() == _event_type_button_press) {
        handled = false;
        if ((unsigned long)message->get_component() < 2) {
            transition_out_with_transition_type(0, _screen_transition_normal);
            handled = true;
        }
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}



// @address 0x4d6b80 ?handle_controller_input_message@c_gui_screen_alpha_motd_editor@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_alpha_motd_editor::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (message->get_event_type() == 13) {
        switch (message->get_component()) {
        case 0:
            tls_game_engine_globals()->sandbox.variant_file_exists = true;
            if (m_save_as) {
                c_start_menu_game_editor *editor = m_editor;
                start_menu_game_editor_request_save_variant_as(editor, message->get_controller());
                editor->m_save_state = -1;
                editor->m_dialog_action = 0;
            }
            else {
                start_menu_game_editor_request_save_variant(m_editor, message);
            }
            transition_out_with_transition_type(0, _screen_transition_normal);
            handled = true;
            break;
        case 1:
            transition_out_with_transition_type(0, _screen_transition_normal);
            handled = true;
            break;
        }
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

