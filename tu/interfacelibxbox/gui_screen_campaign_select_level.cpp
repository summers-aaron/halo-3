// interfacelibxbox/gui_screen_campaign_select_level.obj: from the records 0x4c3968 0x4c3978 0x4c3998 0x4c3be8 0x4c3c10 0x4c3d48 0x4c3e80 0x4c3f08 0x4c4460 0x4c45e0 0x4c4690 0x4c48e0
#include "interface/gui_screens/campaign/campaign_select_difficulty_message.h"
// @flags /TP /O2 /Oc
#include "interface/gui_pregame_setup_manager_types.h"
#include "interface/gui_screens/campaign/gui_screen_campaign_select_difficulty.h"
#include "memory/static_arrays.h"
#include "interface/gui_pregame_setup_manager.h"
#include "interface/gui_screen_campaign_select_level_externs.h"
#include "interface/user_interface_utilities.h"
#include "interface/custom_map_image_types.h"
#include "interface/gui_selected_items.h"
#include "interface/gui_screens/campaign/campaign_select_level_string_ids.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "tag_files/string_ids.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "interface/c_player_profile.h"
#include "main/levels.h"
#include <string.h>
#include "shell/shell.h"
#include "game/game_types.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_data.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "saved_games/saved_game_files.h"
#include "interface/gui_screens/campaign/gui_screen_campaign_select_level.h"
#include "saved_games/content_catalogue.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/c_gui_text_widget.h"
#include "interface/c_gui_widget.h"
#include "interface/gui_selected_items_level.h"
// @address 0x4c3d48 ?post_initialize@c_gui_screen_campaign_select_level@@UAAXXZ





#include "interface/gui_widget.h"
#include "interface/game_tag_parser.h"
#include "interface/c_gui_list_widget.h"

#include "interface/user_interface_messages.h"
#include "interface/c_gui_screen_widget.h"
#include <new>
#include "interface/gui_custom_bitmap_widget.h"
#include <stdlib.h>
#include "text/unicode.h"

typedef c_gui_data gui_screen_campaign_select_level_element;



typedef c_gui_level_selected_item gui_screen_campaign_select_level_selected_item;
typedef char campaign_selection_campaign_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_level_selected_item, m_sort_key) == 0x190))) ? 1 : -1];
typedef char campaign_selection_map_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_level_selected_item, m_map_id) == 0x194))) ? 1 : -1];
typedef char campaign_selection_locked_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_level_selected_item, m_flag3) == 0x198))) ? 1 : -1];
typedef char campaign_selection_insertion_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_level_selected_item, m_extra) == 0x19a))) ? 1 : -1];




typedef char campaign_select_level_mode_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_campaign_select_level, m_mode) == 0x1a88) )) ? 1 : -1];
typedef char campaign_select_level_campaign_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_screen_campaign_select_level, m_campaign_id) == 0x1a8c) )) ? 1 : -1];
typedef char campaign_select_level_map_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_campaign_select_level, m_map_id) == 0x1a90) )) ? 1 : -1];

#include "interface/gui_campaign_insertion_datasource.h"
// @address 0x53a60 ??_7c_gui_campaign_insertion_datasource@@6B@

// @address 0x4c3e80 ?create_bitmap_widget@c_gui_screen_campaign_select_level@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget * c_gui_screen_campaign_select_level::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _campaign_select_level_string_level_image)
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_custom_bitmap_widget), 0, 0);
        if (memory != 0)
        {
            return new (memory) c_gui_custom_bitmap_widget();
        }
    }
    else
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_bitmap_widget), 0, 0);
        if (memory != 0)
        {
            return new (memory) c_gui_bitmap_widget();
        }
    }
    return 0;
}

// @address 0x4c3968 ?get_current_item_count_internal@c_gui_campaign_insertion_datasource@@MAAJXZ
long c_gui_campaign_insertion_datasource::get_current_item_count_internal()
{
    return m_count;
}

// @address 0x4c3978 ?initialize@c_gui_campaign_insertion_datasource@@UAA_NJ@Z
bool c_gui_campaign_insertion_datasource::initialize(long name)
{
    m_name = name;
    m_count = 0;
    m_flags = 1;
    return 1;
}
#define TEST_BIT16(flags, bit) (_rotl((flags), 32 - (bit)) & 1)



