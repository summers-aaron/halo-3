// interfacelibxbox/start_menu_settings_appearance_colors.obj: from the records 0x4bc628 0x4bc680 0x4bc898 0x4bc8d0 0x4bc920 0x4bcbe0 0x4bcf28 0x4bd1d8 0x4bd348 0x4bd3f0 0x4bd400 0x4bd410 0x4bd420 0x4bd430 0x4bd440 0x4bd450 0x4bd5f8 0x4be1f8 0x4be398 0x4be5a8 0x4be670
// @flags /TP /O2 /Oc
#include "interface/gui_screen_widget_legacy.h"
#include "memory/static_arrays.h"
#include "cseries/maximum.h"
#include "cseries/minimum.h"
#include "interface/gui_screen_widget_text.h"
#include "interface/gui_widget_tree.h"
#include "interface/c_player_profile.h"
#include <stddef.h>
#include "shell/shell.h"
#include "game/player_types.h"
#include "interface/gui_widget_types.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "interface/gui_screens/start_menu/panes/settings_appearance_colors/start_menu_settings_appearance_colors.h"
#include "interface/gui_screens/start_menu/panes/settings_appearance_emblem/start_menu_settings_appearance_emblem.h"
#include "interface/gui_screens/start_menu/panes/settings_camera/start_menu_settings_camera.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_allocation.h"

#include "interface/controller_types.h"
#include "interface/controller_inline.h"


extern "C" {
// @address 0x52c80 k_appearance_focused_item_tag
// @rdata 0x82052c80..0x82052cb8
const wchar_t k_appearance_focused_item_tag[] = L"<camera_setting_description";
// @rdata-end
}
// @address 0x52a80 ??_7c_color_swatch_focus_list_item_widget@@6B@


#include "interface/gui_data.h"
#include "interface/list_widget_definition.h"

#include "interface/gui_widget.h"
#include "interface/c_gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/gui_model_widget_types.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "tag_files/string_ids.h"

class c_color_swatch_focus_list_item_widget : public c_gui_list_item_widget
{
public:
    __forceinline c_color_swatch_focus_list_item_widget() : c_gui_list_item_widget() {}
protected:
    virtual bool within_focus_chain();
};

void c_start_menu_settings_appearance_colors::initialize()
{
    gui_screen_widget_initialize(this);
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<color-armor1", this, start_menu_settings_appearance_colors_get_primary));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<color-armor2", this, start_menu_settings_appearance_colors_get_secondary));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<color-armor3", this, start_menu_settings_appearance_colors_get_tertiary));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<color-emblem1", this, start_menu_settings_appearance_colors_get_emblem_primary));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<color-emblem2", this, start_menu_settings_appearance_colors_get_emblem_secondary));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<color-emblem3", this, start_menu_settings_appearance_colors_get_emblem_background));
}

// 0x4bc898
// @address 0x4bc898 ?post_initialize@c_start_menu_settings_appearance_colors@@UAAXXZ
void c_start_menu_settings_appearance_colors::post_initialize()
{
    set_color_values_from_profile();
    c_gui_screen_widget::post_initialize();
}


// 0x4bcf28
// @address 0x4bcf28 ?update_color_picker_enabled_state@c_start_menu_settings_appearance_colors@@AAAXXZ
void c_start_menu_settings_appearance_colors::update_color_picker_enabled_state()
{
    c_gui_widget *list = get_child_widget(_gui_list_widget, 0x1012f);
    c_gui_widget *parent_list = get_focused_widget()
        ? gui_widget_get_parent_list_widget(get_focused_widget())
        : 0;
    c_gui_widget *parent_item = get_focused_widget()
        ? gui_widget_get_parent_list_item_widget(get_focused_widget())
        : 0;
    c_gui_widget *group = get_child_widget(_gui_group_widget, _start_menu_string_color_swatch_focus);
    if (list && parent_list && parent_item && group) {
        list->set_enabled(true);
        group->set_visible(!list->within_focus_chain());
        for (c_gui_widget *item = gui_widget_find_child_by_name(list, _gui_list_item_widget); item;
            item = gui_widget_get_next_list_item(item, true)) {
            c_gui_widget *item_group = get_child_widget(_gui_group_widget, item->get_name());
            c_gui_widget *item_list = get_child_widget(_gui_list_widget, item->get_name());
            bool enabled = true;
            if (parent_list == list) {
                if (parent_item != item) item->set_enabled(false);
                enabled = false;
                gui_screen_widget_set_display_group(this, 1, 0x10046);
            } else {
                if (parent_list->get_name() != item->get_name()) {
                    item->set_enabled(false);
                    enabled = false;
                }
                gui_screen_widget_set_display_group(this, 1, _string_id_a_confirm_b_cancel);
            }
            if (item_list && item_group) {
                bool visible = item->get_element_handle() == ((c_gui_list_widget *)list)->get_focused_element_handle();
                item_group->set_visible(visible);
                item_group->set_child_visible(_gui_text_widget, 0x10132, visible && parent_list == list);
                item_group->set_child_visible(_gui_text_widget, 0x10131, visible && parent_list != list);
                item_list->set_enabled(enabled);
            }
        }
    }
}

