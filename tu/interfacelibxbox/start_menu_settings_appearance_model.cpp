// interfacelibxbox/start_menu_settings_appearance_model.obj: from the records 0x4baab0 0x4baae8 0x4bac00 0x4bace0 0x4bad30 0x4bae50 0x4bafc8 0x4bb0f0 0x4bb258 0x4bb300 0x4bb598 0x4bb670 0x4bb8b8 0x4bb9d8 0x4bbb00 0x4bbb88 0x4bbc70 0x4bbda8 0x4bbee8
// @flags /TP /O2 /Oc
#include "cseries/cseries.h"
#include "interface/gui_screen_widget_legacy.h"
#include "shell/shell.h"
#include "interface/gui_widget_tree.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_player_profile.h"
#include <stddef.h>
#include "game/player_types.h"
#include "memory/static_arrays.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface_memory.h"
#include "text/unicode.h"
#include "interface/gui_screens/start_menu/panes/settings_appearance_model/start_menu_settings_appearance_model.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "interface/game_tag_parser.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/customization_types.h"
#include "interface/user_interface_allocation.h"
#include "math/real_math_types.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_animation_transform.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/c_gui_group_widget.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/list_widget_definition.h"
#include "interface/gui_model_widget_types.h"
#include "interface/user_interface_utilities.h"
#include "objects/model_customization_types.h"
#include "objects/model_customization.h"
#include "interface/gui_data.h"
#include "interface/gui_widget.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "interface/c_gui_widget.h"
#include "tag_files/string_ids.h"
#include <new>
// @address 0x4bace0 ?get_column_names@c_model_customization_selections_datasource@@UAAXQAJPAJ@Z
// @address 0x47d8a0 ?set_model_customization_selection@c_player_profile_interface@@QAAXW4e_player_model_choice@@JD_N@Z
// @address 0x4bb258 ?update@c_start_menu_settings_appearance_model@@UAAXK@Z
// @address 0x4bb0f0 ?initialize_datasource@c_start_menu_settings_appearance_model@@UAAXXZ
// @address 0x4bb300 ?update_model_widget@c_start_menu_settings_appearance_model@@AAAX_N@Z
// @address 0x4bb670 ?handle_controller_input_message@c_start_menu_settings_appearance_model@@UAA_NPBVc_controller_input_message@@@Z
// @address 0x4bbda8 ?get_model_customization_selection_name@@YA_NPAVc_gui_screen_widget@@JJJPAJ@Z
// @address 0x4bbee8 ?get_current_model_selection_area_and_item_index@@YA_NPAVc_gui_screen_widget@@PAJ1@Z
// @address 0x4baab0 ?get_column_names@c_settings_appearance_model_sidebar_items_datasource@@UAAXQAJPAJ@Z
// @address 0x4bac00 ?get_current_item_count_internal@c_model_customization_selections_datasource@@MAAJXZ
// @address 0x4bad30 ?get_integer_value@c_model_customization_selections_datasource@@UAA_NJJPAJ@Z
// @address 0x4bae50 ?get_string_id_value@c_model_customization_selections_datasource@@UAA_NJJPAJ@Z
// @address 0x1b7030 ?in_use@c_controller_interface@@QBA_NXZ
// @address 0x354d48 ?get_multiplayer_customized_model_character@@YAPAUs_multiplayer_customized_model_character@@W4e_player_model_choice@@@Z
// @address 0x47d618 ?set_player_model_choice@c_player_profile_interface@@QAAXW4e_player_model_choice@@_N@Z
// @address 0x47e070 ?get_hidden_unlockable_unlocked@c_player_profile_interface@@QBA_NJ@Z
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x4baae8 ?get_string_id_value@c_settings_appearance_model_sidebar_items_datasource@@UAA_NJJPAJ@Z
// @address 0x4bbc70 ?get_current_model_customization_value@@YAJPAVc_player_profile_interface@@J@Z
// @address 0x47d7a0 ?get_model_customization_selection@c_player_profile_interface@@QBADW4e_player_model_choice@@J@Z
// @address 0x4bb598 ?add_model_customization_selections_datasource@c_start_menu_settings_appearance_model@@AAAXJW4e_controller_index@@@Z
// @address 0x4bb9d8 ?parse_xml_part_description@@YA_NPAXPA_WJ@Z
// @address 0x4bbb00 ?parse_xml_part_count_current@@YA_NPAXPA_WJ@Z
// @address 0x4bbb88 ?parse_xml_part_count_total@@YA_NPAXPA_WJ@Z
// @address 0x486f08 ?add_game_tag_parser@c_gui_screen_widget@@IAAXPAVc_game_tag_parser@@@Z
// @address 0x4bb8b8 ?submenu_invoked@c_start_menu_settings_appearance_model@@UAAXPAVc_gui_list_widget@@@Z
// @address 0x52750 ??_7c_settings_appearance_model_sidebar_items_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_settings_appearance_model_sidebar_items_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_settings_appearance_model_sidebar_items_datasource@@UAAPAXI@Z

