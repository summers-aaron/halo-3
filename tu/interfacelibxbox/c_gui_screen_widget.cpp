// interfacelibxbox/c_gui_screen_widget.obj: from the records 0x485008 0x485118 0x485190 0x485230 0x485350 0x4853e8 0x485478 0x485560 0x485630 0x485640 0x485758 0x4858d8 0x4859b0 0x4859b8 0x485b10 0x485d20 0x485d90 0x486030 0x4860a0 0x486130 0x486340 0x486448 0x486498 0x486578 0x4865f8 0x486668 0x486700 0x4868a8 0x486918 0x486988 0x486998 0x4869e8 0x486a48 0x486a98 0x486ba0 0x486ca0 0x486dd8 0x486f08 0x486f80 0x487148 0x487228 0x4bdfc0 0x4be280 0x4be958 0x4beb40 0x4bebc8 0x4bece0 0x4bf0e8
// @address 0x485190 ?initialize@c_gui_screen_widget@@UAAXXZ
// @address 0x485230 ?initialize_datasource@c_gui_screen_widget@@UAAXXZ
// @compiler xdk6130
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "hs/hs.h"
// @flags /TP /O2 /Oc
#include "interface/controller_types.h"
 #include "interface/c_gui_screen_widget_externs.h"
#include "interface/gui_screen_widget_datasources.h"
#include "interface/c_gui_data_array_test.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "math/real_math_types.h"
#include "interface/user_interface_data.h"
#include "memory/static_arrays.h"
#include "multithreading/synchronized_value.h"
#include "memory/data_types.h"
#include "memory/data_handle_access.h"
#include "shell/shell.h"
#include "interface/gui_widget_types.h"
#include "interface/core_widget_definition.h"
#include "tag_files/tag_types.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "text/text_group.h"
#include "hs/hs_runtime.h"
#include "interface/user_interface_text_parser.h"
#include "interface/c_gui_model_widget.h"
#include "interface/gui_screens/start_menu/panes/settings_camera/start_menu_settings_camera.h"
#include "interface/gui_screens/start_menu/panes/settings_controls/start_menu_settings_controls.h"
#include "interface/gui_screens/start_menu/panes/settings_display/start_menu_settings_display.h"
#include "interface/user_interface_window_manager.h"
#include "game/players.h"
#include "saved_games/content_catalogue.h"
#include "interface/game_tag_parser.h"
#include "interface/c_gui_group_widget.h"
#include "interface/c_gui_button_key_widget.h"
#include "interface/c_gui_text_widget.h"
#include "math/real_math_types.h"
#include "hs/hs_thread_types.h"
#include "hs/hs_script_types.h"
#include "cache/cache_files.h"
#include "interface/user_interface_sound.h"
class c_gui_data;
class c_gui_list_widget;
// @address 0x486f08 ?add_game_tag_parser@c_gui_screen_widget@@IAAXPAVc_game_tag_parser@@@Z
// @address 0x4859b8 ?add_definition_fields@c_gui_screen_widget@@QAAXPBUs_screen_widget_definition@@_N@Z
// @address 0x486ca0 ?get_constants_datasource_integer@c_gui_screen_widget@@QAAJJ@Z
// @address 0x486998 ?get_string_by_string_id@c_gui_screen_widget@@UAA_NJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x485d90 ?set_initial_focused_widget@c_gui_screen_widget@@AAAXXZ
// @address 0x487228 ??0s_runtime_screen_widget_definition@@QAA@XZ
// @address 0x485350 ?dispose@c_gui_screen_widget@@UAAXXZ
// @address 0x485560 ?post_initialize@c_gui_screen_widget@@UAAXXZ
// @address 0x486030 ?swap_display_group_block@c_gui_screen_widget@@AAAXXZ
// @address 0x485118 ?reconstruct@c_gui_screen_widget@@UAAXXZ
// @address 0x485640 ?load_display_group@c_gui_screen_widget@@AAAPAVc_gui_group_widget@@PBUs_runtime_screen_widget_definition@@W4e_display_group_type@1@J@Z
// @address 0x4858d8 ?dispose_display_group@c_gui_screen_widget@@AAAXPAVc_gui_group_widget@@@Z
// @address 0x485b10 ?create_and_add_child_widgets_from_definition@c_gui_screen_widget@@AAAXPAUs_runtime_screen_widget_definition@@@Z
// @address 0x4869e8 ?set_text_widget_string_id@c_gui_screen_widget@@QAAXJJ@Z
// @address 0x486a48 gui_screen_widget_set_text_widget_text
// @address 0x486a98 ?set_text_widget_text_from_datasource@c_gui_screen_widget@@QAAXJJJJ@Z
// @address 0x486ba0 ?set_text_widget_string_id_from_datasource@c_gui_screen_widget@@QAAXJJJJ@Z
// @address 0x486498 ?handle_focused_widget_selected@c_gui_screen_widget@@UAA_NPBVc_controller_input_message@@PAVc_gui_widget@@@Z
// @address 0x5bec98 ?set_disabled_element@c_gui_data@@UAAXJJ@Z
// @address 0x5bec98 ?set_element@c_gui_data@@UAAXJPBXJ@Z
// @address 0x48b098 ?start_animation_at_time@c_gui_widget@@QAAXW4e_animation_state@@K_N@Z
// @address 0x5bec98 ?update@c_gui_data@@UAAXXZ
// @address 0x484f88 ?user_interface_free@@YAXPBX@Z
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x4872e0 ?user_interface_text_parser_unregister@@YAXPAVc_game_tag_parser@@J@Z
// @address 0x489570 ??0c_gui_widget@@QAA@W4e_gui_widget_type@@@Z
// @address 0x5bec98 ?clear_disabled_elements@c_gui_data@@UAAXXZ
// @address 0x4a19f8 ?clear_disabled_elements@c_gui_ordered_data@@UAAXXZ
// @address 0x282ce8 ?content_catalogue_display_device_selection_guide_interface@@YAXW4e_controller_index@@@Z
// @address 0x48bce8 ?delete_all_children@c_gui_widget@@QAAXXZ
// @address 0x5bec98 ?delete_element@c_gui_data@@UAAXJ@Z
// @address 0x5bec98 ?disable_all_elements@c_gui_data@@UAAXJ@Z
// @address 0x4a1a48 ?disable_all_elements@c_gui_ordered_data@@UAAXJ@Z
// @address 0x5bec98 ?dispose@c_gui_data@@UAAXXZ
// @address 0x482250 ?dispose_screen@c_window_manager@@QAAXPAVc_gui_screen_widget@@@Z
// @address 0x5bec98 ?enable_element@c_gui_data@@UAAXJJ@Z
// @address 0x4a1b10 ?enable_element@c_gui_ordered_data@@UAAXJJ@Z
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x4a11d0 ?get_column_names@c_gui_tag_datasource@@UAAXQAJPAJ@Z
// @address 0x4a1198 ?get_current_item_count_internal@c_gui_tag_datasource@@MAAJXZ
// @address 0x48bf08 ?get_deepest_widget_that_can_receive_focus@c_gui_widget@@QAAPAV1@XZ
// @address 0x42d298 ?get_element@c_gui_data@@UAA_NJPAXJ@Z
// @address 0x4a1420 ?get_element@c_gui_tag_datasource@@UAA_NJPAXJ@Z
// @address 0x4898b8 ?get_enabled@c_gui_widget@@UBA_NXZ
// @address 0x42d298 ?get_gui_selected_item@c_gui_data@@UBAPBVc_gui_selected_item@@J@Z
// @address 0x42d298 ?get_integer_value@c_gui_data@@UAA_NJJPAJ@Z
// @address 0x4a14b0 ?get_integer_value@c_gui_tag_datasource@@UAA_NJJPAJ@Z
// @address 0x42d298 ?get_invoked_control@c_gui_data@@UAA_NJPAJ@Z
// @address 0x4a1668 ?get_invoked_control@c_gui_tag_datasource@@UAA_NJPAJ@Z
// @address 0x42d298 ?get_player_appearance@c_gui_data@@UAA_NJPAUs_player_appearance@@@Z
// @address 0x42d298 ?get_qword_value@c_gui_data@@UAA_NJJPA_K@Z
// @address 0x42d298 ?get_real_value@c_gui_data@@UAA_NJJPAM@Z
// @address 0x42d298 ?get_string_id_value@c_gui_data@@UAA_NJJPAJ@Z
// @address 0x4a15d8 ?get_string_id_value@c_gui_tag_datasource@@UAA_NJJPAJ@Z
// @address 0x42d298 ?get_text_value@c_gui_data@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x24d250 ?hs_runtime_script_begin@@YAJFW4e_hs_script_type@@W4e_hs_thread_type@@@Z
// @address 0xabb28 ?hs_thread_delete@@YAXJ_N@Z
// @address 0x1b7030 ?in_use@c_controller_interface@@QBA_NXZ
// @address 0x4c2070 ?initialize@c_gui_data@@UAA_NJ@Z
// @address 0x4a1170 ?initialize@c_gui_tag_datasource@@UAA_NJ@Z
// @address 0x1ebb00 ?length@?$c_static_wchar_string@$0EAA@@@QBAJXZ
// @address 0x4896b0 ?post_initialize@c_gui_widget@@UAAXXZ
// @address 0x489738 ?dispose@c_gui_widget@@UAAXXZ
// @address 0x489bf8 ?add_definition_fields@c_gui_widget@@SAXW4e_gui_widget_type@@PBUs_core_widget_definition@@PAUs_runtime_core_widget_definition@@PBTreal_rectangle2d@@_N@Z
#include <ppcintrinsics.h>
#include <string.h>