// match: 0x4c4690 reads profile+0x38 (look_sensitivity), despite using it as difficulty.
// The existing field-disputes.tsv entry records the discrepancy with difficulty at +0xc0.




static __forceinline bool gui_screen_campaign_select_level_insertion_unlocked(
    const c_player_profile_interface *profile, short level_index, short insertion_index)
{
    bool unlocked = false;
    if (level_index >= 0 && level_index < 32)
        if (insertion_index >= 0 && insertion_index < 4)
        {
            long index = level_index * 4 + insertion_index;
            unlocked = (profile->campaign.campaign_map_completion[index >> 5] & (1 << (index & 31))) != 0;
        }
    return unlocked;
}

// @address 0x4c3998 ?update_insertions@c_gui_campaign_insertion_datasource@@QAAFJJ@Z
short c_gui_campaign_insertion_datasource::update_insertions(long campaign_id, long map_id)
{
    short unlocked_count = 0;
    levels_insertion_datum insertion;
    if (levels_try_and_get_insertion(map_id, &insertion))
    {
        long level_index = (short)levels_get_campaign_map_index(campaign_id, map_id);
        if (map_id != -1)
        {
            m_flags = 0;
            m_count = insertion.insertion_count;
            for (short insertion_index = 0; insertion_index < m_count; insertion_index++)
            {
                ustrnzcpy_inline(m_names[insertion_index], insertion.names[insertion_index], 32);
                ustrnzcpy_inline(m_descriptions[insertion_index], insertion.descriptions[insertion_index], 128);
            }
            for (e_controller_index controller_index = first_controller();
                controller_index != _controller_index_none;
                controller_index = next_controller(controller_index))
            {
                s_controller *controller = controller_get(controller_index);
                bool signed_in = ((const c_controller_interface *)controller)->is_signed_in_to_machine() || TEST_BIT16(controller->flags, 2);
                if (signed_in)
                {
                    const c_player_profile_interface *profile = controller->get_player_profile_interface();
                    for (long insertion_index = 0; insertion_index < m_count; insertion_index++)
                    {
                        if ((m_flags & (1 << insertion_index)) == 0 &&
                            (gui_screen_campaign_select_level_insertion_unlocked(profile, level_index, (short)insertion_index) || insertion_index == 0))
                        {
                            unlocked_count++;
                            m_flags |= 1 << insertion_index;
                        }
                    }
                }
            }
        }
    }
    return unlocked_count;
}


// @address 0x4c3be8 ?get_column_names@c_gui_campaign_insertion_datasource@@UAAXQAJPAJ@Z
void c_gui_campaign_insertion_datasource::get_column_names(long *const value_name, long *count)
{
    *count = 0;
    *value_name = _campaign_select_level_string_insertion_point;
    *count = *count + 1;
}

// @address 0x4c3c10 ?get_text_value@c_gui_campaign_insertion_datasource@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_campaign_insertion_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_count)
    {
        switch (value_name)
        {
        case _campaign_select_level_string_insertion_point:
            if ((m_flags & (1 << element_handle)) != 0)
            {
                value->set(m_names[element_handle]);
                result = true;
            }
            else
            {
                if (text_group_get_string(m_string_list_tag_index, _campaign_select_level_string_insertion_point_locked, value))
                {
                    result = true;
                }
            }
            break;
        case _campaign_select_level_string_insertion_point_description:
            if ((m_flags & (1 << element_handle)) != 0)
            {
                value->set(m_descriptions[element_handle]);
                result = true;
            }
            else
            {
                text_group_get_string(m_string_list_tag_index, _campaign_select_level_string_insertion_point_description_locked, value);
                result = true;
            }
            break;
        }
    }
    return result;
}



static __forceinline gui_screen_campaign_select_level_selected_item *gui_screen_campaign_select_level_get(
    gui_screen_campaign_select_level_element *data, long index)
{
    return (gui_screen_campaign_select_level_selected_item *)data->get_gui_selected_item(index);
}