// 0x4bc628
// @address 0x4bc628 ?within_focus_chain@c_color_swatch_focus_list_item_widget@@MAA_NXZ
bool c_color_swatch_focus_list_item_widget::within_focus_chain()
{
    c_gui_list_widget *list = gui_widget_get_parent_list_widget(this);
    long focused_item = list->m_focused_item_index;
    long item_index = get_list_item_index();
    return focused_item == item_index;
}

static __forceinline void start_menu_settings_appearance_colors_set_focused_item(
    c_gui_screen_widget *screen, long widget_name, long item_index)
{
    c_gui_widget *list = screen->get_child_widget(_gui_list_widget, widget_name);
    if (list != 0)
        ((c_gui_list_widget *)list)->set_focused_item_index(item_index, false);
}

// 0x4bd1d8
// @address 0x4bd1d8 ?set_color_values_from_profile@c_start_menu_settings_appearance_colors@@AAAXXZ
void c_start_menu_settings_appearance_colors::set_color_values_from_profile()
{
    e_controller_index controller_index = get_any_responding_controller();
    if ((unsigned long)controller_index <= 3) {
        if (controller_get(controller_index)->in_use()) {
            c_player_profile_interface *profile = controller_get(controller_index)->get_player_profile_interface();
            start_menu_settings_appearance_colors_set_focused_item(
                this, _appearance_color_primary, profile->customization.primary_change_color);
            start_menu_settings_appearance_colors_set_focused_item(
                this, _appearance_color_secondary, profile->customization.secondary_change_color);
            start_menu_settings_appearance_colors_set_focused_item(
                this, _appearance_color_tertiary, profile->customization.tertiary_change_color);
            start_menu_settings_appearance_colors_set_focused_item(this, _appearance_color_emblem_primary,
                s_emblem_info(profile->customization.emblem_info).primary_color);
            start_menu_settings_appearance_colors_set_focused_item(this, _appearance_color_emblem_secondary,
                s_emblem_info(profile->customization.emblem_info).secondary_color);
            start_menu_settings_appearance_colors_set_focused_item(this, _appearance_color_emblem_background,
                s_emblem_info(profile->customization.emblem_info).background_color);
        }
    }
}



// 0x4bd348
// @address 0x4bd348 ?create_list_item_widget@c_start_menu_settings_appearance_colors@@MAAPAVc_gui_list_item_widget@@PBUs_list_item_widget_block@@@Z
c_gui_list_item_widget *c_start_menu_settings_appearance_colors::create_list_item_widget(
    const s_list_item_widget_block *list_item_widget_block)
{
    if (list_item_widget_block->core.name == _start_menu_string_color_swatch_focus)
        return new(0, 0, _ui_allocation_marker_dummy) c_color_swatch_focus_list_item_widget();
    return new(0, 0, _ui_allocation_marker_dummy) c_gui_list_item_widget();
}

// 0x4bd3f0
// @address 0x4bd3f0 start_menu_settings_appearance_colors_get_primary
extern "C" bool start_menu_settings_appearance_colors_get_primary(void *context, wchar_t *buffer, long count)
{
    return start_menu_settings_appearance_colors_get_focused_color_text(context, buffer, count, _appearance_color_primary);
}