class c_game_tag_parser;
class c_gui_list_widget;
class c_user_interface_text_parse_manager;
#include "interface/list_widget_definition.h"
#include "interface/user_interface_clock.h"

#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_screen_codeless_widget.h"

#include "interface/gui_data.h"
#include "interface/c_window_manager.h"
#include "interface/group_widget_definition.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "tag_files/string_ids.h"
#include <new>

class c_gui_widget;
class c_gui_screen_widget;
struct cache_file_tag_instance;

// Screen widget tag (scn3): the core widget fields, then the string list and parent screen references.
struct s_screen_widget_definition
{
    s_core_widget_definition core;
    s_gui_tag_reference string_list;
    s_gui_tag_reference parent_screen;
    long initial_button_key_name;
    s_tag_block datasources, groups, button_keys;
    s_gui_tag_reference sound_overrides;
    char script_name[32];
    short script_index;
};

inline s_runtime_screen_widget_definition::s_runtime_screen_widget_definition() :
        string_list_index(-1),
        initial_button_key_name(-1),
        datasources(),
        groups(),
        button_keys(),
        sound_override_index(-1),
        script_name()
    {
        datasources.count = 0;
        datasources.address = 0;
        groups.count = 0;
        groups.address = 0;
        button_keys.count = 0;
        button_keys.address = 0;
        script_name[0] = 0;
        script_index = -1;
    }


static __forceinline c_window_manager *window_manager_get() { return &g_window_manager; }

class c_allocation_base;



// @address 0x485008 ??0c_gui_screen_widget@@QAA@J@Z
c_gui_screen_widget::c_gui_screen_widget(long name) :
    c_gui_widget(_gui_screen_widget),
    m_screen_index(-1),
    m_creation_time_milliseconds(g_user_interface_globals.milliseconds.peek()),
    m_disposal_time_milliseconds(0xffffffff),
    m_last_focus_change_time_milliseconds(0xffffffff),
    m_focused_widget(0),
    m_suppress_focus(false),
    m_render_in_screenshot(false),
    m_reload_next_frame(false),
    m_responds_to_controller_events(true),
    m_initial_focus_list_name(-1),
    m_initial_focus_element_handle(-1),
    m_initial_focus_column_name(-1),
    m_initial_focus_column_value(-1),
    m_core_definition(),
    m_datasource_count(0),
    m_game_tag_parser_count(0),
    m_render_data(),
    m_codeless_mode(false)
{
    m_core_definition.name = name;
    for (unsigned long datasource_index = 0; datasource_index < 32; datasource_index++)
        m_datasource[datasource_index] = 0;
    for (unsigned long parser_index = 0; parser_index < 20; parser_index++)
        m_game_tag_parsers[parser_index] = 0;
    clear_display_groups();
}

// @address 0x485118 ?reconstruct@c_gui_screen_widget@@UAAXXZ
void c_gui_screen_widget::reconstruct()
{
    memset(&m_animated_state, 0, sizeof(m_animated_state));
    m_focused_widget = 0;
    clear_display_groups();
    dispose_child_screens();
    delete_all_children();
}

// 0x485190

void c_gui_screen_widget::initialize()
{
    long datasource_size = get_render_data_size();
    void *datasource_storage = user_interface_malloc_tracked(datasource_size, 0, 0);
    m_render_data.render_data_buffer = (char *)datasource_storage;
    if (datasource_storage != 0) {
        memset(datasource_storage, 0, datasource_size);
        m_render_data.buffer_size = datasource_size;
    }
    reload_assets();
    c_gui_widget::initialize();
    initialize_datasource();
}

void c_gui_screen_widget::reload_next_frame()
{
    m_reload_next_frame = true;
}

s_runtime_core_widget_definition *c_gui_screen_widget::get_core_definition()
{
    return &m_core_definition;
}