void c_gui_screen_campaign_select_level::post_initialize()
{
    c_gui_screen_widget::post_initialize();
    gui_screen_campaign_select_level_element *data = 0;
    for (long element_index = 0; element_index < m_datasource_count; element_index = element_index + 1)
    {
        if (m_datasource[element_index]->m_name == _campaign_select_level_string_level)
        {
            data = m_datasource[element_index];
            break;
        }
    }
    c_gui_list_widget *list = (c_gui_list_widget *)this->get_child_widget(_gui_list_widget, _campaign_select_level_string_level);
    if (list == 0 || data == 0)
    {
        return;
    }
    for (long item_index = data->get_first_element_handle(); item_index != -1; item_index = data->get_next_element_handle(item_index))
    {
        gui_screen_campaign_select_level_selected_item *item = gui_screen_campaign_select_level_get(data, item_index);
        if (item != 0 && item->m_type == 0 && item->m_sort_key == m_campaign_id && item->m_map_id == m_map_id)
        {
            (this)->transfer_focus_to_list(list, item_index, false, false);
            return;
        }
    }
}


// @address 0x4c4460 ?initialize_datasource@c_gui_screen_campaign_select_level@@UAAXXZ
void c_gui_screen_campaign_select_level::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    void *memory = user_interface_malloc_tracked(sizeof(c_gui_level_subitem_selectable_item_datasource), 0, 0);
    if (memory != 0)
    {
        c_gui_level_subitem_selectable_item_datasource *datasource = new (memory) c_gui_level_subitem_selectable_item_datasource();
        if (datasource != 0)
        {
            e_controller_index controller = this->get_any_responding_controller();
            if (datasource->initialize_levels(controller, _campaign_select_level_string_level, m_campaign_id))
            {
                m_datasource[m_datasource_count] = (gui_screen_campaign_select_level_element *)datasource;
                m_datasource_count = m_datasource_count + 1;
            }
            else
            {
                datasource->~c_gui_level_subitem_selectable_item_datasource();
                ::user_interface_free((void const *)(datasource));
            }
        }
    }
    void *level_memory = user_interface_malloc_tracked(sizeof(c_gui_campaign_insertion_datasource), 0, 0);
    c_gui_campaign_insertion_datasource *level_datasource = new (level_memory) c_gui_campaign_insertion_datasource(m_core_definition.string_list_index);
    if (level_datasource != 0)
    {
        if (level_datasource->initialize(_campaign_select_level_string_insertion_point))
        {
            m_datasource[m_datasource_count] = (gui_screen_campaign_select_level_element *)level_datasource;
            m_datasource_count = m_datasource_count + 1;
        }
        else
        {
            level_datasource->~c_gui_campaign_insertion_datasource();
            ::user_interface_free((void const *)(level_datasource));
        }
    }
}


static __forceinline bool gui_screen_campaign_select_level_handle_device_selection(const c_dialog_result_message *message)
{
    bool handled = false;
    if (message->m_dialog_name == _start_menu_string_device_selection) {
        if (message->m_dialog_result == 0)
            content_catalogue_display_device_selection_guide_interface(message->m_controller);
        handled = true;
    }
    return handled;
}

// @address 0x4c45e0 ?handle_dialog_result@c_gui_screen_campaign_select_level@@UAA_NPBVc_dialog_result_message@@@Z
bool c_gui_screen_campaign_select_level::handle_dialog_result(const c_dialog_result_message *message)
{
    bool handled = false;
    if (message->m_dialog_name == _campaign_select_level_string_campaign_changing_setting_will_delete_saved_game) {
        if (message->m_dialog_result == 0) {
            saved_game_files_delete_personal_storage(message->m_controller);
            gui_screen_campaign_select_level_handle_selection(this, message->m_controller);
        }
        handled = true;
    }
    if (!handled)
        handled = gui_screen_campaign_select_level_handle_device_selection(message);
    return handled;
}

// March: constructor c_load_campaign_select_difficulty_screen_message(e_controller_index, e_window_index, long,
// e_gui_campaign_difficulty_setup_mode, e_campaign_id, e_map_id, e_campaign_difficulty_level).