// 0x4bd400
// @address 0x4bd400 start_menu_settings_appearance_colors_get_secondary
extern "C" bool start_menu_settings_appearance_colors_get_secondary(void *context, wchar_t *buffer, long count)
{
    return start_menu_settings_appearance_colors_get_focused_color_text(context, buffer, count, _appearance_color_secondary);
}

// 0x4bd410
// @address 0x4bd410 start_menu_settings_appearance_colors_get_tertiary
extern "C" bool start_menu_settings_appearance_colors_get_tertiary(void *context, wchar_t *buffer, long count)
{
    return start_menu_settings_appearance_colors_get_focused_color_text(context, buffer, count, _appearance_color_tertiary);
}

// 0x4bd420
// @address 0x4bd420 start_menu_settings_appearance_colors_get_emblem_primary
extern "C" bool start_menu_settings_appearance_colors_get_emblem_primary(void *context, wchar_t *buffer, long count)
{
    return start_menu_settings_appearance_colors_get_focused_color_text(context, buffer, count, _appearance_color_emblem_primary);
}

// 0x4bd430
// @address 0x4bd430 start_menu_settings_appearance_colors_get_emblem_secondary
extern "C" bool start_menu_settings_appearance_colors_get_emblem_secondary(void *context, wchar_t *buffer, long count)
{
    return start_menu_settings_appearance_colors_get_focused_color_text(context, buffer, count, _appearance_color_emblem_secondary);
}

// 0x4bd440
// @address 0x4bd440 start_menu_settings_appearance_colors_get_emblem_background
extern "C" bool start_menu_settings_appearance_colors_get_emblem_background(void *context, wchar_t *buffer, long count)
{
    return start_menu_settings_appearance_colors_get_focused_color_text(context, buffer, count, _appearance_color_emblem_background);
}

// 0x4bd450
// @address 0x4bd450 start_menu_settings_appearance_colors_get_focused_color_text
extern "C" bool start_menu_settings_appearance_colors_get_focused_color_text(void *context, wchar_t *buffer, long count, int widget_name)
{
    bool result = false;
    c_gui_screen_widget *screen = (c_gui_screen_widget *)context;
    c_gui_list_widget *list = (c_gui_list_widget *)screen->get_child_widget(_gui_list_widget, widget_name);
    if (list != 0)
    {
        long focused_item = list->m_focused_item_index;
        c_gui_data *datasource = 0;
        for (long i = 0; i < screen->m_datasource_count; i++)
        {
            if (screen->m_datasource[i]->get_name() == 0x10009)
            {
                datasource = screen->m_datasource[i];
                break;
            }
        }
        if (focused_item != -1 && datasource != 0)
        {
            for (long element = datasource->get_first_element_handle(); element != -1;
                element = datasource->get_next_element_handle(element))
            {
                long value = -1;
                if (!datasource->get_integer_value(element, 0x1000a, &value) || value != focused_item)
                    continue;
                long string_id;
                if (!datasource->get_string_id_value(element, 0x29c, &string_id))
                    continue;
                c_static_wchar_string<1024> string;
                if (screen->get_string_by_string_id(string_id, &string))
                {
                    long size = count;
                    if ((unsigned long)count > 0x400)
                        size = 0x400;
                    ustrnzcpy(buffer, string.m_string, size);
                    result = true;
                    break;
                }
            }
        }
    }
    return result;
}


// @address 0x4be5a8 ?submenu_invoked@c_start_menu_settings_camera@@UAAXPAVc_gui_list_widget@@@Z
void c_start_menu_settings_camera::submenu_invoked(c_gui_list_widget *list_widget)
{
    e_controller_index index = get_any_responding_controller();
    if ((unsigned long)index > 3)
        return;
    if (!controller_get(index)->in_use())
        return;
    c_player_profile_interface *profile = controller_get(index)->get_player_profile_interface();
    long value;
    switch (list_widget->m_name - _start_menu_string_camera_horizontal_inversion) {
    case 0: value = profile->camera.unknown_setting_20; break;
    case 1: value = profile->camera.unknown_setting_24; break;
    case 2: value = profile->camera.unknown_setting_28; break;
    case 3: value = profile->camera.unknown_setting_2c; break;
    case 4: value = profile->camera.unknown_setting_30; break;
    default: return;
    }
    if (value != -1)
        list_widget->set_focused_datasource_element_from_value(_string_id_value, value, false);
}