class c_model_customization_selections_datasource : public c_gui_ordered_data
{
public:
    c_model_customization_selections_datasource(c_player_profile_interface *profile, long name)
        : m_profile(profile), m_name(name)
    {
    }
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
protected:
    virtual long get_current_item_count_internal();
public:
    c_player_profile_interface *m_profile;
    long m_name;
};

static __forceinline void add_column_name(long *column_names, long *column_count, long column_name)
{
    column_names[*column_count] = column_name;
    (*column_count)++;
}

void c_settings_appearance_model_sidebar_items_datasource::get_column_names(
    long *const column_names, long *column_count)
{
    *column_count = 0;
    add_column_name(column_names, column_count, _string_id_value);
    add_column_name(column_names, column_count, _string_id_description);
}

bool c_settings_appearance_model_sidebar_items_datasource::get_string_id_value(
    long element_handle, long value_name, long *value)
{
    bool success = false;
    if (element_handle != -1 && (value_name == _string_id_value || value_name == _string_id_description))
    {
        long area_name = -1;
        if (get_base_datasource()->get_string_id_value(element_handle, _string_id_name, &area_name))
        {
            c_gui_data *data = m_screen->find_data(area_name, 0);
            if (data)
            {
                long current_selection = get_current_model_customization_value(m_profile, area_name);
                if (get_model_customization_selection_name(m_screen, area_name, current_selection,
                    value_name == _string_id_value ? _string_id_name : _string_id_description, value))
                    success = true;
            }
        }
        success = true;
    }
    else
        success = get_base_datasource()->get_string_id_value(element_handle, value_name, value);
    return success;
}

long c_model_customization_selections_datasource::get_current_item_count_internal()
{
    long result = 0;
    s_multiplayer_customized_model_character *character =
        get_multiplayer_customized_model_character((e_player_model_choice)m_profile->get_player_model_choice());

    if (character != 0) {
        long count = character->regions.count();
        for (long i = 0; i < count; i++) {
            s_multiplayer_customized_model_region *regions = character->regions.begin();
            s_multiplayer_customized_model_region *selection = &regions[i];
            if (selection->name == m_name) {
                for (long j = 0; j < selection->permutations.count(); j++) {
                    s_multiplayer_customized_model_permutation *permutations = selection->permutations.begin();
                    s_multiplayer_customized_model_permutation *item = &permutations[j];
                    if ((item->flags & 2) == 0 || m_profile->get_hidden_unlockable_unlocked(item->unlockable))
                        result++;
                }
                break;
            }
        }
    }

    return result;
}

void c_model_customization_selections_datasource::get_column_names(
    long *const column_names, long *column_count)
{
    *column_count = 0;
    add_column_name(column_names, column_count, _string_id_value);
    add_column_name(column_names, column_count, _string_id_name);
    add_column_name(column_names, column_count, _string_id_description);
}

