// interfacelibxbox/gui_screen_in_progress.obj: from the records 0x4c1548 0x4c1598 0x4c1770 0x4c1a60 0x4c1af0 0x4c1b28 0x4c1bf8 0x4c1c80 0x4c1dc0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include <stddef.h>
#include "interface/gui_screen_in_progress_externs.h"
#include "interface/gui_screens/gui_screen_in_progress.h"
#include "memory/static_arrays.h"
#include "shell/shell.h"
#include "interface/gui_widget_types.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface.h"
#include "interface/user_interface_session.h"
#include "interface/user_interface_utilities.h"
#include "interface/experience_rank_types.h"
#include "interface/experience_grade_types.h"
#include "interface/c_gui_text_widget.h"
#include "tag_files/string_ids.h"

#include "game/player_configuration.h"

class c_font_cache;
class c_gui_widget;
struct s_player_appearance;
struct s_player_configuration;
class c_gui_selected_item;



#include "interface/gui_data.h"



#include "interface/user_interface_shared_globals.h"

#include "interface/gui_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screen_widget_legacy.h"


#include "interface/user_interface_global_string_inlines.h"


// @address 0x4c1548 ?update@c_gui_screen_in_progress@@UAAXK@Z
void c_gui_screen_in_progress::update(unsigned long current_milliseconds)
{
    c_gui_screen_widget::update(current_milliseconds);
    set_text_widget_string_id(0x29d, m_title);
    set_text_widget_string_id(0x10005, m_message);
}

// @address 0x4c1598 ?update@c_gui_screen_spartan_milestone_dialog@@UAAXK@Z
void c_gui_screen_spartan_milestone_dialog::update(unsigned long current_milliseconds)
{
    c_gui_screen_widget::update(current_milliseconds);
    c_gui_data *data = 0;
    for (long index = 0; index < m_datasource_count; index++)
    {
        if (m_datasource[index]->get_name() == 0x1006c)
        {
            data = m_datasource[index];
            break;
        }
    }
    if (data != 0)
    {
        long element = data->get_first_element_handle();
        if (element != -1)
        {
            long title;
            while (!data->get_integer_value(element, 0x10a, &title) || title != m_milestone)
            {
                element = data->get_next_element_handle(element);
                if (element == -1)
                    break;
            }
            if (element != -1)
            {
                c_gui_widget *sprite = get_child_widget(_gui_bitmap_widget, 0x3f5);
                if (sprite != 0)
                {
                    if (data->get_integer_value(element, 0x3f5, &title))
                    {
                        sprite->set_visible(true);
                        ((c_gui_bitmap_widget *)sprite)->set_sprite_frame(title);
                    }
                    else
                    {
                        sprite->set_visible(false);
                    }
                }
                set_text_widget_string_id_from_datasource(0x29d, 0x1006c, element, 0x29d);
                set_text_widget_string_id_from_datasource(0x29e, 0x1006c, element, 0x29e);
            }
        }
    }
    gui_screen_in_progress_update_player_display(this);
    long name;
    if (GetTickCount() > m_input_enable_time)
        name = _string_id_button_key_a_ok;
    else
        name = _string_id_button_key_wait;
    gui_screen_widget_set_display_group(this, 1, name);
}

