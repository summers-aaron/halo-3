// interfacelibxbox/gui_screen_dialog.obj: from the records 0x4a7d50 0x4a7f10 0x4c0e98 0x4c0ea0 0x4c0f38 0x4c0f98 0x4c0fa0 0x4c10a8 0x4c1160 0x4c11d0 0x4c1268 0x4c1308 0x4c1458
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/game_engine_scoring_globals.h"
#include "interface/gui_data.h"
#include "interface/gui_screens/dialog/gui_screen_dialog.h"
#include "interface/gui_screen_dialog_externs.h"
#include "interface/gui_screen_widget_legacy.h"
#include "memory/tls_globals.h"
#include "game/game_time_types.h"
#include <stddef.h>
#include "shell/shell.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/c_deletable.h"
#include "interface/user_interface.h"
#include "interface/user_interface_shared_globals.h"
#include "interface/user_interface_memory.h"
#include "tag_files/tag_groups.h"
#include <new>
struct s_user_interface_shared_globals;
class c_message;
class c_gui_list_widget;
class c_gui_data;
#include "interface/c_gui_list_widget.h"
class c_gui_data;
#include "interface/user_interface_messages.h"
#include "interface/user_interface_allocation.h"
#include "game/game_time_inline.h"
#include "interface/user_interface_message_inlines.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/c_gui_group_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_dialog_definition.h"
#include "interface/gui_screens/sandbox_menu/gui_screen_sandbox_menu.h"
#include "tag_files/string_ids.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"

// @address 0x8c4950 g_gui_last_dialog_name
long g_gui_last_dialog_name;
// @address 0x4c0e98 ?get_current_item_count_internal@c_dialog_screen_widget_dialog_items_datasource@@MAAJXZ
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x4869e8 ?set_text_widget_string_id@c_gui_screen_widget@@QAAXJJ@Z
// @address 0x485350 ?dispose@c_gui_screen_widget@@UAAXXZ
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x58d720 GetProcessHeap

// @address 0x5bec98 ?initialize@c_message@@UAAXXZ
// @address 0x5bec98 ?update@c_message@@UAAXXZ

__forceinline c_dialog_result_message::c_dialog_result_message(long screen_name,
    e_controller_index controller, e_window_index window, long dialog_name, e_gui_dialog_choice choice)
    : c_message(_message_type_dialog_result, screen_name, controller, window), m_dialog_result(choice), m_dialog_name(dialog_name)
{
}


static __forceinline void update_placeable_object(c_gui_widget *child, long index, const c_gui_placeable_object_datasource *list)
{
    if (index != -1)
    {
        s_game_engine_globals *g = tls_game_engine_globals();
        c_gui_placeable_object_datasource::s_sandbox_placeable_object definition = list->get_placeable_object(index);
        c_map_variant *variant = &g->map_info;
        long budget = map_variant_find_quota_index(variant, definition.object_definition_index);
        if (budget != -1)
        {
            s_variant_quota *entry = &g->map_info.quotas[budget];
            if (entry->placed_on_map >= entry->maximum_allowed ||
                (variant->current_budget + entry->price > variant->maximum_budget &&
                    entry->maximum_count <= entry->placed_on_map))
                child->set_enabled(false);
        }
    }
}

long c_dialog_screen_widget_dialog_items_datasource::get_current_item_count_internal()
{
    return m_current_item_count;
}

// @address 0x4c0ea0 ?get_string_id_value@c_dialog_screen_widget_dialog_items_datasource@@UAA_NJJPAJ@Z
bool c_dialog_screen_widget_dialog_items_datasource::get_string_id_value(long index, long name, long *value)
{
    bool found = false;
    if (index >= 0 && index < m_current_item_count && name == _string_id_name)
    {
        *value = m_values[index];
        found = true;
    }
    return found;
}

// @address 0x4a7d50 ?update@c_sandbox_object_menu_screen_widget@@UAAXK@Z
void c_sandbox_object_menu_screen_widget::update(unsigned long time)
{
    gui_screen_widget_update(this, time);
    gui_screen_widget_set_display_group(this, 0, m_category);
    c_gui_list_widget *list = get_child_list_widget(m_category);
    if (list)
    {
        if (!list->within_focus_chain())
        {
            c_gui_placeable_object_datasource *source = ((c_gui_placeable_object_datasource *)list->get_data());
            if (source)
                this->transfer_focus_to_list(list, source->get_first_element_handle(), true, false);
        }
        list->set_enabled(true);
        c_gui_placeable_object_datasource *source = ((c_gui_placeable_object_datasource *)list->get_data());
        for (c_gui_widget *child = gui_widget_find_child_by_name(list, _gui_list_item_widget); child; child = gui_widget_get_next_list_item(child, true))
        {
            long index = child->get_element_handle();
            update_placeable_object(child, index, source);
        }
    }
}

// @address 0x4a7f10 ?dispose@c_sandbox_object_menu_screen_widget@@UAAXXZ
void c_sandbox_object_menu_screen_widget::dispose()
{
    c_gui_screen_widget::dispose();
    g_gui_last_dialog_name = m_category;
}

// @address 0x4c1160 ?update@c_gui_dialog_screen_widget@@UAAXK@Z
void c_gui_dialog_screen_widget::update(unsigned long time)
{
    c_gui_group_widget *button_key_group = m_current_display_group_widgets[c_gui_screen_widget::_display_group_type_button_key];
    set_text_widget_string_id(_string_id_title, m_title);
    set_text_widget_string_id(_start_menu_string_message, m_description);
    if (button_key_group && button_key_group->m_name != m_button_key)
        gui_screen_widget_set_display_group(this, 1, m_button_key);
    gui_screen_widget_update(this, time);
}