bool c_model_customization_selections_datasource::get_integer_value(
    long element_handle, long value_name, long *value)
{
    bool success = false;
    if (value_name == _string_id_value) {
        s_multiplayer_customized_model_character *character =
            get_multiplayer_customized_model_character((e_player_model_choice)m_profile->get_player_model_choice());

        if (element_handle != -1 && character != 0) {
            for (long i = 0; i < character->regions.count(); i++) {
                s_multiplayer_customized_model_region *regions = character->regions.begin();
                s_multiplayer_customized_model_region *selection = &regions[i];
                if (selection->name == m_name && element_handle >= 0 && element_handle < selection->permutations.count()) {
                    long visible_index = 0;
                    for (long j = 0; j < selection->permutations.count(); j++) {
                        s_multiplayer_customized_model_permutation *permutations = selection->permutations.begin();
                        s_multiplayer_customized_model_permutation *item = &permutations[j];
                        if ((item->flags & 2) == 0 || m_profile->get_hidden_unlockable_unlocked(item->unlockable)) {
                            if (element_handle == visible_index) {
                                *value = j;
                                success = true;
                                break;
                            }
                            visible_index++;
                        }
                    }
                }
            }
        }
    }
    return success;
}

bool c_model_customization_selections_datasource::get_string_id_value(
    long element_handle, long value_name, long *value)
{
    bool success = false;
    s_multiplayer_customized_model_character *character =
        get_multiplayer_customized_model_character((e_player_model_choice)m_profile->get_player_model_choice());

    if (element_handle != -1 && character != 0) {
        for (long i = 0; i < character->regions.count(); i++) {
            s_multiplayer_customized_model_region *regions = character->regions.begin();
            s_multiplayer_customized_model_region *selection = &regions[i];
            if (selection->name == m_name) {
                long visible_index = 0;
                for (long j = 0; j < selection->permutations.count(); j++) {
                    s_multiplayer_customized_model_permutation *permutations = selection->permutations.begin();
                    s_multiplayer_customized_model_permutation *item = &permutations[j];
                    if ((item->flags & 2) != 0 && !m_profile->get_hidden_unlockable_unlocked(item->unlockable))
                        continue;
                    else if (element_handle == visible_index) {
                        if ((item->flags & 1) != 0 && !m_profile->get_hidden_unlockable_unlocked(item->unlockable)) {
                            if (value_name != _string_id_name) {
                                if (value_name != _string_id_description)
                                    break;
                                else {
                                    *value = _string_id_locked_description;
                                    success = true;
                                    break;
                                }
                            }
                            else {
                                *value = _string_id_locked;
                                success = true;
                                break;
                            }
                        }
                        else if (value_name != _string_id_name) {
                            if (value_name != _string_id_description)
                                break;
                            else {
                                *value = item->description;
                                success = true;
                                break;
                            }
                        }
                        else {
                            *value = item->name;
                            success = true;
                            break;
                        }
                    }
                    else {
                        visible_index++;
                        continue;
                    }
                }
            }
        }
    }

    return success;
}

// @address 0x4bafc8 ?initialize@c_start_menu_settings_appearance_model@@UAAXXZ
void c_start_menu_settings_appearance_model::initialize()
{
    gui_screen_widget_initialize(this);
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<part-description", this, parse_xml_part_description));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<part-count-current", this, parse_xml_part_count_current));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<part-count-total", this, parse_xml_part_count_total));
}

void c_start_menu_settings_appearance_model::update(unsigned long current_milliseconds)
{
    c_start_menu_pane_screen_widget::update(current_milliseconds);
    c_gui_widget *widget = get_child_widget(_gui_text_widget, _start_menu_string_part_count);
    update_model_widget(false);
    if (widget)
    {
        bool visible = true;
        long area, item;
        if (get_current_model_selection_area_and_item_index((c_gui_screen_widget *)this, &area, &item))
            visible = (_start_menu_string_model - area) != 0;
        widget->set_visible(visible);
    }
}

// @address 0x4bc288 ?update_render_state@c_start_menu_settings_appearance_model@@UAAXK@Z
void c_start_menu_settings_appearance_model::update_render_state(unsigned long current_milliseconds)
{
    e_controller_index controller_index = get_any_responding_controller();
    long color = controller_get(controller_index)->player_profile.customization.primary_change_color;
    c_gui_widget::update_render_state(current_milliseconds);
    c_gui_widget *child = get_child_widget(_gui_bitmap_widget, _start_menu_string_base_color);
    while (child != 0)
    {
        if (child->m_name == _start_menu_string_base_color)
            tint_widget_to_change_color(child, color, false);
        child = gui_widget_get_next_sibling_of_type(child, _gui_bitmap_widget);
    }
}