void c_gui_screen_widget::add_definition_fields(const s_screen_widget_definition *definition, bool require_nonzero_bounds)
{
    c_gui_widget::add_definition_fields(m_type, &definition->core, &m_core_definition, 0, require_nonzero_bounds);
    if (definition->initial_button_key_name != 0)
        m_core_definition.initial_button_key_name = definition->initial_button_key_name;
    if (definition->string_list.index != -1 && definition->string_list.index != 0)
        m_core_definition.string_list_index = definition->string_list.index;
    if (definition->datasources.count > 0 && definition->datasources.address != 0)
        m_core_definition.datasources = definition->datasources;
    if (definition->groups.count > 0 && definition->groups.address != 0)
        m_core_definition.groups = definition->groups;
    if (definition->button_keys.count > 0 && definition->button_keys.address != 0)
        m_core_definition.button_keys = definition->button_keys;
    if (definition->sound_overrides.index != -1 && definition->sound_overrides.index != 0)
        m_core_definition.sound_override_index = definition->sound_overrides.index;
    if (definition->script_name[0])
        csstrnzcpy(m_core_definition.script_name, definition->script_name, 32);
    else
        m_core_definition.script_name[0] = 0;
    m_core_definition.script_index = definition->script_index;
}

// @address 0x485478 ?reload_assets@c_gui_screen_widget@@UAAXXZ
void c_gui_screen_widget::reload_assets()
{
    long screen_definition_name = m_core_definition.name;
    long screen_definition_index = c_window_manager::locate_screen_definition_tag_by_name(screen_definition_name);
    delete_all_children();
    s_runtime_screen_widget_definition definition;
    memcpy(&m_core_definition, &definition, sizeof(definition));
    m_core_definition.name = screen_definition_name;
    clear_display_groups();
    if (screen_definition_index != -1)
    {
        const s_screen_widget_definition *screen_definition =
            (const s_screen_widget_definition *)((cache_file_tag_instance *)global_tag_instances)[(unsigned short)screen_definition_index]
                .definition;
        bool require_nonzero_bounds = false;
        if (screen_definition->parent_screen.index != -1)
        {
            const s_screen_widget_definition *parent_definition =
                (const s_screen_widget_definition *)((cache_file_tag_instance *)global_tag_instances)[(unsigned short)screen_definition->parent_screen.index]
                    .definition;
            add_definition_fields(parent_definition, false);
            require_nonzero_bounds = true;
        }
        add_definition_fields(screen_definition, require_nonzero_bounds);
    }
    m_name = m_core_definition.name;
    gui_screen_widget_adjust_for_splitscreen(this);
    create_and_add_child_widgets_from_definition(&m_core_definition);
}


static __forceinline bool c_gui_screen_widget_group_widget_loaded_on_initialization(const s_group_widget_block *group)
{
    bool result = false;
    if ((group->definition.core.flags & 8) != 0)
        result = true;
    else if (group->group_widget_template.index != -1)
        result = (((const s_group_widget_definition *)((cache_file_tag_instance *)global_tag_instances)[(unsigned short)group->group_widget_template.index]
            .definition)->core.flags & 8) != 0;
    return result;
}

// @address 0x485d20 ?dispose_child_screens@c_gui_screen_widget@@QAAXXZ
void c_gui_screen_widget::dispose_child_screens()
{
    bool disposed_screen;
    do
    {
        disposed_screen = false;
        for (c_gui_widget *child = robust_pointer_get_repaired_value(&m_children); child != 0; child = robust_pointer_get_repaired_value(&child->m_next))
        {
            if (child->m_type == _gui_screen_widget)
            {
                window_manager_get()->dispose_screen((c_gui_screen_widget *)child);
                disposed_screen = true;
                break;
            }
        }
    } while (disposed_screen);
}

void c_gui_screen_widget::set_initial_focused_widget()
{
    if (m_initial_focus_list_name != -1)
    {
        c_gui_widget *widget = get_child_list_widget(m_initial_focus_list_name);
        if (widget != 0 && widget->m_type == _gui_list_widget)
        {
            c_gui_list_widget *list = (c_gui_list_widget *)widget;
            if (m_initial_focus_column_name == -1)
            {
                (this)->transfer_focus_to_list(list, m_initial_focus_element_handle, false, false);
            }
            else
            {
                c_gui_data *data = list->get_data();
                if (data != 0)
                {
                    for (long element_handle = data->get_first_element_handle(); element_handle != -1;
                         element_handle = data->get_next_element_handle(element_handle))
                    {
                        long value = -1;
                        if (data->get_string_id_value(element_handle, m_initial_focus_column_name, &value) && value == m_initial_focus_column_value)
                        {
                            (this)
                                ->transfer_focus_to_list(
                                    (c_gui_list_widget *)widget, element_handle, false, false);
                            break;
                        }
                    }
                }
            }
        }
    }
    else
    {
        c_gui_widget *widget = this->get_deepest_widget_that_can_receive_focus();
        this->transfer_focus_without_animations(widget != 0 ? widget : (c_gui_widget *)this, false, false);
    }
}

// @address 0x4860a0 ?process_message@c_gui_screen_widget@@UAA_NPBVc_message@@@Z
bool c_gui_screen_widget::process_message(const c_message *message)
{
    bool result = false;
    e_ui_message_type message_type = message->get_type();
    if (message_type != _ui_controller_input)
    {
        if (message_type == _message_type_dialog_result)
            return handle_dialog_result((const c_dialog_result_message *)message);
    }
    else
    {
        const c_controller_input_message *input_message = (const c_controller_input_message *)message;
        bool handled = handle_controller_input_message(input_message);
        if (handled)
            window_manager_play_input_sound(m_core_definition.sound_override_index, input_message->get_event_type(), input_message->get_component());
        result = handled;
    }
    return result;
}

// @address 0x486130 ?handle_controller_input_message@c_gui_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_widget::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    c_gui_screen_widget *screen = this;
    c_gui_widget *focused = screen->get_focused_widget();
    if (!handled && focused)
    {
        switch (message->get_event_type())
        {
        case _event_type_tab_up: case _event_type_tab_left: case _event_type_tab_down: case _event_type_tab_right:
            {
                bool result = false;
                c_gui_widget *tab = screen->get_focused_widget();
                if (tab)
                    result = tab->handle_tab(message);
                else
                    result = false;
                handled = result;
            }
            break;
        case _event_type_alt_stick_up: case _event_type_alt_stick_left: case _event_type_alt_stick_down: case _event_type_alt_stick_right:
            handled = gui_screen_widget_spin_models_with_dpad(this, message);
            break;
        case _event_type_alt_tab_up: case _event_type_alt_tab_left: case _event_type_alt_tab_down: case _event_type_alt_tab_right:
            {
                bool result = false;
                c_gui_widget *tab = screen->get_focused_widget();
                if (tab)
                    result = tab->handle_alt_tab(message);
                else
                    result = false;
                handled = result;
            }
            break;
        case _event_type_button_press:
            switch (message->get_component())
            {
            case 0:
                handled = focused->handle_widget_selected();
                handled |= screen->handle_focused_widget_selected(message, focused);
                break;
            case 1:
                if (focused != screen)
                    handled = focused->handle_widget_back_out();
                break;
            case 6:
            case 7:
                {
                    bool result = false;
                    c_gui_widget *tab = screen->get_focused_widget();
                    if (tab)
                        result = tab->handle_tab(message);
                    else
                        result = false;
                    handled = result;
                }
                break;
            }
            break;
        }
    }
    if (!handled)
    {
        switch (message->get_event_type())
        {
        case _event_type_button_press:
            switch (message->get_component())
            {
            case 1:
                if (!(screen->get_core_definition()->flags & 8))
                {
                    screen->transition_out_with_transition_type(1, _screen_transition_normal);
                    handled = true;
                }
                break;
            }
            break;
        }
    }
    return handled;
}