// 0x4bd5f8
// @address 0x4bd5f8 start_menu_settings_appearance_colors_get_item_value
extern "C" int start_menu_settings_appearance_colors_get_item_value(c_gui_list_item_widget *object)
{
    return object->m_list_item_index;
}

// @address 0x4be1f8 ?initialize@c_start_menu_settings_camera@@UAAXXZ
void c_start_menu_settings_camera::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        k_appearance_focused_item_tag, this, start_menu_settings_appearance_colors_get_focused_item_text));
}

// @address 0x4be670 start_menu_settings_appearance_colors_get_focused_item_text
extern "C" bool start_menu_settings_appearance_colors_get_focused_item_text(void *context, wchar_t *output, long capacity)
{
    c_gui_screen_widget *screen = (c_gui_screen_widget *)context;
    bool success = false;
    c_gui_widget *focused = screen->get_focused_widget();
    c_gui_widget *parent = focused ? gui_widget_get_parent_list_widget(focused) : 0;
    c_gui_widget *sidebar = screen->get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items);
    c_gui_data *data = sidebar ? ((c_gui_list_widget *)sidebar)->get_data() : 0;
    long value;
    do
    {
        if (parent == sidebar && data)
        {
            if (!data->get_string_id_value(
                ((c_gui_list_widget *)sidebar)->get_focused_element_handle(), _start_menu_string_description, &value))
                break;
        }
        else
        {
            c_gui_widget *first = screen->get_child_widget(_gui_list_widget, _start_menu_string_camera_horizontal_inversion);
            c_gui_widget *second = screen->get_child_widget(_gui_list_widget, _start_menu_string_camera_panning);
            c_gui_widget *third = screen->get_child_widget(_gui_list_widget, _start_menu_string_camera_look_sensitivity);
            c_gui_widget *fourth = screen->get_child_widget(_gui_list_widget, _start_menu_string_camera_speed);
            c_gui_widget *fifth = screen->get_child_widget(_gui_list_widget, _start_menu_string_camera_thrust);
            c_gui_data *selected_data;
            long element;
            if (first && first->within_focus_chain())
            {
                selected_data = ((c_gui_list_widget *)first)->get_data();
                element = ((c_gui_list_widget *)first)->get_focused_element_handle();
            }
            else if (second && second->within_focus_chain())
            {
                selected_data = ((c_gui_list_widget *)second)->get_data();
                element = ((c_gui_list_widget *)second)->get_focused_element_handle();
            }
            else if (third && third->within_focus_chain())
            {
                selected_data = ((c_gui_list_widget *)third)->get_data();
                element = ((c_gui_list_widget *)third)->get_focused_element_handle();
            }
            else if (fourth && fourth->within_focus_chain())
            {
                selected_data = ((c_gui_list_widget *)fourth)->get_data();
                element = ((c_gui_list_widget *)fourth)->get_focused_element_handle();
            }
            else if (fifth && fifth->within_focus_chain())
            {
                selected_data = ((c_gui_list_widget *)fifth)->get_data();
                element = ((c_gui_list_widget *)fifth)->get_focused_element_handle();
            }
            else
                break;
            if (!selected_data || element == -1)
                break;
            if (!selected_data->get_string_id_value(element, _start_menu_string_description, &value))
                break;
        }
        if (value != -1)
        {
            c_static_wchar_string<1024> text;
            if (screen->get_string_by_string_id(value, &text))
            {
                long count = capacity;
                if ((unsigned long)capacity > 1024)
                    count = 1024;
                ustrnzcpy(output, text.m_string, count);
                success = true;
            }
        }
    } while (false);
    return success;
}

static __forceinline long start_menu_settings_appearance_colors_item_index(c_gui_widget *item)
{
    return ((c_gui_list_item_widget *)item)->get_list_item_index();
}

static __forceinline long start_menu_settings_appearance_colors_color_index(c_gui_widget *list)
{
    return minimum(maximum(((c_gui_list_widget *)list)->get_focused_element_handle(), 0L), 31L);
}