void c_start_menu_settings_appearance_model::update_model_widget(bool force)
{
    long value, area, item, kind, first;
    s_player_appearance appearance;
    if (get_current_model_selection_area_and_item_index((c_gui_screen_widget *)this, &area, &item))
    {
        c_gui_widget *group = gui_widget_find_child_by_name(this, _gui_group_widget);
        c_gui_widget *model = group ? gui_widget_find_child_by_name(group, _gui_model_widget) : 0;
        c_gui_data *data = find_data(area, 0);
        if (model && data)
        {
            ((c_gui_model_widget *)model)->reset_player_appearance();
            appearance = *((c_gui_model_widget *)model)->get_player_appearance();
            if (data->get_integer_value(item, _string_id_value, &value) &&
                data->get_string_id_value(item, _string_id_name, &kind) &&
                data->get_integer_value(0, _string_id_value, &first))
            {
                if (kind == _string_id_locked) value = first;
                switch (area)
                {
                case _start_menu_string_model: appearance.player_model_choice = (signed char)PIN(value, 0, 1); break;
                case _start_menu_string_helmet:
                appearance.model_permutations[appearance.player_model_choice][0] =
                    (unsigned char)PIN(value, 0, 31);
                break;
                case _start_menu_string_leftshoulder:
                appearance.model_permutations[appearance.player_model_choice][1] =
                    (unsigned char)PIN(value, 0, 31);
                break;
                case _start_menu_string_rightshoulder:
                appearance.model_permutations[appearance.player_model_choice][2] =
                    (unsigned char)PIN(value, 0, 31);
                break;
                case _string_id_body:
                appearance.model_permutations[appearance.player_model_choice][3] =
                    (unsigned char)PIN(value, 0, 31);
                break;
                }
            }
            ((c_gui_model_widget *)model)->set_player_appearance(&appearance, -1);
        }
    }
}

void c_start_menu_settings_appearance_model::add_model_customization_selections_datasource(long name, e_controller_index controller)
{
    c_player_profile_interface *profile = controller_get(controller)->get_player_profile_interface();
    c_model_customization_selections_datasource *data = new(0, 0, _ui_allocation_marker_dummy) c_model_customization_selections_datasource(profile, name);
    if (data)
    {
        if (data->initialize(name))
            this->add_datasource(data);
        else
        {
            data->~c_model_customization_selections_datasource();
            user_interface_free(data);
        }
    }
}

void c_start_menu_settings_appearance_model::initialize_datasource()
{
    gui_screen_widget_initialize_datasource(this);
    long controller_index = get_any_responding_controller();
    if ((unsigned long)controller_index <= 3 && controller_get((e_controller_index)controller_index)->in_use())
    {
        c_player_profile_interface *p = controller_get((e_controller_index)controller_index)->get_player_profile_interface();
        long data_index = -1;
        c_gui_data *base = get_data(_start_menu_string_start_menu_game_items, &data_index);
        if (base)
        {
            void *memory = ::user_interface_malloc_tracked(sizeof(c_settings_appearance_model_sidebar_items_datasource), 0, 0);
            if (memory)
            {
                c_settings_appearance_model_sidebar_items_datasource *datasource =
                    new (memory) c_settings_appearance_model_sidebar_items_datasource(base, p, this);
                if (datasource->initialize())
                    add_datasource(datasource);
                else
                    ui_track_delete<c_gui_data>(datasource);
            }
        }
        add_model_customization_selections_datasource(_start_menu_string_helmet, (e_controller_index)controller_index);
        add_model_customization_selections_datasource(_start_menu_string_leftshoulder, (e_controller_index)controller_index);
        add_model_customization_selections_datasource(_start_menu_string_rightshoulder, (e_controller_index)controller_index);
        add_model_customization_selections_datasource(_string_id_body, (e_controller_index)controller_index);
    }
}