// @address 0x4c1770 gui_screen_in_progress_update_player_display
extern "C" void gui_screen_in_progress_update_player_display(c_gui_screen_widget *screen)
{
    int index;
    const s_player_configuration *info;
    c_gui_widget *widget;
    int found;
    wchar_t title[256];
    wchar_t text[1024];
    wchar_t row_text[1024];
    wchar_t category_text[1024];

    index = ((c_gui_widget *)screen)->get_driving_controller();
    if (index == -1 || index == 255 ||
        (index = ::user_interface_session_get_player_at_local_controller((e_controller_index)index)) == -1 ||
        (info = user_interface_session_get_player_data(index)) == 0)
    {
        screen->transition_out_with_transition_type(0, _screen_transition_normal);
        return;
    }
    widget = screen->get_child_widget(_gui_text_widget, 686);
    if (widget)
        ((c_gui_text_widget *)widget)->set_text(info->host.name);
    text[0] = 0;
    if (info->client.appearance.player_model_choice == 1)
        found = user_interface_global_string_get_unchecked(198, text);
    else
        found = user_interface_global_string_get_unchecked(197, text);
    if ((unsigned char)found)
    {
        title[0] = 0;
        ((c_static_wchar_string<256> *)title)->print(L"%s - %s", text, info->client.appearance.service_tag);
        gui_screen_widget_set_text_widget_text(screen, 0x103cd, title);
    }
    widget = screen->get_child_widget(_gui_bitmap_widget, 0x103bc);
    if (widget)
    {
        bool enabled = info->host.global_stats_valid;
        widget->set_visible(enabled);
        if (enabled)
        {
            unsigned int row = info->host.global_rank;
            unsigned int column = info->host.global_grade;
            int value = -1;
            if (row <= 13 && column <= 3)
                value = g_experience_rank_icon_element_handles[row][column];
            ((c_gui_bitmap_widget *)widget)->set_sprite_frame(value);
        }
    }
    widget = screen->get_child_widget(_gui_bitmap_widget, 0x103ba);
    if (widget)
    {
        bool has_value = info->host.hopper_stats_valid;
        widget->set_visible(has_value);
        if (has_value)
            ((c_gui_bitmap_widget *)widget)->set_sprite_frame(info->host.hopper_skill_display);
    }
    widget = screen->get_child_widget(_gui_text_widget, 0x103af);
    if (widget)
    {
        row_text[0] = 0;
        if (info->host.global_stats_valid &&
            user_interface_get_player_rank_and_grade_string(
                (e_experience_rank)info->host.global_rank, (e_experience_grade)info->host.global_grade, true, (c_static_wchar_string<1024> *)row_text))
        {
            widget->set_visible(true);
            ((c_gui_text_widget *)widget)->set_text(row_text);
        }
        else
        {
            widget->set_visible(false);
        }
    }
    widget = screen->get_child_widget(_gui_text_widget, 0x103ac);
    if (widget)
    {
        category_text[0] = 0;
        if (info->host.global_stats_valid && user_interface_get_player_total_xp(info->host.global_experience, (c_static_wchar_string<1024> *)category_text))
        {
            widget->set_visible(true);
            ((c_gui_text_widget *)widget)->set_text(category_text);
        }
        else
        {
            widget->set_visible(false);
        }
    }
}