// @address 0x4bc8d0 ?update@c_start_menu_settings_appearance_colors@@UAAXK@Z
void c_start_menu_settings_appearance_colors::update(unsigned long current_milliseconds)
{
    update_color_picker_enabled_state();
    update_model_widget();
    c_start_menu_pane_screen_widget::update(current_milliseconds);
}

// @address 0x4bc920 ?update_model_widget@c_start_menu_settings_appearance_colors@@AAAXXZ
void c_start_menu_settings_appearance_colors::update_model_widget()
{
    if (get_child_widget(_gui_list_widget, 0x1012f))
    {
        c_gui_widget *primary = get_child_widget(_gui_list_widget, _appearance_color_primary);
        c_gui_widget *secondary = get_child_widget(_gui_list_widget, _appearance_color_secondary);
        c_gui_widget *tertiary = get_child_widget(_gui_list_widget, _appearance_color_tertiary);
        c_gui_widget *emblem_primary = get_child_widget(_gui_list_widget, _appearance_color_emblem_primary);
        c_gui_widget *emblem_secondary = get_child_widget(_gui_list_widget, _appearance_color_emblem_secondary);
        c_gui_widget *emblem_background = get_child_widget(_gui_list_widget, _appearance_color_emblem_background);
        if (primary && secondary && tertiary && emblem_primary && emblem_secondary && emblem_background)
        {
            c_gui_widget *group = gui_widget_find_child_by_name(this, _gui_group_widget);
            c_gui_widget *model = group ? gui_widget_find_child_by_name(group, _gui_model_widget) : 0;
            if (model)
            {
                s_player_appearance appearance;
                c_player_profile_interface *profile = controller_get(get_any_responding_controller())->get_player_profile_interface();
                profile->get_player_appearance(&appearance);
                appearance.change_colors[0] = (signed char)start_menu_settings_appearance_colors_color_index(primary);
                appearance.change_colors[1] = (signed char)start_menu_settings_appearance_colors_color_index(secondary);
                appearance.change_colors[2] = (signed char)start_menu_settings_appearance_colors_color_index(tertiary);
                appearance.emblem_primary_color = (signed char)start_menu_settings_appearance_colors_color_index(emblem_primary);
                appearance.emblem_secondary_color = (signed char)start_menu_settings_appearance_colors_color_index(emblem_secondary);
                appearance.emblem_background_color = (signed char)start_menu_settings_appearance_colors_color_index(emblem_background);
                ((c_gui_model_widget *)model)->set_player_appearance(&appearance, -1);
            }
        }
    }
}