bool c_start_menu_settings_appearance_model::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    long event_type = message->get_event_type();
    long controller = message->get_controller();
    long component = message->get_component();
    if (event_type == _event_type_button_press && component == 0)
    {
        c_gui_widget *focused = get_focused_widget();
        c_gui_widget *item = focused && focused->get_type() == _gui_list_item_widget ? focused : 0;
        c_gui_list_widget *list = item ? gui_widget_get_parent_list_widget(item) : 0;
        c_gui_data *data = list ? list->get_data() : 0;
        if (list && data)
        {
            long value = -1;
            if ((unsigned long)controller <= 3 &&
                data->get_string_id_value(list->get_focused_element_handle(), _string_id_name, &value) && value != _string_id_locked)
            {
                c_player_profile_interface *profile = controller_get((e_controller_index)controller)->get_player_profile_interface();
                long selection = -1;
                if (data->get_integer_value(list->get_focused_element_handle(), _string_id_value, &selection))
                {
                    switch (list->m_name)
                    {
                    case _start_menu_string_model: profile->set_player_model_choice((e_player_model_choice)selection, 1); break;
                    case _start_menu_string_helmet:
                    profile->set_model_customization_selection(
                        (e_player_model_choice)profile->get_player_model_choice(), 0, (char)selection, 1);
                    break;
                    case _start_menu_string_leftshoulder:
                    profile->set_model_customization_selection(
                        (e_player_model_choice)profile->get_player_model_choice(), 1, (char)selection, 1);
                    break;
                    case _start_menu_string_rightshoulder:
                    profile->set_model_customization_selection(
                        (e_player_model_choice)profile->get_player_model_choice(), 2, (char)selection, 1);
                    break;
                    case _string_id_body:
                    profile->set_model_customization_selection(
                        (e_player_model_choice)profile->get_player_model_choice(), 3, (char)selection, 1);
                    break;
                    }
                    update_model_widget(1);
                    item->handle_widget_back_out();
                    handled = true;
                }
            }
            else
                ::user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
        }
    }
    if (!handled)
        handled = c_start_menu_pane_screen_widget::handle_controller_input_message(message);
    return handled;
}

long get_current_model_customization_value(c_player_profile_interface *profile, long name)
{
    long result = -1;
    switch (name)
    {
    case _start_menu_string_model:
        result = profile->get_player_model_choice();
        break;
    case _start_menu_string_helmet:
        result = profile->get_model_customization_selection((e_player_model_choice)profile->get_player_model_choice(), 0);
        break;
    case _start_menu_string_leftshoulder:
        result = profile->get_model_customization_selection((e_player_model_choice)profile->get_player_model_choice(), 1);
        break;
    case _start_menu_string_rightshoulder:
        result = profile->get_model_customization_selection((e_player_model_choice)profile->get_player_model_choice(), 2);
        break;
    case _string_id_body:
        result = profile->get_model_customization_selection((e_player_model_choice)profile->get_player_model_choice(), 3);
        break;
    }
    return result;
}

bool get_model_customization_selection_name(c_gui_screen_widget *screen, long datasource_name,
    long selection, long column_name, long *value)
{
    bool success = false;
    c_gui_data *data = screen->find_data(datasource_name, 0);
    if (data) {
        for (long element_handle = data->get_first_element_handle(); element_handle != -1;
            element_handle = data->get_next_element_handle(element_handle)) {
            long selection_index = -1;
            if (data->get_integer_value(element_handle, _string_id_value, &selection_index) && selection_index == selection) {
                if (data->get_string_id_value(element_handle, column_name, value))
                    success = true;
                break;
            }
        }
    }
    return success;
}