// @address 0x4c0f98 ?post_initialize@c_gui_dialog_screen_widget@@UAAXXZ
void c_gui_dialog_screen_widget::post_initialize()
{
    c_gui_screen_widget::post_initialize();
}

// @address 0x4c11d0 ?handle_controller_input_message@c_gui_dialog_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_dialog_screen_widget::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (message->m_event_type == _event_type_button_press && message->m_component == 1)
    {
        if (m_b_button_action != 1)
            transition_out_with_transition_type(1, _screen_transition_normal);
        handled = true;
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

bool c_gui_dialog_screen_widget::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget, c_gui_data *data)
{
    bool result = false;
    if (list_name == _gui_dialog_string_dialog_items)
    {
        send_dialog_result(message->get_controller(), (e_gui_dialog_choice)list_item_widget->get_element_handle());
        transition_out_with_transition_type(0, _screen_transition_normal);
        result = true;
    }
    if (!result)
        result = c_gui_screen_widget::handle_list_item_chosen(message, list_name, list_item_widget, data);
    return result;
}

void c_gui_dialog_screen_widget::set_dialog_name(long dialog_name)
{
    const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
    m_dialog_name = dialog_name;
    if (globals)
    {
        for (long dialog_index = 0; dialog_index < globals->dialogs.count; ++dialog_index)
        {
            const s_gui_dialog_definition *dialog = TAG_BLOCK_GET_ELEMENT(&globals->dialogs, dialog_index, const s_gui_dialog_definition);
            if (dialog->name == dialog_name)
            {
                m_title = dialog->title;
                m_description = dialog->description;
                m_description_flags = dialog->description_flags;
                m_button_key = dialog->button_key ? dialog->button_key : _gui_dialog_string_default_button_key;
                m_choices[m_choice_count] = dialog->choices[0];
                ++m_choice_count;
                if (dialog->choices[1])
                {
                    m_choices[m_choice_count] = dialog->choices[1];
                    ++m_choice_count;
                }
                if (dialog->choices[2])
                {
                    m_choices[m_choice_count] = dialog->choices[2];
                    ++m_choice_count;
                }
                if (dialog->choices[3])
                {
                    m_choices[m_choice_count] = dialog->choices[3];
                    ++m_choice_count;
                }
                m_b_button_action = dialog->b_button_action;
                set_initial_focus(_gui_dialog_string_dialog_items, dialog->initial_choice);
                break;
            }
        }
    }
}

void c_gui_dialog_screen_widget::send_dialog_result(e_controller_index controller, e_gui_dialog_choice choice)
{
    if (!m_test_mode)
    {
        c_dialog_result_message *message = new (0, 0, _ui_allocation_marker_dummy)
            c_dialog_result_message(m_dialog_invoker, controller, get_render_window(), m_dialog_name, choice);
        if (message)
        {
            message->set_dispose_on_success_screen_index(m_dispose_on_success_screen_index);
            ::user_interface_messaging_post((c_message const *)(c_deletable *)message);
            m_result_sent = true;
        }
    }
}

// @address 0x4c10a8 ?dispose@c_gui_dialog_screen_widget@@UAAXXZ
void c_gui_dialog_screen_widget::dispose()
{
    if (!m_result_sent && m_b_button_action != 0 && m_b_button_action != 1)
    {
        long choice;
        switch (m_b_button_action - 2)
        {
        case 0:
            choice = 0;
            break;
        case 1:
            choice = 1;
            break;
        case 2:
            choice = 2;
            break;
        case 3:
            choice = 3;
            break;
        default:
            // match: 0x4c10a8: unreachable hint omits dead branch check
            __assume(0);
        }
        send_dialog_result(this->get_any_responding_controller(), (e_gui_dialog_choice)choice);
    }
    c_gui_screen_widget::dispose();
}

// @address 0x53460 ??_7c_dialog_screen_widget_dialog_items_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_dialog_screen_widget_dialog_items_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_dialog_screen_widget_dialog_items_datasource@@UAAPAXI@Z
// @address 0x4b4128 ?get_column_names@c_dialog_screen_widget_dialog_items_datasource@@UAAXQAJPAJ@Z
// @address 0x4c0fa0 ?initialize_datasource@c_gui_dialog_screen_widget@@UAAXXZ
void c_gui_dialog_screen_widget::initialize_datasource()
{
    gui_screen_widget_initialize_datasource(this);
    void *memory = ::user_interface_malloc_tracked(sizeof(c_dialog_screen_widget_dialog_items_datasource), (char const *)(0), 0);
    c_dialog_screen_widget_dialog_items_datasource *data =
        memory ? new (memory) c_dialog_screen_widget_dialog_items_datasource(m_choices, m_choice_count) : 0;
    if (data) {
        if (data->initialize(_gui_dialog_string_dialog_items)) {
            m_datasource[m_datasource_count] = data;
            ++m_datasource_count;
        } else {
            data->~c_dialog_screen_widget_dialog_items_datasource();
            ::user_interface_free((void const *)(data));
        }
    }
}

// @address 0x53398 ??_7c_gui_dialog_screen_widget@@6B@
// @address 0x4a8838 ??_Ec_gui_dialog_screen_widget@@UAAPAXI@Z
// @address 0x4c0e90 ?get_render_data_size@c_gui_dialog_screen_widget@@MAA?AW4e_render_data_size@@XZ
// @address 0x4c0f38 ??0c_gui_dialog_screen_widget@@QAA@J@Z
c_gui_dialog_screen_widget::c_gui_dialog_screen_widget(long name) :
    c_gui_screen_widget(name),
    m_title(-1),
    m_description(-1),
    m_button_key(-1),
    m_choice_count(0),
    m_b_button_action(0),
    m_test_mode(false),
    m_result_sent(false)
{
}