static __forceinline void gui_screen_campaign_select_level_transition_out(c_gui_screen_campaign_select_level *screen, long transition_type, bool immediate)
{
    screen->transition_out_with_transition_type(transition_type, immediate ? _screen_transition_normal : _screen_transition_none);
}

// @address 0x4c4690 gui_screen_campaign_select_level_handle_selection
extern "C" void gui_screen_campaign_select_level_handle_selection(c_gui_screen_campaign_select_level *screen, long controller_index)
{
    c_gui_data *data = 0;
    for (long element_index = 0; element_index < screen->m_datasource_count; element_index = element_index + 1)
    {
        if (screen->m_datasource[element_index]->m_name == _campaign_select_level_string_level)
        {
            data = (c_gui_data *)screen->m_datasource[element_index];
            break;
        }
    }
    c_gui_list_widget *list = (c_gui_list_widget *)((c_gui_widget *)screen)->get_child_widget(_gui_list_widget, _campaign_select_level_string_level);
    c_gui_list_widget *insertion_list = (c_gui_list_widget *)((c_gui_widget *)screen)
        ->get_child_widget(_gui_list_widget, _campaign_select_level_string_insertion_point);
    if (data != 0 && list != 0)
    {
        const c_gui_level_selected_item *selected = (const c_gui_level_selected_item *)data->get_gui_selected_item(
            (list)->get_focused_element_handle());
        if (selected != 0)
        {
            c_gui_level_selected_item item(*selected);
            if (insertion_list != 0)
                item.m_extra = (short)(insertion_list)->get_focused_element_handle();
            else
                item.m_extra = 0;
            switch (screen->m_mode)
            {
            case 0:
            {
                c_player_profile_interface *profile =
                    (c_player_profile_interface *)controller_get((e_controller_index)controller_index)->get_player_profile_interface();
                if (profile != 0)
                {
                    long campaign_id = item.m_sort_key;
                    long map_id = item.m_map_id;
                    void *memory = user_interface_malloc_tracked(sizeof(c_load_campaign_select_difficulty_screen_message), 0, 0);
                    if (memory != 0)
                    {
                        c_load_campaign_select_difficulty_screen_message *message = new (memory) c_load_campaign_select_difficulty_screen_message(
                            (e_controller_index)controller_index, screen->get_render_window(), screen->m_name,
            1, campaign_id, map_id, profile->campaign.look_sensitivity);
                        message->set_parent_screen_index(screen->m_screen_index);
                        ::user_interface_messaging_post(message);
                    }
                }
                break;
            }
            case 1:
                g_pregame_setup_manager.set_selected_item((e_controller_index)controller_index,
            (const c_gui_selected_item *)&item, false, -1);
                gui_screen_campaign_select_level_transition_out(screen, 0, true);
                break;
            }
        }
    }
}

static __forceinline bool gui_screen_campaign_select_level_controller_has_save(e_controller_index controller)
{
    bool result = false;
    if (saved_game_files_get_personal_storage_content_item(controller, false) != -1)
        result = true;
    return result;
}