bool get_current_model_selection_area_and_item_index(c_gui_screen_widget *screen, long *area, long *item)
{
    bool success = false;
    c_gui_list_widget *list = (c_gui_list_widget *)screen->get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items);
    c_gui_widget *focused = screen->get_focused_widget();
    if (list && focused)
    {
        c_gui_list_widget *parent = gui_widget_get_parent_list_widget(focused);
        if (list->has_active_submenu() && parent && parent->is_submenu())
        {
            *area = parent->m_name;
            *item = parent->m_focused_item_index;
            success = true;
        }
        else
        {
            c_gui_data *data = list->get_data();
            long value = -1;
            if (data && data->get_string_id_value(list->get_focused_element_handle(), _string_id_name, area))
            {
                unsigned int controller = ((c_gui_widget *)screen)->get_any_responding_controller();
                if (controller <= 3 && controller_get((e_controller_index)controller)->in_use())
                {
                    c_player_profile_interface *profile = controller_get((e_controller_index)controller)->get_player_profile_interface();
                    long current = get_current_model_customization_value(profile, *area);
                    c_gui_data *parts = screen->find_data(*area, 0);
                    if (parts)
                    {
                        for (long element = parts->get_first_element_handle(); element != -1;
                            element = parts->get_next_element_handle(element))
                        {
                            long part_value = -1;
                            if (parts->get_integer_value(element, _string_id_value, &part_value) && part_value == current)
                            {
                                *item = element;
                                success = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return success;
}

bool parse_xml_part_description(void *widget, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    c_gui_screen_widget *screen = (c_gui_screen_widget *)widget;
    long area;
    long item;
    if (screen != 0 && get_current_model_selection_area_and_item_index(screen, &area, &item))
    {
        for (long index = 0; index < screen->m_datasource_count; ++index)
        {
            if (screen->m_datasource[index]->get_name() == area)
            {
                c_gui_data *data = screen->m_datasource[index];
                if (data != 0 && data->get_string_id_value(item, _string_id_description, &item) && item != -1 && item != 0)
                {
                    c_static_wchar_string<1024> text;
                    if (screen->get_string_by_string_id(item, &text))
                    {
                        text.copy_to(buffer, buffer_length);
                        success = true;
                    }
                }
                break;
            }
        }
    }
    return success;
}

bool parse_xml_part_count_current(void *widget, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    long area;
    long item;
    if (widget != 0 && get_current_model_selection_area_and_item_index((c_gui_screen_widget *)widget, &area, &item))
    {
        c_static_wchar_string<256> text;
        text.print(L"%d", item + 1);
        unsigned int length = buffer_length;
        if (length > 256)
            length = 256;
        ustrnzcpy(buffer, text.m_string, length);
        success = true;
    }
    return success;
}

void c_start_menu_settings_appearance_model::submenu_invoked(c_gui_list_widget *list_widget)
{
    e_controller_index controller_index = this->get_any_responding_controller();
    if ((unsigned int)controller_index > 3)
        return;
    if (!controller_get(controller_index)->in_use())
        return;
    c_player_profile_interface *profile = controller_get(controller_index)->get_player_profile_interface();
    long value;
    switch (list_widget->m_name)
    {
    case _start_menu_string_model:
        value = profile->get_player_model_choice();
        break;
    case _string_id_body:
        value = profile->get_model_customization_selection((e_player_model_choice)profile->get_player_model_choice(), 3);
        break;
    case _start_menu_string_helmet:
        value = profile->get_model_customization_selection((e_player_model_choice)profile->get_player_model_choice(), 0);
        break;
    case _start_menu_string_leftshoulder:
        value = profile->get_model_customization_selection((e_player_model_choice)profile->get_player_model_choice(), 1);
        break;
    case _start_menu_string_rightshoulder:
        value = profile->get_model_customization_selection((e_player_model_choice)profile->get_player_model_choice(), 2);
        break;
    default:
        return;
    }
    if (value != -1)
        list_widget->set_focused_datasource_element_from_value(_string_id_value, value, false);
}

bool parse_xml_part_count_total(void *widget, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    c_gui_screen_widget *screen = (c_gui_screen_widget *)widget;
    long item;
    long area;
    if (screen != 0 && get_current_model_selection_area_and_item_index(screen, &area, &item))
    {
        for (long index = 0; index < screen->m_datasource_count; ++index)
        {
            if (screen->m_datasource[index]->get_name() == area)
            {
                c_gui_data *data = screen->m_datasource[index];
                if (data != 0)
                {
                    c_static_wchar_string<256> text;
                    text.print(L"%d", data->get_current_item_count());
                    unsigned int length = buffer_length;
                    if (length > 256)
                        length = 256;
                    ustrnzcpy(buffer, text.m_string, length);
                    success = true;
                }
                break;
            }
        }
    }
    return success;
}