// 0x486340

// @address 0x486340 gui_screen_widget_spin_models_with_dpad
extern "C" bool gui_screen_widget_spin_models_with_dpad(c_gui_screen_widget *screen, const c_controller_input_message *message)
{
    c_gui_widget *widget = screen->m_suppress_focus ? 0 : (c_gui_widget *)screen->m_focused_widget;
    for (; widget != 0; widget = robust_pointer_get_repaired_value(&widget->m_parent))
    {
        c_gui_model_widget *model = (c_gui_model_widget *)gui_widget_find_child_by_name(widget, 2);
        if (model != 0)
        {
            float x = 0.0f;
            float y = 0.0f;
            switch (message->get_event_type())
            {
            case 5: y = -1.0f; break;
            case 6: x = -1.0f; break;
            case 7: y = 1.0f; break;
            case 8: x = 1.0f; break;
            }
            model->handle_camera_input(x, y);
        }
    }
    return false;
}

// @address 0x486448 ?handle_dialog_result@c_gui_screen_widget@@UAA_NPBVc_dialog_result_message@@@Z
bool c_gui_screen_widget::handle_dialog_result(const c_dialog_result_message *message)
{
    bool result = false;
    if (message->m_dialog_name == _start_menu_string_device_selection) {
        if (message->m_dialog_result == 0) {
            content_catalogue_display_device_selection_guide_interface(message->m_controller);
        }
        result = true;
    }
    return result;
}

// @address 0x486498 ?handle_focused_widget_selected@c_gui_screen_widget@@UAA_NPBVc_controller_input_message@@PAVc_gui_widget@@@Z

bool c_gui_screen_widget::handle_focused_widget_selected(const c_controller_input_message *message, c_gui_widget *widget)
{
    bool result = false;
    if (message != 0 && message->get_event_type() == _event_type_button_press && message->get_component() == 0)
    {
        c_gui_list_widget *list = gui_widget_get_parent_list_widget(widget);
        c_gui_list_item_widget *list_item = widget->m_type == _gui_list_item_widget ? (c_gui_list_item_widget *)widget : 0;
        if (list != 0 && list_item != 0)
        {
            if (list_item->get_enabled())
            {
                c_gui_data *data = (list)->get_data();
                if (data != 0)
                {
                    result = handle_list_item_chosen(message, list->get_name(), list_item, data);
                }
            }
            else
            {
                ::user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
            }
        }
    }
    return result;
}

void c_gui_screen_widget::submenu_invoked(c_gui_list_widget *submenu_widget)
{
}

e_window_index c_gui_screen_widget::get_render_window()
{
    return window_manager_get()->get_render_window_for_screen(this);
}

c_gui_data *c_gui_screen_widget::get_data(long name, long *datasource_index)
{
    return find_data(name, datasource_index);
}

// 0x486668
// @address 0x486668 ?transfer_focus_without_animations@c_gui_screen_widget@@QAAXPAVc_gui_widget@@_N1@Z
void c_gui_screen_widget::transfer_focus_without_animations(c_gui_widget *widget, bool animate, bool restart)
{
    if (!widget->can_receive_focus())
        return;
    c_gui_widget *previous = (c_gui_widget *)m_focused_widget;
    if (animate)
        widget->start_animation_at_time(_animation_state_next_control_received_focus, 0xffffffff, true);
    if (previous != 0 && restart)
        previous->start_animation_at_time(_animation_state_next_control_lost_focus, 0xffffffff, true);
    m_focused_widget = widget;
}

// 0x486700

static __forceinline c_gui_widget *c_gui_screen_widget_next_list_item(c_gui_widget *item)
{
    c_gui_widget *next = gui_widget_get_next_sibling_of_type(item, 5);
    if (next != 0 && next->get_element_handle() == -1)
        next = 0;
    return next;
}

// @address 0x486700 ?transfer_focus_to_list@c_gui_screen_widget@@QAAXPAVc_gui_list_widget@@J_N1@Z
void c_gui_screen_widget::transfer_focus_to_list(class c_gui_list_widget * list, long element_handle, bool animate, bool restart)
{
    c_gui_data *data = (list)->get_data();
    if (data == 0)
        return;
    bool found = false;
    long handle;
    for (handle = data->get_first_element_handle(); handle != -1; handle = data->get_next_element_handle(handle))
    {
        if (handle == element_handle)
        {
            found = true;
            break;
        }
    }
    if (!found)
        element_handle = data->get_first_element_handle();
    if (element_handle == -1)
        return;
    long index = 0;
    for (handle = data->get_first_element_handle(); handle != -1 && handle != element_handle; handle = data->get_next_element_handle(handle))
        index++;
    list->set_focused_item_index(index, false);
    long focused = (list)->get_focused_element_handle();
    for (c_gui_widget *item = gui_widget_find_child_by_name((c_gui_widget *)list, 5); item != 0; item = c_gui_screen_widget_next_list_item(item))
    {
        if (focused == item->get_element_handle())
        {
            this->transfer_focus_without_animations(item, animate, restart);
            return;
        }
    }
}

bool c_gui_screen_widget::transitioning_in()
{
    bool result = (m_animated_state.animation_state_flags & (1ui64 << 7)) != 0
        || (m_animated_state.animation_state_flags & (1ui64 << 9)) != 0
        || (m_animated_state.animation_state_flags & (1ui64 << 10)) != 0
        || (m_animated_state.animation_state_flags & (1ui64 << 11)) != 0
        || (m_animated_state.animation_state_flags & (1ui64 << 32)) != 0
        || (m_animated_state.animation_state_flags & (1ui64 << 34)) != 0;
    return result;
}

bool c_gui_screen_widget::transitioning_out()
{
    bool result = (m_animated_state.animation_state_flags & (1ui64 << 6)) != 0
        || (m_animated_state.animation_state_flags & (1ui64 << 8)) != 0
        || (m_animated_state.animation_state_flags & (1ui64 << 12)) != 0
        || (m_animated_state.animation_state_flags & (1ui64 << 13)) != 0
        || (m_animated_state.animation_state_flags & (1ui64 << 33)) != 0
        || (m_animated_state.animation_state_flags & (1ui64 << 35)) != 0;
    return result;
}


// @address 0x486988 ?transition_out_with_transition_type@c_gui_screen_widget@@UAAXJW4e_screen_transition_type@@@Z
void c_gui_screen_widget::transition_out_with_transition_type(long transition_out, e_screen_transition_type transition_type)
{
    window_manager_close_screen_animated(window_manager_get(), this, transition_type);
}