// @address 0x4c1a60 ?handle_controller_input_message@c_gui_screen_spartan_milestone_dialog@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_spartan_milestone_dialog::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (message->get_event_type() == _event_type_button_press && message->get_component() == _controller_component_none &&
        GetTickCount() > m_input_enable_time)
    {
        transition_out_with_transition_type(0, _screen_transition_normal);
        handled = true;
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

// @address 0x4c1af0 ?initialize@c_gui_screen_spartan_rank_dialog@@UAAXXZ
void c_gui_screen_spartan_rank_dialog::initialize()
{
    c_gui_screen_widget::initialize();
    ::user_interface_play_sound(_ui_sound_screen_in_progress, m_core_definition.sound_override_index);
}

// @address 0x4c1b28 ?update@c_gui_screen_spartan_rank_dialog@@UAAXK@Z
void c_gui_screen_spartan_rank_dialog::update(unsigned long current_milliseconds)
{
    c_gui_screen_widget::update(current_milliseconds);
    unsigned long timeout = (unsigned long)get_constants_datasource_integer(0x1006f);
    c_gui_widget *first_widget = get_child_widget(_gui_group_widget, 0x10070);
    c_gui_widget *second_widget = get_child_widget(_gui_group_widget, 0x10071);
    set_responds_to_controller_events(false);
    gui_screen_in_progress_update_title(this);
    if (first_widget != 0)
        gui_screen_in_progress_update_rank_widget(first_widget, (unsigned int)m_previous_rank, (unsigned int)m_previous_grade);
    if (second_widget != 0)
        gui_screen_in_progress_update_rank_widget(second_widget, (unsigned int)m_current_rank, (unsigned int)m_current_grade);
    if (current_milliseconds - m_creation_time_milliseconds > timeout)
        transition_out_with_transition_type(0, _screen_transition_normal);
}

// @address 0x4c1bf8 ?update_render_state@c_gui_screen_spartan_dialog@@UAAXK@Z
void c_gui_screen_spartan_dialog::update_render_state(unsigned long current_milliseconds)
{
    c_gui_widget::update_render_state(current_milliseconds);
    int result = get_driving_controller();
    if (result != -1 && result != 255)
    {
        result = ::user_interface_session_get_player_at_local_controller((e_controller_index)result);
        if (result != -1)
        {
            const s_player_configuration *configuration = user_interface_session_get_player_data(result);
            if (configuration != 0)
            {
                unsigned char team_color = (unsigned char)configuration->client.appearance.primary_color;
                tint_widget_to_change_color(get_child_widget(_gui_bitmap_widget, 0x10127), (signed char)team_color, false);
            }
        }
    }
}

// @address 0x4c1c80 gui_screen_in_progress_update_title
extern "C" void gui_screen_in_progress_update_title(c_gui_screen_widget *screen)
{
    int index;
    const s_player_configuration *info;
    c_gui_widget *widget;
    wchar_t title[256];
    wchar_t text[1024];
    int found;

    index = ((c_gui_widget *)screen)->get_driving_controller();
    if (index == -1 || index == 255 ||
        (index = ::user_interface_session_get_player_at_local_controller((e_controller_index)index)) == -1 ||
        (info = user_interface_session_get_player_data(index)) == 0)
    {
        screen->transition_out_with_transition_type(0, _screen_transition_normal);
        return;
    }
    widget = screen->get_child_widget(_gui_text_widget, 686);
    if (widget)
        ((c_gui_text_widget *)widget)->set_text(info->host.name);
    text[0] = 0;
    if (info->client.appearance.player_model_choice == 1)
        found = user_interface_global_string_get_unchecked(198, text);
    else
        found = user_interface_global_string_get_unchecked(197, text);
    if ((unsigned char)found)
    {
        title[0] = 0;
        ((c_static_wchar_string<256> *)title)->print(L"%s - %s", text, info->client.appearance.service_tag);
        gui_screen_widget_set_text_widget_text(screen, 0x103cd, title);
    }
}

// @address 0x4c1dc0 gui_screen_in_progress_update_rank_widget
extern "C" void gui_screen_in_progress_update_rank_widget(c_gui_widget *widget, unsigned int row, unsigned int column)
{
    wchar_t text[1024];
    c_gui_widget *sprite = widget->get_child_widget(_gui_bitmap_widget, 0x3f5);
    if (sprite != 0)
    {
        int frame = -1;
        if (row <= 13 && column <= 3)
            frame = g_experience_rank_icon_element_handles[row][column];
        if (frame != -1)
        {
            sprite->set_visible(true);
            ((c_gui_bitmap_widget *)sprite)->set_sprite_frame(frame);
        }
        else
        {
            sprite->set_visible(false);
        }
    }
    c_gui_widget *text_widget = widget->get_child_widget(_gui_text_widget, 0x29d);
    if (text_widget != 0)
    {
        text[0] = 0;
        if (user_interface_get_player_rank_and_grade_string((e_experience_rank)row, (e_experience_grade)column, true, (c_static_wchar_string<1024> *)text) != 0)
        {
            text_widget->set_visible(true);
            ((c_gui_text_widget *)text_widget)->set_text(text);
            return;
        }
        text_widget->set_visible(false);
    }
}

typedef char gui_in_progress_info_category_offset[(offsetof(s_player_configuration, host.global_experience) == 0xf4) ? 1 : -1];
typedef char gui_in_progress_info_value_offset[(offsetof(s_player_configuration, host.hopper_skill_display) == 0x108) ? 1 : -1];