// @address 0x4c48e0 ?handle_list_item_chosen@c_gui_screen_campaign_select_level@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_campaign_select_level::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item, c_gui_data *data)
{
    bool handled = false;
    if (list_name == _campaign_select_level_string_level) {
        c_gui_data *levels = 0;
        for (long i = 0; i < this->m_datasource_count; ++i) {
            if (this->m_datasource[i]->m_name == _campaign_select_level_string_level) {
                levels = (c_gui_data *)this->m_datasource[i];
                break;
            }
        }
        c_gui_campaign_insertion_datasource *insertions = 0;
        for (long i = 0; i < this->m_datasource_count; ++i) {
            if (this->m_datasource[i]->m_name == _campaign_select_level_string_insertion_point) {
                insertions = (c_gui_campaign_insertion_datasource *)this->m_datasource[i];
                break;
            }
        }
        const c_gui_level_selected_item *selected =
            (const c_gui_level_selected_item *)levels->get_gui_selected_item(list_item->get_element_handle());
        if (selected->m_map_id != -1) {
            long campaign_id = selected->m_sort_key;
            short count = insertions->update_insertions(campaign_id, selected->m_map_id);
            if (count > 0) {
                if (gui_widget_get_parent_list_widget(list_item)->invoke_submenu(
                        _campaign_select_level_string_insertion_point, _campaign_select_level_string_insertion_point, false)) {
                    c_gui_list_widget *insertion_list = static_cast<c_gui_list_widget *>(
                        get_child_widget(_gui_list_widget, _campaign_select_level_string_insertion_point));
                    this->transfer_focus_to_list(insertion_list, 0, false, false);
                }
            } else {
                gui_screen_campaign_select_level_handle_selection(this, message->m_controller);
            }
        }
    } else if (list_name == _campaign_select_level_string_insertion_point) {
        bool single_player = (long)this->m_mode == 1 && user_interface_squad_get_player_count() == 1;
        e_controller_index controller = message->m_controller;
        bool show_dialog = single_player && gui_screen_campaign_select_level_controller_has_save(controller);
        if (show_dialog) {
            void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
            if (memory) {
                long name = this->m_name;
                c_load_dialog_screen_message *dialog = new(memory) c_load_dialog_screen_message(controller, this->get_render_window(), name,
                    _campaign_select_level_string_campaign_changing_setting_will_delete_saved_game, name);
                if (dialog) {
                    dialog->set_parent_screen_index(this->m_screen_index);
                    user_interface_messaging_post(dialog);
                }
            }
        } else {
            gui_screen_campaign_select_level_handle_selection(this, message->m_controller);
        }
        handled = true;
    }
    return handled;
}


static __forceinline void campaign_select_set_text(c_gui_widget *widget, const wchar_t *text)
{
    static_cast<c_gui_text_widget *>(widget)->set_text(text);
}