bool c_gui_screen_widget::get_string_by_string_id(long string_id, c_static_wchar_string<1024> *string)
{
    bool result = false;
    if (string_id != 0 && string_id != -1 && m_core_definition.string_list_index != -1)
    {
        text_group_get_string(m_core_definition.string_list_index, string_id, string);
        result = true;
    }
    return result;
}

// @address 0x4869e8 ?set_text_widget_string_id@c_gui_screen_widget@@QAAXJJ@Z
void c_gui_screen_widget::set_text_widget_string_id(long name, long value)
{
    c_gui_text_widget *text_widget = get_child_text_widget(name);
    if (text_widget != 0)
        text_widget->set_text_from_string_id(this, value);
}

// @address 0x486a48 gui_screen_widget_set_text_widget_text
extern "C" void gui_screen_widget_set_text_widget_text(c_gui_screen_widget *screen, long name, const wchar_t *text)
{
    c_gui_text_widget *text_widget = screen->get_child_text_widget(name);
    if (text_widget != 0)
        text_widget->set_text(text);
}

// @address 0x486a98 ?set_text_widget_text_from_datasource@c_gui_screen_widget@@QAAXJJJJ@Z
void c_gui_screen_widget::set_text_widget_text_from_datasource(long name, long datasource_name, long element_handle, long value_name)
{
    c_gui_text_widget *text_widget = get_child_text_widget(0x29e);
    c_gui_data *data = find_data(datasource_name, 0);
    if (data != 0 && text_widget != 0)
    {
        c_static_wchar_string<1024> string;
        if (data->get_text_value(element_handle, value_name, &string) && string.length() > 0)
            text_widget->set_text(string.get_string());
        else
            text_widget->set_text(L" ");
    }
}

// @address 0x486ba0 ?set_text_widget_string_id_from_datasource@c_gui_screen_widget@@QAAXJJJJ@Z
void c_gui_screen_widget::set_text_widget_string_id_from_datasource(long name, long datasource_name, long element_handle, long value_name)
{
    c_gui_text_widget *text_widget = get_child_text_widget(name);
    long string_id;
    c_gui_data *data = find_data(datasource_name, 0);
    if (data != 0 && text_widget != 0)
    {
        if (data->get_string_id_value(element_handle, value_name, &string_id) && string_id != -1)
            text_widget->set_text_from_string_id(this, string_id);
        else
            text_widget->set_text(L" ");
    }
}

// Rebuilds the datasource children from the widget's tag entries.
void c_gui_screen_widget::initialize_datasource()
{
    dispose_datasource();
    for (long i = 0; i < m_core_definition.datasources.count; i++)
    {
        const s_tag_reference &datasource = ((s_tag_reference *)m_core_definition.datasources.address)[i];
        long handle = datasource.index;
        if (handle == -1)
            continue;
        long *definition = (long *)((cache_file_tag_instance *)global_tag_instances)[(unsigned short)handle].definition;
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_tag_datasource), 0, 0);
        if (memory != 0)
        {
            c_gui_tag_datasource *child = new (memory) c_gui_tag_datasource(handle);
            long value = (*definition == -1 || *definition == 0) ? m_name : *definition;
            if (child->initialize(value))
            {
                m_datasource[m_datasource_count] = child;
                m_datasource_count++;
            }
            else
            {
                child->~c_gui_tag_datasource();
                user_interface_free(child);
            }
        }
    }
}
// Disposes, destroys and frees every child, then empties the list.
// @address 0x4853e8 ?dispose_datasource@c_gui_screen_widget@@AAAXXZ
void c_gui_screen_widget::dispose_datasource()
{
  c_gui_screen_widget *owner = this;
  for (long i = 0; i < owner->m_datasource_count; i++) {
    if (owner->m_datasource[i] != 0) {
      owner->m_datasource[i]->dispose();
      c_gui_data *child = owner->m_datasource[i];
      child->~c_gui_data();
      user_interface_free(child);
      owner->m_datasource[i] = 0;
    }
  }
  owner->m_datasource_count = 0;
}

// @address 0x485350 ?dispose@c_gui_screen_widget@@UAAXXZ
void c_gui_screen_widget::dispose()
{
    this->c_gui_widget::dispose();
    if (m_render_data.render_data_buffer != 0)
        user_interface_free(m_render_data.render_data_buffer);
    dispose_datasource();
    for (long i = 0; i < m_game_tag_parser_count; ++i)
    {
        user_interface_text_parser_unregister(m_game_tag_parsers[i], m_screen_index);
        c_game_tag_parser *parser = m_game_tag_parsers[i];
        parser->~c_game_tag_parser();
        user_interface_free(parser);
        m_game_tag_parsers[i] = 0;
    }
    m_game_tag_parser_count = 0;
}

// @address 0x485560 ?post_initialize@c_gui_screen_widget@@UAAXXZ
void c_gui_screen_widget::post_initialize()
{
    set_initial_focused_widget();
    long script_index = m_core_definition.script_index;
    if (script_index != -1)
    {
        c_window_manager *window_manager = window_manager_get();
        if (window_manager->m_script_thread_index != -1)
        {
            if (datum_try_and_get_inline(g_hs_thread_tracking_data, window_manager->m_script_thread_index))
                hs_thread_delete(window_manager->m_script_thread_index, false);
            window_manager->m_script_thread_index = -1;
        }
        // match: 0x485560: the script type is read sign-extended; hs.h stores it unsigned.
        window_manager->m_script_thread_index = hs_runtime_script_begin(script_index,
            (e_hs_script_type)(short)((s_hs_script_definition *)global_scenario->scripts.address)[script_index].type,
            _hs_thread_type_script);
    }
    this->c_gui_widget::post_initialize();
}

__forceinline c_gui_group_widget *c_gui_screen_widget::load_display_group_inline(
    const s_runtime_screen_widget_definition *definition, e_display_group_type group_type, long group)
{
    c_gui_group_widget *result = 0;
    do
    {
        if (group_type == _display_group_type_group)
        {
            if (group < 0 || group >= definition->groups.count)
                break;
            s_group_widget_block *block = &((s_group_widget_block *)definition->groups.address)[group];
            c_gui_group_widget *group_widget = create_group_widget(&block->definition);
            if (group_widget != 0)
            {
                gui_widget_append_child((c_gui_widget *)this, (c_gui_widget *)group_widget);
                group_widget->initialize(block, true);
            }
            result = group_widget;
        }
        else if (group_type == _display_group_type_button_key)
        {
            if (group < 0 || group >= definition->button_keys.count)
                break;
            s_button_key_block *block = &((s_button_key_block *)definition->button_keys.address)[group];
            const s_button_key_definition *tag_definition =
                (const s_button_key_definition *)((cache_file_tag_instance *)global_tag_instances)[(unsigned short)block->button_key_template.index].definition;
            c_gui_group_widget *button_key_widget = 0;
            if (tag_definition != 0)
            {
                button_key_widget = create_button_key_widget(tag_definition);
                if (button_key_widget != 0)
                {
                    gui_widget_append_child((c_gui_widget *)this, (c_gui_widget *)button_key_widget);
                    ((c_gui_button_key_widget *)button_key_widget)->initialize(block, true);
                }
            }
            result = button_key_widget;
        }
    } while (0);
    return result;
}