// @address 0x4bcbe0 ?handle_controller_input_message@c_start_menu_settings_appearance_colors@@UAA_NPBVc_controller_input_message@@@Z
bool c_start_menu_settings_appearance_colors::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    long event_type = message->get_event_type();
    long component = message->get_component();
    c_gui_widget *focused = get_focused_widget();
    c_gui_widget *parent = focused ? gui_widget_get_parent_list_widget(focused) : 0;
    c_gui_widget *list = get_child_widget(_gui_list_widget, 0x1012f);
    if (parent && list)
    {
        if (parent == list)
        {
            if (event_type == _event_type_button_press)
            {
                if (component == 0)
                {
                    c_gui_widget *current = get_focused_widget();
                    c_gui_widget *parent_item = current ? gui_widget_get_parent_list_item_widget(current) : 0;
                    c_gui_widget *picker = get_child_widget(_gui_list_widget, parent_item->m_name);
                    if (picker)
                    {
                        long selected = ((c_gui_list_widget *)picker)->m_focused_item_index;
                        for (c_gui_widget *item = gui_widget_find_child_by_name(picker, _gui_list_item_widget); item;
            item = gui_widget_get_next_list_item(item, true))
                        {
                            if (selected == start_menu_settings_appearance_colors_item_index(item))
                            {
                                gui_screen_widget_transfer_focus_without_animations(this, item, true, true);
                                handled = true;
                            }
                        }
                    }
                }
                else if (component == 1)
                {
                    close_current_subpane();
                    handled = true;
                }
            }
        }
        else if (event_type == _event_type_button_press && (component == 0 || component == 1))
        {
            long name = parent->m_name;
            for (c_gui_widget *item = gui_widget_find_child_by_name(list, _gui_list_item_widget); item;
            item = gui_widget_get_next_list_item(item, true))
            {
                if (item->m_name == name)
                {
                    ((c_gui_list_widget *)list)->set_focused_item_index(start_menu_settings_appearance_colors_item_index(item), false);
                    gui_screen_widget_transfer_focus_without_animations(this, item, true, true);
                    handled = true;
                    break;
                }
            }
            if (component == 0)
            {
                e_player_color_index color = (e_player_color_index)((c_gui_list_widget *)parent)->m_focused_item_index;
                c_player_profile_interface *profile = controller_get(message->get_controller())->get_player_profile_interface();
                s_emblem_info emblem = profile->customization.emblem_info;
                switch (name)
                {
                case _appearance_color_primary: if (color >= 0) profile->set_primary_change_color(color, true); break;
                case _appearance_color_secondary: if (color >= 0) profile->set_secondary_change_color(color, true); break;
                case _appearance_color_tertiary: if (color >= 0) profile->set_tertiary_change_color(color, true); break;
                case _appearance_color_emblem_primary:
                    emblem.primary_color = (signed char)color;
                    profile->set_emblem_info(&emblem, true);
                    break;
                case _appearance_color_emblem_secondary:
                    emblem.secondary_color = (signed char)color;
                    profile->set_emblem_info(&emblem, true);
                    break;
                case _appearance_color_emblem_background:
                    emblem.background_color = (signed char)color;
                    profile->set_emblem_info(&emblem, true);
                    break;
                }
                handled = true;
            }
            else if (component == 1)
            {
                set_color_values_from_profile();
                handled = true;
            }
        }
    }
    if (!handled && message->get_event_type() == _event_type_button_press)
    {
        switch (message->get_component())
        {
        case _controller_component_button_x: player_emblem_toggle_alternate_emblem_foreground(message->get_controller());
        case _controller_component_button_left_thumb: handled = true; break;
        case _controller_component_button_right_thumb: handled = true; break;
        }
    }
    if (!handled)
        handled = c_start_menu_pane_screen_widget::handle_controller_input_message(message);
    return handled;
}

static __forceinline c_gui_widget *start_menu_settings_appearance_colors_focused_item(c_gui_screen_widget *screen)
{
    c_gui_widget *item = screen->get_focused_widget();
    if (item && item->get_type() == _gui_list_item_widget)
        return item;
    return 0;
}

// @address 0x4be398 ?handle_controller_input_message@c_start_menu_settings_camera@@UAA_NPBVc_controller_input_message@@@Z
bool c_start_menu_settings_camera::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    c_gui_widget *focused = start_menu_settings_appearance_colors_focused_item(this);
    c_gui_widget *list;
    c_gui_data *data;
    if (focused)
    {
        list = gui_widget_get_parent_list_widget(focused);
        data = list ? ((c_gui_list_widget *)list)->get_data() : 0;
    }
    else
    {
        list = 0;
        data = 0;
    }
    e_controller_index controller = message->get_controller();
    long event_type = message->get_event_type();
    long component = message->get_component();
    if ((unsigned long)controller <= 3 && controller_get(controller)->in_use())
    {
        c_player_profile_interface *profile = controller_get(controller)->get_player_profile_interface();
        if (list && data && event_type == _event_type_button_press && component == 0)
        {
            long value = -1;
            if (data->get_integer_value(((c_gui_list_widget *)list)->get_focused_element_handle(), _string_id_value, &value))
            {
                switch (list->m_name)
                {
                case _start_menu_string_camera_horizontal_inversion:
                    if (value >= 0) profile->set_unknown_setting_20(value, true);
                    break;
                case _start_menu_string_camera_panning:
                    if (value >= 0) profile->set_unknown_setting_24(value, true);
                    break;
                case _start_menu_string_camera_look_sensitivity:
                    if (value >= 0) profile->set_unknown_setting_28(value, true);
                    break;
                case _start_menu_string_camera_speed:
                    if (value >= 0) profile->set_unknown_setting_2c(value, true);
                    break;
                case _start_menu_string_camera_thrust:
                    if (value >= 0) profile->set_unknown_setting_30(value, true);
                    break;
                }
                focused->handle_widget_back_out();
                handled = true;
            }
        }
    }
    if (!handled)
        handled = c_start_menu_pane_screen_widget::handle_controller_input_message(message);
    return handled;
}