// @address 0x4c3f08 ?update@c_gui_screen_campaign_select_level@@UAAXK@Z
void c_gui_screen_campaign_select_level::update(unsigned long milliseconds)
{
    const long insertion_value_name = _campaign_select_level_string_insertion_point_description;
    const long level_value_name = _string_id_description;
    c_gui_screen_widget::update(milliseconds);
    gui_screen_campaign_select_level_element *data = 0;
    for (long element_index = 0; element_index < m_datasource_count; element_index = element_index + 1)
    {
        if (m_datasource[element_index]->m_name == _campaign_select_level_string_level)
        {
            data = m_datasource[element_index];
            break;
        }
    }
    c_gui_widget *screen = (c_gui_widget *)this;
    c_gui_list_widget *levels = (c_gui_list_widget *)screen->get_child_widget(_gui_list_widget, _campaign_select_level_string_level);
    c_gui_list_widget *insertions = (c_gui_list_widget *)screen->get_child_widget(_gui_list_widget, _campaign_select_level_string_insertion_point);
    e_controller_index controller = screen->get_any_responding_controller();
    c_player_profile_interface *profile = controller_get(controller)->get_player_profile_interface();
    c_gui_widget *map = screen->get_child_widget(_gui_bitmap_widget, _campaign_select_level_string_level_image);
    if (insertions != 0)
    {
        c_gui_list_widget *text_list;
        if ((text_list = (c_gui_list_widget *)screen->get_child_widget(_gui_list_widget, _campaign_select_level_string_insertion_point)) != 0)
        {
            long datasource_name = text_list->m_datasource_name;
            this
                ->set_text_widget_text_from_datasource(
                    _string_id_description, datasource_name, (text_list)->get_focused_element_handle(), insertion_value_name);
        }
    }
    else
    {
        c_gui_list_widget *text_list;
        if ((text_list = (c_gui_list_widget *)screen->get_child_widget(_gui_list_widget, _campaign_select_level_string_level)) != 0)
        {
            long datasource_name = text_list->m_datasource_name;
            long element_handle = (text_list)->get_focused_element_handle();
            this->set_text_widget_text_from_datasource(_string_id_description, datasource_name, element_handle, level_value_name);
        }
    }
    if (data != 0 && map != 0)
    {
        gui_screen_campaign_select_level_selected_item *item =
            (gui_screen_campaign_select_level_selected_item *)data->get_gui_selected_item(levels->get_focused_element_handle());
        if (item != 0)
        {
            map->set_visible(!item->m_locked);
            bool locked = item->m_locked;
            c_gui_widget *lock = screen->get_child_widget(_gui_bitmap_widget, _campaign_select_level_string_level_locked);
            if (lock != 0)
                lock->set_visible(locked);
            if (!item->m_locked)
                ((c_gui_custom_bitmap_widget *)map)->set_map_image(_custom_map_image_type_small, (e_map_id)item->m_map_id, true);
        }
    }
    if (profile != 0)
    {
        for (c_gui_widget *item = gui_widget_find_child_by_name((c_gui_widget *)levels, 5); item != 0; item = gui_widget_get_next_list_item(item, false))
        {
            bool enabled = false;
            unsigned long handle = (unsigned long)item->get_element_handle();
            if (handle <= 31)
            {
                short index = (short)handle;
                bool bit = false;
                if (index >= 0 && index < 32)
                    bit = ((1 << (index & 31)) & profile->campaign.campaign_map_unlocked[index >> 5]) != 0;
                enabled = bit || (long)handle == 0;
            }
            item->set_enabled(enabled);
        }
        if (insertions != 0)
        {
            short level = (short)(levels)->get_focused_element_handle();
            if (level >= 0 && level < 32)
            {
                for (c_gui_widget *item = gui_widget_find_child_by_name((c_gui_widget *)insertions, 5); item != 0;
                    item = gui_widget_get_next_list_item(item, false))
                {
                    unsigned long handle = (unsigned long)item->get_element_handle();
                    if (handle <= 3)
                    {
                        long signed_handle = (long)handle;
                        short insertion = (short)signed_handle;
                        bool enabled;
                        enabled = ((const c_player_profile_interface *)profile)->get_map_completed_at_difficulty_level(level, insertion) || signed_handle == 0;
                        item->set_enabled(enabled);
                    }
                }
            }
        }
        long icon_names[8] = {
            _campaign_select_level_string_solo_easy, _campaign_select_level_string_solo_normal,
            _campaign_select_level_string_solo_heroic, _campaign_select_level_string_solo_legendary,
            _campaign_select_level_string_coop_easy, _campaign_select_level_string_coop_normal,
            _campaign_select_level_string_coop_heroic, _campaign_select_level_string_coop_legendary
        };
        for (long row = 0; row < 2; row = row + 1)
        {
            for (long difficulty = 0; difficulty < 4; difficulty = difficulty + 1)
            {
                c_gui_widget *icon = screen->get_child_widget(_gui_bitmap_widget, icon_names[row * 4 + difficulty]);
                if (icon != 0)
                {
                    long level = (levels)->get_focused_element_handle();
                    bool visible = false;
                    if ((level == -1 || (level >= 0 && (unsigned long)level < 32)) && (unsigned long)difficulty <= 3)
                    {
                        visible = (level != -1) ? ((profile->campaign.map_completed_flags[row][level] & (1 << difficulty)) != 0) : false;
                    }
                    icon->set_visible(visible);
                    ((c_gui_bitmap_widget *)icon)->set_sprite_frame(difficulty);
                }
            }
        }
        c_gui_widget *date0 = screen->get_child_widget(_gui_text_widget, _campaign_select_level_string_solo_last_played);
        c_gui_widget *date1 = screen->get_child_widget(_gui_text_widget, _campaign_select_level_string_coop_last_played);
        unsigned __int64 time0 = profile->campaign.last_campaign_played_time[0];
        unsigned __int64 time1 = profile->campaign.last_campaign_played_time[1];
        c_static_wchar_string<1024> text;
        text.m_string[0] = 0;
        if (date0 != 0)
        {
            date0->set_visible(time0 != 0);
            if (time0 != 0 && user_interface_get_date_string(time0, &text))
                campaign_select_set_text(date0, text.get_string());
        }
        if (date1 != 0)
        {
            date1->set_visible(time1 != 0);
            if (time1 != 0 && user_interface_get_date_string(time1, &text))
                campaign_select_set_text(date1, text.get_string());
        }
    }
}