// @address 0x485640 ?load_display_group@c_gui_screen_widget@@AAAPAVc_gui_group_widget@@PBUs_runtime_screen_widget_definition@@W4e_display_group_type@1@J@Z
c_gui_group_widget *c_gui_screen_widget::load_display_group(const s_runtime_screen_widget_definition *definition, e_display_group_type group_type, long group)
{
    return load_display_group_inline(definition, group_type, group);
}

// 0x485758

static __forceinline long c_gui_screen_widget_find_display_group_index(c_gui_screen_widget *screen, long type, long name)
{
    long result = -1;
    if (type == 0)
    {
        for (long i = 0; i < screen->m_core_definition.groups.count; i++)
        {
            s_group_widget_block *block = &((s_group_widget_block *)screen->m_core_definition.groups.address)[i];
            if (block->definition.core.name == name)
            {
                result = i;
                break;
            }
        }
    }
    else if (type == 1)
    {
        for (long i = 0; i < screen->m_core_definition.button_keys.count; i++)
        {
            s_tag_reference *reference = &(
                (s_tag_reference *)screen->m_core_definition.button_keys.address)[i];
            if (reference->index != -1)
            {
                s_core_widget_definition *definition = (s_core_widget_definition *)((
                    cache_file_tag_instance *)global_tag_instances)[(unsigned short)reference->index]
                                                                              .definition;
                if (definition->name == name)
                {
                    result = i;
                    break;
                }
            }
        }
    }
    return result;
}

// @address 0x485758 gui_screen_widget_set_display_group
extern "C" void gui_screen_widget_set_display_group(c_gui_screen_widget *screen, long type, long name)
{
    long index = c_gui_screen_widget_find_display_group_index(screen, type, name);
    if (index == -1)
        return;
    if (index != screen->m_display_group_types[type])
    {
        screen->dispose_display_group(screen->m_previous_display_group_widgets[type]);
        screen->m_previous_display_group_widgets[type] = screen->m_current_display_group_widgets[type];
        screen->m_current_display_group_widgets[type] = 0;
        if (screen->m_previous_display_group_widgets[type])
            screen->m_previous_display_group_widgets[type]->start_animation_at_time(_animation_state_previous_control_lost_focus, 0xffffffff, true);
        screen->m_current_display_group_widgets[type] = screen->load_display_group(
            &screen->m_core_definition, (c_gui_screen_widget::e_display_group_type)type, index);
        if (screen->m_current_display_group_widgets[type])
        {
            screen->m_display_group_types[type] = index;
            screen->m_current_display_group_widgets[type]->post_initialize();
            screen->m_current_display_group_widgets[type]->start_animation_at_time(_animation_state_previous_control_received_focus, 0xffffffff, true);
        }
        screen->m_display_group_types[type] = index;
    }
}

// @address 0x4858d8 ?dispose_display_group@c_gui_screen_widget@@AAAXPAVc_gui_group_widget@@@Z
void c_gui_screen_widget::dispose_display_group(c_gui_group_widget *group)
{
    if (group == 0)
        return;
    bool within_focus_chain = group->within_focus_chain();
    for (long group_index = 0; group_index < k_display_group_type_count; ++group_index)
    {
        if (group == m_previous_display_group_widgets[group_index])
            m_previous_display_group_widgets[group_index] = 0;
        if (group == m_current_display_group_widgets[group_index])
        {
            m_current_display_group_widgets[group_index] = 0;
            m_display_group_types[group_index] = -1;
        }
    }
    gui_widget_remove_child((c_gui_widget *)this, (c_gui_widget *)group);
    group->dispose();
    group->~c_gui_group_widget();
    user_interface_free(group);
    if (within_focus_chain)
    {
        m_focused_widget = 0;
        set_initial_focused_widget();
    }
}

// @address 0x485b10 ?create_and_add_child_widgets_from_definition@c_gui_screen_widget@@AAAXPAUs_runtime_screen_widget_definition@@@Z
void c_gui_screen_widget::create_and_add_child_widgets_from_definition(s_runtime_screen_widget_definition *definition)
{
    for (long group_index = 0; group_index < definition->groups.count; ++group_index)
    {
        s_group_widget_block *block = &((s_group_widget_block *)definition->groups.address)[group_index];
        if (!c_gui_screen_widget_group_widget_loaded_on_initialization(block))
            load_display_group_inline(definition, _display_group_type_group, group_index);
    }
    for (long button_index = 0; button_index < definition->button_keys.count; ++button_index)
    {
        s_button_key_block *block = &((s_button_key_block *)definition->button_keys.address)[button_index];
        if (block->button_key_template.index == -1)
            continue;
        const s_core_widget_definition *tag_definition = (const s_core_widget_definition *)((cache_file_tag_instance *)global_tag_instances)
            [(unsigned short)block->button_key_template.index].definition;
        if (!(((tag_definition->flags & 8) != 0 && definition->initial_button_key_name == 0)
              || tag_definition->name == definition->initial_button_key_name))
            continue;
        c_gui_group_widget *button_key_widget = load_display_group_inline(definition, _display_group_type_button_key, button_index);
        m_current_display_group_widgets[_display_group_type_button_key] = button_key_widget;
        m_display_group_types[_display_group_type_button_key] = button_index;
        if (button_key_widget != 0)
            definition->initial_button_key_name = button_key_widget->m_name;
        break;
    }
}

// @address 0x486030 ?swap_display_group_block@c_gui_screen_widget@@AAAXXZ
void c_gui_screen_widget::swap_display_group_block()
{
    for (long i = 0; i < k_display_group_type_count; ++i)
    {
        c_gui_group_widget *previous = m_previous_display_group_widgets[i];
        if (previous != 0 && previous->get_dispose_as_display_group())
            dispose_display_group(previous);
        c_gui_group_widget *current = m_current_display_group_widgets[i];
        if (current != 0 && current->get_dispose_as_display_group())
            dispose_display_group(current);
    }
}

long c_gui_screen_widget::get_constants_datasource_integer(long name)
{
    long result = -1;
    c_gui_data *data = find_data(0x10018, 0);
    if (data)
    {
        for (long element_handle = data->get_first_element_handle(); element_handle != -1;
             element_handle = data->get_next_element_handle(element_handle))
        {
            long element_name = -1;
            if (data->get_string_id_value(element_handle, _string_id_name, &element_name) && element_name == name)
            {
                long value = -1;
                if (data->get_integer_value(element_handle, _string_id_value, &value))
                {
                    result = value;
                    break;
                }
            }
        }
    }
    return result;
}

// 0x486dd8
// @address 0x50508 ??_7c_gui_data_array_test@@6B@
// @address 0x4a0d38 ??_Gc_gui_data_array_test@@UAAPAXI@Z
// @address 0x4a8578 ?get_column_names@c_gui_data_array_test@@UAAXQAJPAJ@Z

// @address 0x486dd8 ?initialize_datasource@c_gui_screen_codeless_widget@@UAAXXZ
void c_gui_screen_codeless_widget::initialize_datasource()
{
    c_gui_screen_codeless_widget *owner = this;
    ((c_gui_screen_widget *)owner)->c_gui_screen_widget::initialize_datasource();
    if (owner->m_datasource_count == 0) {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_data_array_test), 0, 0);
        c_gui_data_array_test *data = memory ? new (memory) c_gui_data_array_test() : 0;
        if (data) {
            if (data->initialize(owner->m_name)) {
                for (long i = 0; i < data->get_maximum_item_count(); ++i) {
                    long handle = data->add_element();
                    struct { short first; short second; } value;
                    value.second = 12345;
                    data->set_element(handle, &value, 4);
                }
                owner->m_datasource[owner->m_datasource_count] = (c_gui_data *)data;
                owner->m_datasource_count++;
            } else {
                data->~c_gui_data_array_test();
                user_interface_free(data);
            }
        }
    }
}

void c_gui_screen_widget::add_game_tag_parser(c_game_tag_parser *parser)
{
    if (parser != 0 && (unsigned long)m_game_tag_parser_count < 20)
    {
        g_parse_manager.register_parser(parser, m_screen_index);
        m_game_tag_parsers[m_game_tag_parser_count] = parser;
        m_game_tag_parser_count++;
    }
}

// @address 0x52d88 ??_7c_settings_camera_sidebar_items_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_settings_camera_sidebar_items_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_settings_camera_sidebar_items_datasource@@UAAPAXI@Z

// @address 0x4b92c0 ?get_column_names@c_settings_camera_sidebar_items_datasource@@UAAXQAJPAJ@Z
void c_settings_camera_sidebar_items_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _string_id_value;
    ++*column_count;
}

// @address 0x4bdfc0 ?get_string_id_value@c_settings_camera_sidebar_items_datasource@@UAA_NJJPAJ@Z
bool c_settings_camera_sidebar_items_datasource::get_string_id_value(long index, long id, long *out)
{
    if (index != -1 && (id == _string_id_value || id == _string_id_description)) {
        long found = -1;
        if (m_base_datasource->get_string_id_value(index, _string_id_name, &found)) {
            c_gui_screen_widget *list = screen;
            int i;
            int count = list->m_datasource_count;
            for (i = 0; i < count; i++) {
                if (list->m_datasource[i]->m_name == found) {
                    c_gui_data *item = list->m_datasource[i];
                    if (item) {
                        int expect = -1;
                        int k;
                        switch (found) {
                        case _start_menu_string_camera_horizontal_inversion: expect = profile->camera.unknown_setting_20; break;
                        case _start_menu_string_camera_panning: expect = profile->camera.unknown_setting_24; break;
                        case _start_menu_string_camera_look_sensitivity: expect = profile->camera.unknown_setting_28; break;
                        case _start_menu_string_camera_speed: expect = profile->camera.unknown_setting_2c; break;
                        case _start_menu_string_camera_thrust: expect = profile->camera.unknown_setting_30; break;
                        }
                        k = item->get_first_element_handle();
                        while (k != -1) {
                            long value = -1;
                            if (item->get_integer_value(k, _string_id_value, &value) && value == expect) {
                                long result = -1;
                                if (item->get_string_id_value(k, id == _string_id_value ? _string_id_name : _string_id_description, &result))
                                    *out = result;
                                return 1;
                            }
                            k = item->get_next_element_handle(k);
                        }
                    }
                    return 1;
                }
            }
        }
        return 1;
    }
    return m_base_datasource->get_string_id_value(index, id, out);
}

// @address 0x4be280 ?initialize_datasource@c_start_menu_settings_camera@@UAAXXZ
void c_start_menu_settings_camera::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    unsigned int controller = get_any_responding_controller();
    if (controller <= 3 && ((c_controller_interface *)&controller_globals().controllers[controller])->in_use()) {
        long index = -1;
        c_gui_data *widget = get_data(_start_menu_string_start_menu_game_items, &index);
        if (widget != 0) {
            void *memory = user_interface_malloc_tracked(sizeof(c_settings_camera_sidebar_items_datasource), 0, 0);
            if (memory != 0) {
                c_settings_camera_sidebar_items_datasource *datasource = new (memory) c_settings_camera_sidebar_items_datasource(
                    widget, controller_globals().controllers[controller].get_player_profile_interface(), this);
                if (datasource->initialize()) {
                    add_datasource(datasource);
                    return;
                }
                datasource->~c_settings_camera_sidebar_items_datasource();
                user_interface_free(datasource);
            }
        }
    }
}


// @address 0x4be958 ?get_string_id_value@c_alternate_controller_datasource@@UAA_NJJPAJ@Z
bool c_alternate_controller_datasource::get_string_id_value(long index, long id, long *out)
{
    if (index == -1 || (id != _string_id_value && id != _string_id_description))
        return m_base_datasource->get_string_id_value(index, id, out);
    long found = -1;
    if (!m_base_datasource->get_string_id_value(index, _string_id_name, &found))
        return true;
    c_gui_screen_widget *screen = m_screen;
    for (long i = 0; i < screen->m_datasource_count; i++)
    {
        if (screen->m_datasource[i]->get_name() == found)
        {
            c_gui_data *item = screen->m_datasource[i];
            if (item != 0)
            {
                long expect = -1;
                switch (found)
                {
                case _start_menu_string_display_subtitles: expect = global_preferences_get_subtitle_setting(m_profile); break;
                case _start_menu_string_display_brightness: expect = global_preferences_get_brightness_setting(); break;
                }
                for (long element = item->get_first_element_handle(); element != -1; element = item->get_next_element_handle(element))
                {
                    long value = -1;
                    if (item->get_integer_value(element, _string_id_value, &value) && value == expect)
                    {
                        long result = -1;
                        if (item->get_string_id_value(element, id == _string_id_value ? _string_id_name : _string_id_description, &result))
                            *out = result;
                        return true;
                    }
                }
            }
            return true;
        }
    }
    return true;
}

// @address 0x52df8 k_focused_item_tag_text
// @rdata 0x82052df8..0x82052e34
extern "C" {
const wchar_t k_focused_item_tag_text[] = {
    '<', 'd', 'i', 's', 'p', 'l', 'a', 'y', '_', 's', 'e', 't', 't', 'i', 'n', 'g', '_',
    'd', 'e', 's', 'c', 'r', 'i', 'p', 't', 'i', 'o', 'n', 0, 0
};
}
// @rdata-end

// @address 0x4beb40 ?initialize@c_start_menu_settings_display@@UAAXXZ
void c_start_menu_settings_display::initialize()
{
    c_gui_screen_widget::initialize();
    c_game_tag_parser *result_parser;
    void *memory = user_interface_malloc_tracked(sizeof(c_magic_string_game_tag_parser), 0, 0);
    if (memory != 0) {
        result_parser = new (memory) c_magic_string_game_tag_parser(
            k_focused_item_tag_text, this, gui_screen_widget_get_focused_item_text);
    } else {
        result_parser = 0;
    }
    add_game_tag_parser(result_parser);
}




// @address 0x4bf0e8 gui_screen_widget_get_focused_item_text
extern "C" bool gui_screen_widget_get_focused_item_text(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_widget *self = (c_gui_screen_widget *)context;
    long value;
    bool success = false;
    c_gui_widget *other;
    if (!self->m_suppress_focus)
    {
        c_gui_widget *candidate = *(c_gui_widget **)&self->m_focused_widget;
        if (candidate)
            other = gui_widget_get_parent_list_widget(candidate);
        else
            other = 0;
    }
    else
    {
        other = 0;
    }
    c_gui_data *data;
    c_gui_list_widget *list = self->get_child_list_widget(_start_menu_string_start_menu_items);
    if (list != 0)
        data = (list)->get_data();
    else
        data = 0;
    do
    {
        if (other == list && data != 0)
        {
            if (!data->get_string_id_value((list)->get_focused_element_handle(), 0x29e, &value))
                break;
        }
        else
        {
            c_gui_data *focused_data = 0;
            long handle = -1;
            c_gui_list_widget *primary = self->get_child_list_widget(_start_menu_string_display_subtitles);
            c_gui_list_widget *secondary = self->get_child_list_widget(_start_menu_string_display_brightness);
            if (primary != 0 && primary->within_focus_chain())
            {
                focused_data = (primary)->get_data();
                handle = (primary)->get_focused_element_handle();
            }
            else if (secondary != 0 && secondary->within_focus_chain())
            {
                focused_data = (secondary)->get_data();
                handle = (secondary)->get_focused_element_handle();
            }
            if (focused_data == 0 || handle == -1)
                break;
            if (!focused_data->get_string_id_value(handle, 0x29e, &value))
                break;
        }
        if (value != -1)
        {
            c_static_wchar_string<1024> text;
            if (self->get_string_by_string_id(value, &text))
            {
                unsigned int length = buffer_length;
                if (length > 0x400)
                    length = 0x400;
                ustrnzcpy(buffer, text.get_string(), length);
                success = true;
            }
        }
    } while (0);
    return success;
}
// @address 0x52f08 ??_7c_alternate_controller_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_alternate_controller_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_alternate_controller_datasource@@UAAPAXI@Z

// @address 0x4b92c0 ?get_column_names@c_alternate_controller_datasource@@UAAXQAJPAJ@Z
void c_alternate_controller_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _string_id_value;
    ++*column_count;
}

// @address 0x4bebc8 ?initialize_datasource@c_start_menu_settings_display@@UAAXXZ
void c_start_menu_settings_display::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    unsigned int controller = get_any_responding_controller();
    if (controller <= 3 && ((c_controller_interface *)&controller_globals().controllers[controller])->in_use()) {
        long index = -1;
        c_gui_data *widget = get_data(_start_menu_string_start_menu_game_items, &index);
        if (widget != 0) {
            void *memory = user_interface_malloc_tracked(sizeof(c_alternate_controller_datasource), 0, 0);
            if (memory != 0) {
                c_alternate_controller_datasource *datasource = new (memory) c_alternate_controller_datasource(
                    widget, controller_globals().controllers[controller].get_player_profile_interface(), this);
                if (datasource->initialize()) {
                    add_datasource(datasource);
                    return;
                }
                datasource->~c_alternate_controller_datasource();
                user_interface_free(datasource);
            }
        }
    }
}


// @address 0x4bece0 ?dispose@c_start_menu_settings_display@@UAAXXZ
void c_start_menu_settings_display::dispose()
{
    c_gui_screen_widget::dispose();
    update_pending_value();
}

static __forceinline void c_gui_screen_widget_offset_real_rectangle2d(real_rectangle2d *rectangle, float dx, float dy)
{
    rectangle->x0 += dx;
    rectangle->y0 += dy;
    rectangle->x1 += dx;
    rectangle->y1 += dy;
}

// @address 0x486f80 gui_screen_widget_adjust_for_splitscreen
extern "C" void gui_screen_widget_adjust_for_splitscreen(c_gui_screen_widget *screen)
{
    if (robust_pointer_get_repaired_value(&screen->m_parent) != 0)
        return;
    e_window_index window = screen->get_render_window();
    if (window < 0 || window > 3)
        return;
    long player_count = players_get_active_user_count();
    if (player_count <= 1)
        return;
    rectangle2d inset_render_bounds;
    rectangle2d render_bounds;
    rasterizer_main_get_display_and_render_bounds(0, 0, &render_bounds, &inset_render_bounds);
    long width = (render_bounds.x1 - render_bounds.x0) - (inset_render_bounds.x1 - inset_render_bounds.x0);
    long height = (render_bounds.y1 - render_bounds.y0) - (inset_render_bounds.y1 - inset_render_bounds.y0);
    float dy = (float)height * 0.25f;
    float dx = (float)width * 0.25f;
    if ((long)window >= (player_count >> 1))
        dy = -dy;
    float fx = 0.0f;
    switch (player_count)
    {
    case 3:
        switch (window)
        {
        case 1:
            fx = dx;
            break;
        case 2:
            fx = -dx;
            break;
        }
        break;
    case 4:
        switch (window)
        {
        case 0:
        case 2:
            fx = dx;
            break;
        case 1:
        case 3:
            fx = -dx;
            break;
        }
        break;
    }
    c_gui_screen_widget_offset_real_rectangle2d(&screen->m_core_definition.current_bounds, fx, dy);
    c_gui_screen_widget_offset_real_rectangle2d(&screen->m_core_definition.authored_bounds, fx, dy);
}



// @address 0x487148 gui_screen_widget_find_list_item_by_value
extern "C" c_gui_widget *gui_screen_widget_find_list_item_by_value(c_gui_widget *widget)
{
    c_gui_widget *result = 0;
    long value;
    c_gui_list_widget *list = widget->get_child_list_widget(0x102a7);
    if (list != 0)
    {
        c_gui_data *data = (list)->get_data();
        if (data != 0)
        {
            c_gui_widget *item = gui_widget_find_child_by_name(list, 5);
            if (item != 0)
            {
                do
                {
                    if (data->get_string_id_value(item->get_element_handle(), 0x29c, &value) && value == 0x102b3)
                    {
                        result = item;
                        break;
                    }
                    item = gui_widget_get_next_list_item(item, true);
                } while (item != 0);
            }
        }
    }
    return result;
}
