// interfacelibxbox/c_gui_list_widget.obj: from the records 0x4e1340 0x4e1958 0x4e1a58 0x4e1af8 0x4e1b00 0x4e1c80 0x4e1cb8 0x4e1e70 0x4e2118 0x4e2680 0x4e2788 0x4e2830 0x4e28b8 0x4e2958 0x4e2a28 0x4e2a98 0x4e2af8 0x4e2c78 0x4e2d20 0x4e2de8 0x4e2eb8 0x4e2f58 0x4e2fd8 0x4e3188 0x4e31e0 0x4e3350 0x4e34a0 0x4e3600
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "text/font_id_types.h"
#include "text/font_cache_thread_state_types.h"
#include "text/character_data_index_types.h"
#include "interface/c_gui_list_widget.h"
#include "interface/gui_widget_hierarchy.h"
#include "interface/gui_widget_navigation.h"
#include "interface/gui_widget_tree.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/c_gui_group_widget.h"
#include "interface/list_widget_definition.h"
#include "interface/group_widget_definition.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/c_gui_text_widget.h"
#include "interface/gui_widget_types.h"
#include "interface/chud/utf32_types.h"
#include "math/real_math_types.h"
#include "tag_files/tag_groups.h"
#include "interface/core_widget_definition.h"
#include "interface/bitmap_widget_definition.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface_memory.h"
#include "interface/gui_data.h"
#include <string.h>
#include <stdlib.h>
#include "text/font_cache_lock.h"
#include "text/font_cache_types.h"

// @address 0x55fc8 ??_7c_gui_list_widget@@6B@
// @address 0x4a8838 ??_Ec_gui_list_widget@@UAAPAXI@Z
// @address 0x4e8138 ?get_core_definition@c_gui_list_widget@@UAAPAUs_runtime_core_widget_definition@@XZ
// @address 0x4e1340 ?add_definition_fields@c_gui_group_widget@@QAAXPBUs_group_widget_definition@@_N@Z
// @address 0x4e1a58 ?initialize@c_gui_list_widget@@UAAXPBUs_list_widget_block@@@Z
// @address 0x4e2588 ?add_definition_fields@c_gui_list_widget@@QAAXPBUs_list_widget_definition@@_N@Z
// @address 0x48a410 ?create_and_add_child_list_item_widgets@c_gui_widget@@QAAXPBUs_tag_block@@J@Z
// @address 0x489610 ?initialize@c_gui_widget@@UAAXXZ
// @address 0x4e1af8 ?post_initialize@c_gui_list_widget@@UAAXXZ
// @address 0x4e1c80 ?update_render_state@c_gui_list_widget@@UAAXK@Z
// @address 0x4e2a98 ?get_data@c_gui_list_widget@@QAAPAVc_gui_data@@XZ
// @address 0x4e2c78 ?get_focused_element_handle@c_gui_list_widget@@QAAJXZ
// @address 0x4e2d20 ?set_focused_element_handle@c_gui_list_widget@@QAA_NJ_N@Z
// @address 0x4e2eb8 ?get_element_handle_from_list_item_index@c_gui_list_widget@@QAAJJ@Z
// @address 0x4e2958 ?list_has_more_elements_following@c_gui_list_widget@@QAA_NXZ
// @address 0x4e2118 ?handle_tab_direction@c_gui_list_widget@@AAA_NPAVc_gui_list_item_widget@@W4e_list_scroll_direction@1@@Z
// @address 0x4e3188 ?dispose_submenu@c_gui_list_widget@@QAAXPAV1@@Z
// @address 0x484f88 ?user_interface_free@@YAXPBX@Z
// @address 0x48a3b0 gui_widget_create_and_add_child_list_widget
// @address 0x48b808 ?get_authored_bounds@c_gui_widget@@QAAPATreal_rectangle2d@@PAT2@@Z
// @address 0x489570 ??0c_gui_widget@@QAA@W4e_gui_widget_type@@@Z
// @address 0x48e0a8 ??0s_bitmap_widget_block@@QAA@XZ
// @address 0x489bf8 ?add_definition_fields@c_gui_widget@@SAXW4e_gui_widget_type@@PBUs_core_widget_definition@@PAUs_runtime_core_widget_definition@@PBTreal_rectangle2d@@_N@Z
// @address 0x48a288 ?create_and_add_child_bitmap_widget@c_gui_widget@@QAAPAVc_gui_bitmap_widget@@PBUs_bitmap_widget_block@@@Z
// @address 0x4865f8 ?get_data@c_gui_screen_widget@@QAAPAVc_gui_data@@JPAJ@Z
// @address 0x4896b0 ?post_initialize@c_gui_widget@@UAAXXZ
// @address 0x48b228 ?set_full_animation_state@c_gui_widget@@QAAXPBUs_animation_transform@@_N@Z
// @address 0x489840 ?set_visible@c_gui_widget@@QAAX_N@Z
// @address 0x48a580 ?update_render_state@c_gui_widget@@UAAXK@Z
// @address 0x48ac58 ?animate_recursively@c_gui_widget@@IAAXK@Z
// @address 0x48b648 ?get_parent_screen@c_gui_widget@@QAAPAVc_gui_screen_widget@@XZ
// @address 0x48baf0 ?handle_tab@c_gui_widget@@UAA_NPBVc_controller_input_message@@@Z
// @address 0x4e1cb8 ?handle_tab@c_gui_list_widget@@UAA_NPBVc_controller_input_message@@@Z
// @address 0x48b098 ?start_animation_at_time@c_gui_widget@@QAAXW4e_animation_state@@K_N@Z
// @address 0x48bf08 ?get_deepest_widget_that_can_receive_focus@c_gui_widget@@QAAPAV1@XZ
// @address 0x4e2fd8 ?invoke_submenu@c_gui_list_widget@@QAA_NPBUs_list_widget_block@@J_N@Z
// @address 0x4e1668 ?contains_submenu_control_definition@c_gui_group_widget@@QAA_NJPAUs_list_widget_block@@@Z
// @address 0x4e2f58 ?invoke_submenu@c_gui_list_widget@@QAA_NJJ_N@Z
// @address 0x4e2a28 ?get_datasource_index@c_gui_list_widget@@UAAJXZ
// @address 0x4e34a0 ?offset_horizontal_list_item_indicators@c_gui_list_widget@@AAAXXZ
// @address 0x4e3600 ??0s_list_widget_block@@QAA@XZ
// @address 0x48e160 ??0s_core_widget_definition@@QAA@XZ
// @address 0x189960 ??0c_font_cache@@QAA@XZ
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x1f1358 ?character_data_get_hardware_block_index@c_font_cache@@UAAJW4e_character_data_index@@@Z
// @address 0x1f13c0 ?character_data_set_hardware_block_index@c_font_cache@@UAAXW4e_character_data_index@@J@Z
// @address 0x18aeb8 ?character_data_touch@c_font_cache@@UAAXW4e_character_data_index@@@Z
// @address 0x1f1428 ?get_pixels@c_font_cache@@UAA_NW4e_font_id@@Uutf32@@PBUs_font_character@@PAG@Z
// @address 0x1f14e0 ?font_character_predict@c_font_cache@@UAA_NW4e_font_id@@Uutf32@@@Z
// @address 0x1f14a0 ?get_character@c_font_cache@@UAA_NW4e_font_id@@Uutf32@@_NPAKPAJPAPBUs_font_character@@@Z

template <class T> __forceinline const T &list_maximum(const T &a, const T &b) { return a > b ? a : b; }
template <class T> __forceinline const T &list_minimum(const T &a, const T &b) { return a < b ? a : b; }
template <class T> __forceinline const T &list_widget_minimum(const T &a, const T &b) { return a > b ? b : a; }

// @address 0x4e1af8 ?post_initialize@c_gui_list_widget@@UAAXXZ
void c_gui_list_widget::post_initialize()
{
    this->c_gui_widget::post_initialize();
}
// @address 0x4e1b00 ?update@c_gui_list_widget@@UAAXK@Z
void c_gui_list_widget::update(unsigned long current_milliseconds)
{
    this->c_gui_widget::update(current_milliseconds);
    long selectable_item_count = this->get_selectable_item_count();
    if (this->m_focused_item_index >= selectable_item_count) {
        c_gui_screen_widget *screen = this->get_parent_screen();
        if (screen) {
            long count = this->get_selectable_item_count();
            (screen)
                ->transfer_focus_to_list(this,
                    (this)->get_element_handle_from_list_item_index(count - 1), false, false);
        }
    }
    bool has_previous = this->list_has_more_elements_preceeding();
    bool has_following = this->list_has_more_elements_following();
    if (this->m_more_items_preceeding_indicator != 0 && (this->m_last_scroll_direction == 2 || this->m_last_scroll_direction == 4)) {
        ((c_gui_widget *)this->m_more_items_preceeding_indicator)->start_animation_at_time((e_animation_state)(20 + !has_previous), (unsigned long)-1, false);
    }
    if (this->m_more_items_following_indicator != 0 && (this->m_last_scroll_direction == 1 || this->m_last_scroll_direction == 3)) {
        ((c_gui_widget *)this->m_more_items_following_indicator)->start_animation_at_time((e_animation_state)(20 + !has_following), (unsigned long)-1, false);
    }
    this->m_last_scroll_direction = 0;
    if (this->m_is_submenu_that_needs_disposal) {
        c_gui_list_widget *submenu = gui_widget_get_parent_list_widget(this);
        c_gui_screen_widget *submenu_screen = ((c_gui_widget *)submenu)->get_parent_screen();
        submenu_screen->transfer_focus_without_animations((c_gui_widget *)submenu->m_submenu_owner_list_item, false, true);
        submenu->m_submenu_owner_list_item = 0;
        c_gui_widget *parent = (c_gui_widget *)robust_pointer_get_repaired_value(&this->m_parent);
        gui_widget_remove_child(parent, (c_gui_widget *)this);
        this->~c_gui_list_widget();
        user_interface_free(this);
        submenu->animate_recursively(submenu->m_last_animated_milliseconds);
    }
}

// @address 0x4e1c80 ?update_render_state@c_gui_list_widget@@UAAXK@Z
void c_gui_list_widget::update_render_state(unsigned long current_milliseconds)
{
    this->c_gui_widget::update_render_state(current_milliseconds);
    offset_horizontal_list_item_indicators();
}

// Grid navigation for a list widget: move the focused item one row or column in a direction
// (1 up, 2 left, 3 down, 4 right), wrapping when the widget's wrap flag is set; returns whether
// the focus moved.
// @address 0x4e1e70 ?handle_grid_tab@c_gui_list_widget@@AAA_NPAVc_gui_list_item_widget@@W4e_event_type@@@Z
bool c_gui_list_widget::handle_grid_tab(c_gui_list_item_widget *item, e_event_type event_type)
{
    bool result = false;
    int total = get_item_count(1);
    int count = get_item_count(0);
    int columns = total / (int)this->m_definition.rows;
    if (count > 1 && total % (int)this->m_definition.rows == 0) {
        int row = item->get_list_item_index() / columns;
        int column = item->get_list_item_index() % columns;
        int limit;
        if ((c_gui_data *)((void *)this->get_data())) {
            c_gui_data *source = (c_gui_data *)((void *)this->get_data());
            limit = (int)source->get_current_item_count();
        } else {
            limit = 0;
        }
        switch (event_type) {
        case 1: row--; break;
        case 3: row++; break;
        case 2: column--; break;
        case 4: column++; break;
        }
        bool wrap = (_rotl(this->m_definition.flags, 32 - 5) & 1) != 0;
        if (wrap) {
            int maximum_row = count / columns + 1;
            int row_length = (count >= columns) ? columns : count;
            if (row < 0 || row >= maximum_row || column < 0 || column >= row_length
                || row * columns + column < 0 || row * columns + column >= count) {
                switch (event_type) {
                case 1:
                    row = maximum_row - 1;
                    if (row * columns + column >= count)
                        row--;
                    break;
                case 3:
                    row = 0;
                    break;
                case 2:
                    column = row_length - 1;
                    if (row * columns + column >= count)
                        column = count % columns - 1;
                    break;
                case 4:
                    column = 0;
                    break;
                }
            }
        }
        int index = row * columns + (int)this->m_scroll_position + column;
        if (event_type == 3 && index >= limit) {
            int last = limit - 1;
            if (index < (last / columns + 1) * columns)
                index = last;
        }
        if (item->get_list_item_index() + (int)this->m_scroll_position != index && column >= 0 && column < columns
            && (c_gui_data *)((void *)this->get_data()) && index >= 0 && index < limit) {
            set_focused_item_index(index, 1);
            result = true;
        }
    }
    return result;
}

// @address 0x4e2680 ?create_and_add_additional_items_indicators_bitmaps@c_gui_list_widget@@QAAXXZ
void c_gui_list_widget::create_and_add_additional_items_indicators_bitmaps()
{
    s_bitmap_widget_block block;
    memset(&block, 0, sizeof(block));
    block.bitmap_widget_template.group = 'bmp3';
    block.bitmap_widget_template.index = -1;
    if (this->m_definition.more_items_preceeding_bitmap_index != -1) {
        memcpy(&block.definition, tag_get('bmp3', this->m_definition.more_items_preceeding_bitmap_index), sizeof(block.definition));
        this->m_more_items_preceeding_indicator = this->create_and_add_child_bitmap_widget(&block);
    }
    if (this->m_definition.more_items_following_bitmap_index != -1) {
        memcpy(&block.definition, tag_get('bmp3', this->m_definition.more_items_following_bitmap_index), sizeof(block.definition));
        this->m_more_items_following_indicator = this->create_and_add_child_bitmap_widget(&block);
    }
    if (this->m_more_items_preceeding_indicator != 0)
        ((c_gui_widget *)this->m_more_items_preceeding_indicator)->set_visible(false);
    if (this->m_more_items_following_indicator != 0)
        ((c_gui_widget *)this->m_more_items_following_indicator)->set_visible(false);
}

// @address 0x4e2788 ?get_item_count@c_gui_list_widget@@QAAJ_N@Z
long c_gui_list_widget::get_item_count(bool include_unindexed)
{
    int count = 0;
    c_gui_widget *object = (c_gui_widget *)gui_widget_find_child_by_name(this, _gui_list_item_widget);
    while (object) {
        if (include_unindexed || object->get_element_handle() != -1) count++;
        c_gui_widget *next = (c_gui_widget *)gui_widget_get_next_sibling_of_type((c_gui_widget *)object, _gui_list_item_widget);
        if (next && !include_unindexed && next->get_element_handle() == -1) next = 0;
        object = next;
    }
    return count;
}

// @address 0x4e2830 ?get_selectable_item_count@c_gui_list_widget@@QAAJXZ
long c_gui_list_widget::get_selectable_item_count()
{
    long selectable_item_count = 0;
    c_gui_data *data = (c_gui_data *)((void *)this->get_data());
    if (data) {
        if (this->m_selectable_item_cap_count == -1)
            selectable_item_count = data->get_current_item_count();
        else
            selectable_item_count = list_widget_minimum(data->get_current_item_count(), this->m_selectable_item_cap_count);
    }
    return selectable_item_count;
}

// @address 0x4e28b8 ?list_has_more_elements_preceeding@c_gui_list_widget@@QAA_NXZ
bool c_gui_list_widget::list_has_more_elements_preceeding()
{
    c_gui_widget *item = (c_gui_widget *)gui_widget_find_child_by_name(this, _gui_list_item_widget);
    c_gui_data *data = (c_gui_data *)((void *)this->get_data());
    long index = item ? item->get_element_handle() : -1;
    return data && data->get_previous_element_handle(index) != -1;
}

__forceinline c_gui_widget *c_gui_list_widget_next_valid_item(c_gui_widget *item)
{
    c_gui_widget *next = (c_gui_widget *)gui_widget_get_next_sibling_of_type((c_gui_widget *)item, _gui_list_item_widget);
    if (next && next->get_element_handle() == -1)
        next = 0;
    return next;
}
bool c_gui_list_widget::list_has_more_elements_following()
{
    bool result = false;
    c_gui_widget *last_item = 0;
    c_gui_data *data = (c_gui_data *)((void *)this->get_data());
    for (c_gui_widget *item = (c_gui_widget *)gui_widget_find_child_by_name(this, _gui_list_item_widget);
        item != 0; item = c_gui_list_widget_next_valid_item(item))
        last_item = item;
    if (last_item != 0 && data != 0)
        result = data->get_next_element_handle(last_item->get_element_handle()) != -1;
    return result;
}

// The widget's datasource index resolved through its parent screen, or -1.
// @address 0x4e2a28 ?get_datasource_index@c_gui_list_widget@@UAAJXZ
long c_gui_list_widget::get_datasource_index()
{
    int value = -1;
    c_gui_screen_widget *screen = this->get_parent_screen();
    do {
        if (screen == 0)
            break;
        if (screen->get_data(this->m_datasource_name, (long *)&value) == 0)
            break;
        return value;
    } while (false);
    return -1;
}

class c_gui_data * c_gui_list_widget::get_data(void)
{
    int value = -1;
    c_gui_screen_widget *screen = this->get_parent_screen();
    if (screen == 0)
        return 0;
    return screen->get_data(this->m_datasource_name, (long *)&value);
}

// @address 0x4e2af8 ?set_focused_item_index@c_gui_list_widget@@QAA_NJ_N@Z
bool c_gui_list_widget::set_focused_item_index(long index, bool transfer_focus)
{
    bool result = false;
    c_gui_data *data = (c_gui_data *)((void *)this->get_data());
    if (data != 0) {
        long original_index = index;
        index = list_minimum(list_maximum(index, 0L), list_maximum(get_selectable_item_count() - 1, 0L));
        result = (original_index == index);
        long total_item_count = get_item_count(1);
        long visible_item_count = get_item_count(0);
        bool changed = (this->m_focused_item_index != index);
        long items_per_row = this->m_definition.rows > 0 ? total_item_count / this->m_definition.rows : 1;
        this->m_focused_item_index = index;
        if (visible_item_count == 0) {
            this->m_scroll_position = 0;
        } else if (index < this->m_scroll_position) {
            this->m_scroll_position = (index / items_per_row) * items_per_row;
        } else if (index >= this->m_scroll_position + visible_item_count) {
            this->m_scroll_position = ((index - visible_item_count + items_per_row) / items_per_row) * items_per_row;
        }
        if (changed && transfer_focus) {
            c_gui_screen_widget *screen = this->get_parent_screen();
            if (screen != 0)
                (screen)
                    ->transfer_focus_to_list(
                        this, (this)->get_focused_element_handle(), true, true);
        }
    }
    return result;
}

long c_gui_list_widget::get_focused_element_handle(void)
{
    c_gui_data *data = get_data();
    long element_handle = -1;
    if (data) {
        element_handle = data->get_first_element_handle();
        long count = list_widget_minimum(this->m_focused_item_index, data->get_current_item_count());
        for (long i = 0; i < count; i++)
            element_handle = data->get_next_element_handle(element_handle);
    }
    return element_handle;
}

bool c_gui_list_widget::set_focused_element_handle(long element_handle, bool animate)
{
    bool result = false;
    c_gui_data *data = get_data();
    if (data) {
        long candidate = data->get_first_element_handle();
        for (long index = 0; index < data->get_current_item_count(); index++) {
            if (element_handle == candidate) {
                result = set_focused_item_index(index, animate);
                break;
            }
            candidate = data->get_next_element_handle(candidate);
        }
    }
    return result;
}

// @address 0x4e2de8 ?set_focused_datasource_element_from_value@c_gui_list_widget@@QAA_NJJ_N@Z
bool c_gui_list_widget::set_focused_datasource_element_from_value(long value_name, long value, bool focused)
{
    c_gui_data *data = (c_gui_data *)((void *)this->get_data());
    bool result = false;
    if (data) {
        for (long element_handle = data->get_first_element_handle(); element_handle != -1;
             element_handle = data->get_next_element_handle(element_handle)) {
            long element_value;
            if (data->get_integer_value(element_handle, value_name, &element_value) && element_value == value) {
                (this)->set_focused_element_handle(element_handle, focused);
                result = true;
                break;
            }
        }
    }
    return result;
}

long c_gui_list_widget::get_element_handle_from_list_item_index(long list_item_index)
{
    long element_handle = -1;
    c_gui_data *data = get_data();
    list_item_index += this->m_scroll_position;
    if (data != 0 && list_item_index < data->get_current_item_count()) {
        element_handle = data->get_first_element_handle();
        for (long i = 0; i < list_item_index; i++)
            element_handle = data->get_next_element_handle(element_handle);
    }
    return element_handle;
}

// Open the submenu list widget described by a tag block under the focused widget of the
// owning screen, move focus into it, and animate the siblings out.
bool c_gui_list_widget::invoke_submenu(const s_list_widget_block *block, long datasource_name, bool transfer_focus)
{
    bool result = false;
    c_gui_screen_widget *screen = this->get_parent_screen();
    c_gui_widget *focused = 0;
    if (!screen->m_suppress_focus)
        focused = screen->m_focused_widget;
    c_gui_list_widget *submenu = (c_gui_list_widget *)gui_widget_create_and_add_child_list_widget(focused, block);
    if (submenu) {
        void *data;
        if (datasource_name != -1 && datasource_name != 0) {
            long value;
            c_gui_screen_widget *data_screen = this->get_parent_screen();
            data = data_screen->get_data(datasource_name, &value);
            if (data)
                submenu->m_datasource_name = datasource_name;
        } else {
            data = ((void *)((c_gui_list_widget *)submenu)->get_data());
        }
        if (data) {
            c_gui_widget *item = (c_gui_widget *)((c_gui_widget *)submenu)->get_deepest_widget_that_can_receive_focus();
            if (item && item->type == 5 && robust_pointer_get_repaired_value(&item->m_parent) == (c_gui_widget *)submenu) {
                screen->transfer_focus_without_animations((c_gui_widget *)item, true, false);
                submenu->post_initialize();
                this->start_animation_at_time(_animation_state_load_submenu_focused, (unsigned long)-1, false);
                for (c_gui_widget *child = robust_pointer_get_repaired_value(&this->m_children); child;
                    child = robust_pointer_get_repaired_value(&child->m_next))
                    ((c_gui_widget *)child)->start_animation_at_time(
                        (e_animation_state)(_animation_state_load_submenu_focused + ((c_gui_widget *)child != focused)), (unsigned long)-1, true);
                ((c_gui_widget *)submenu)->start_animation_at_time(_animation_state_load_as_submenu, (unsigned long)-1, true);
                submenu->m_is_submenu = true;
                this->m_submenu_owner_list_item = (c_gui_list_item_widget *)focused;
                screen->submenu_invoked(submenu);
                result = true;
            } else {
                dispose_submenu(submenu);
            }
        } else {
            dispose_submenu(submenu);
        }
    }
    return result;
}

void c_gui_list_widget::dispose_submenu(c_gui_list_widget *object)
{
    void *parent = robust_pointer_get_repaired_value(&object->m_parent);
    gui_widget_remove_child((c_gui_widget *)parent, (c_gui_widget *)object);
    static_cast<c_gui_widget *>(object)->~c_gui_widget();
    user_interface_free(object);
}

// @address 0x4e31e0 ?get_child_list_item_widget_bounds@c_gui_list_widget@@QAAXPATreal_rectangle2d@@@Z
void c_gui_list_widget::get_child_list_item_widget_bounds(real_rectangle2d *bounds)
{
    memset(bounds, 0, sizeof(*bounds));
    for (c_gui_widget *list_item = robust_pointer_get_repaired_value(&this->m_children); list_item;
        list_item = robust_pointer_get_repaired_value(&list_item->m_next))
    {
        if (list_item->type == 5) {
            for (c_gui_widget *child = robust_pointer_get_repaired_value(&list_item->m_children); child;
                child = robust_pointer_get_repaired_value(&child->m_next))
            {
                real_rectangle2d child_bounds;
                child->get_container_current_bounds(&child_bounds);
                bounds->x0 = bounds->x0 > child_bounds.x0 ? child_bounds.x0 : bounds->x0;
                bounds->x1 = bounds->x1 > child_bounds.x1 ? bounds->x1 : child_bounds.x1;
                bounds->y0 = bounds->y0 > child_bounds.y0 ? child_bounds.y0 : bounds->y0;
                bounds->y1 = bounds->y1 > child_bounds.y1 ? bounds->y1 : child_bounds.y1;
            }
        }
    }
}

// @address 0x4e3350 ?get_child_list_item_text_bounds@c_gui_list_widget@@QAAXPAVc_font_cache@@PATrectangle2d@@PA_N@Z
// match: 0x4e3350: pointer __restrict qualifier prevents alias reload
void c_gui_list_widget::get_child_list_item_text_bounds(c_font_cache *font_cache, rectangle2d *__restrict bounds, bool *all_text_precached)
{
    bounds->x0 = 32767;
    bounds->y0 = 32767;
    bounds->x1 = -32768;
    bounds->y1 = -32768;
    *all_text_precached = true;
    for (c_gui_widget *list_item = robust_pointer_get_repaired_value(&this->m_children); list_item;
        list_item = robust_pointer_get_repaired_value(&list_item->m_next))
    {
        if (list_item->type == 5) {
            for (c_gui_widget *child = robust_pointer_get_repaired_value(&list_item->m_children); child;
                child = robust_pointer_get_repaired_value(&child->m_next))
            {
                if (child->type == 0) {
                    c_gui_widget *text_widget = child;
                    rectangle2d text_bounds;
                    if (((c_gui_text_widget *)text_widget)->get_text_bounds(font_cache, &text_bounds)) {
                        bounds->x0 = bounds->x0 > text_bounds.x0 ? text_bounds.x0 : bounds->x0;
                        bounds->x1 = bounds->x1 > text_bounds.x1 ? bounds->x1 : text_bounds.x1;
                        bounds->y0 = bounds->y0 > text_bounds.y0 ? text_bounds.y0 : bounds->y0;
                        bounds->y1 = bounds->y1 > text_bounds.y1 ? bounds->y1 : text_bounds.y1;
                    } else {
                        *all_text_precached = false;
                    }
                }
            }
        }
    }
}

void c_gui_list_widget::offset_horizontal_list_item_indicators()
{
    c_gui_list_widget *self = this;
    if (self->m_more_items_following_indicator == 0 && self->m_more_items_preceeding_indicator == 0)
        return;
    bool update_indicators = (self->m_definition.flags & 16) != 0;
    if (update_indicators) {
        rectangle2d bounds;
        bounds.y0 = 0;
        bounds.x0 = 0;
        bounds.y1 = 0;
        bounds.x1 = 0;
        bool all_text_precached = false;
        c_font_cache font_cache;
        ((c_gui_list_widget *)self)->get_child_list_item_text_bounds(&font_cache, &bounds, &all_text_precached);
        bool visible = all_text_precached;
        if (self->m_more_items_preceeding_indicator != 0) {
            s_animation_transform state = *self->m_more_items_preceeding_indicator->get_animated_state();
            ((c_gui_widget *)self->m_more_items_preceeding_indicator)->set_full_animation_state((const s_animation_transform *)&state, false);
            ((c_gui_widget *)self->m_more_items_preceeding_indicator)->set_visible(visible);
        }
        if (self->m_more_items_following_indicator != 0) {
            s_animation_transform state = *self->m_more_items_following_indicator->get_animated_state();
            state.position.x += (float)(bounds.x1 - bounds.x0);
            ((c_gui_widget *)self->m_more_items_following_indicator)->set_full_animation_state((const s_animation_transform *)&state, false);
            ((c_gui_widget *)self->m_more_items_following_indicator)->set_visible(visible);
        }
        return;
    }
    if (self->m_more_items_preceeding_indicator != 0)
        ((c_gui_widget *)self->m_more_items_preceeding_indicator)->set_visible(true);
    if (self->m_more_items_following_indicator != 0)
        ((c_gui_widget *)self->m_more_items_following_indicator)->set_visible(true);
}

s_list_widget_block::s_list_widget_block()
{
    s_gui_tag_reference defaults(0x6c737433);
    list_widget_template = defaults;
}

void c_gui_group_widget::add_definition_fields(const s_group_widget_definition *definition, bool require_nonzero_bounds)
{
    c_gui_group_widget *self = this;
    real_rectangle2d parent_bounds;
    ((c_gui_widget *)robust_pointer_get_repaired_value(&((c_gui_widget *)self)->m_parent))->get_authored_bounds(&parent_bounds);
    c_gui_widget::add_definition_fields((e_gui_widget_type)((c_gui_widget *)self)->type, (const s_core_widget_definition *)definition,
        (s_runtime_core_widget_definition *)&self->m_definition, &parent_bounds, require_nonzero_bounds);
    if (definition->list_widgets.count > 0 && definition->list_widgets.address != 0) {
        self->m_definition.list_widgets = definition->list_widgets;
    }
    if (definition->text_widgets.count > 0 && definition->text_widgets.address != 0) {
        self->m_definition.text_widgets = definition->text_widgets;
    }
    if (definition->bitmap_widgets.count > 0 && definition->bitmap_widgets.address != 0) {
        self->m_definition.bitmap_widgets = definition->bitmap_widgets;
    }
    if (definition->model_widgets.count > 0 && definition->model_widgets.address != 0) {
        self->m_definition.model_widgets = definition->model_widgets;
    }
}

bool c_gui_list_widget::handle_tab_direction(c_gui_list_item_widget *item, e_list_scroll_direction direction)
{

    bool result = false;
    c_gui_screen_widget *screen = this->get_parent_screen();
    c_gui_data *data = (c_gui_data *)((void *)this->get_data());
    long total_item_count = get_item_count(1);
    long rows = m_definition.rows;
    long items_per_row = rows > 0 ? total_item_count / rows : 1;
    m_last_scroll_direction = direction;
    if (screen != 0 && data != 0) {
    long element_handle = -1;
    
    if (direction == _list_scroll_direction_up) {
        element_handle = data->get_next_element_handle(item->get_element_handle());
        if (element_handle == -1 && (_rotl(m_definition.flags, 32 - 5) & 1) != 0)
            element_handle = data->get_first_element_handle();
    } else if (direction == _list_scroll_direction_left) {
        element_handle = data->get_previous_element_handle(item->get_element_handle());
        if (element_handle == -1 && (_rotl(m_definition.flags, 32 - 5) & 1) != 0) {
            while (data->get_next_element_handle(element_handle) != -1)
                element_handle = data->get_next_element_handle(element_handle);
        }
    } else if (direction == _list_scroll_direction_down) {
        bool is_last_item = gui_widget_get_next_list_item((c_gui_widget *)item, true) == 0;
        bool in_last_row = false;
        long item_index = 0;
        for (c_gui_widget *widget = (c_gui_widget *)gui_widget_find_child_by_name(this, _gui_list_item_widget); widget != 0 && widget != (c_gui_widget *)item;
             widget = gui_widget_get_next_list_item(widget, false))
            item_index++;
        if (rows > 1)
            in_last_row = item_index >= (rows - 1) * items_per_row;
        long move_count;
        if (!is_last_item && !in_last_row) {
            long visible_item_count = get_item_count(0);
            long selectable_item_count = get_selectable_item_count();
            move_count = list_widget_minimum(selectable_item_count, visible_item_count) - item_index - 1;
        } else {
            move_count = get_item_count(1) - 1;
        }
        if (items_per_row > 1)
            move_count -= move_count % items_per_row;
        element_handle = item->get_element_handle();
        while (move_count > 0 && data->get_next_element_handle(element_handle) != -1) {
            element_handle = data->get_next_element_handle(element_handle);
            move_count--;
        }
    } else if (direction == _list_scroll_direction_right) {
        bool is_first_item = gui_widget_get_previous_list_item((c_gui_widget *)item, true) == 0;
        bool in_first_row = false;
        long item_index = 0;
        for (c_gui_widget *widget = (c_gui_widget *)gui_widget_find_child_by_name(this, _gui_list_item_widget); widget != 0 && widget != (c_gui_widget *)item;
             widget = gui_widget_get_next_list_item(widget, false))
            item_index++;
        if (rows > 1)
            in_first_row = item_index < items_per_row;
        if (is_first_item || in_first_row)
            item_index = get_item_count(1) - 1;
        if (items_per_row > 1)
            item_index -= item_index % items_per_row;
        element_handle = item->get_element_handle();
        while (item_index > 0 && data->get_previous_element_handle(element_handle) != -1) {
            element_handle = data->get_previous_element_handle(element_handle);
            item_index--;
        }
    }
    if (element_handle != -1 && element_handle != item->get_element_handle()
        && this->set_focused_element_handle(element_handle, false)) {
        (screen)->transfer_focus_to_list(this, element_handle, true, true);
        result = true;
    }
    }
    return result;
}

static __forceinline c_gui_widget *list_tab_focused_widget(c_gui_screen_widget *screen)
{
    c_gui_widget *result = 0;
    if (!screen->m_suppress_focus)
        result = screen->m_focused_widget;
    return result;
}
static __forceinline bool list_tab_scrolls_horizontally(c_gui_list_widget *list)
{
    return (bool)(_rotl(list->m_definition.flags, 28) & 1);
}

bool c_gui_list_widget::handle_tab(const c_controller_input_message *message)
{
    bool handled = false;
    c_gui_screen_widget *screen = this->get_parent_screen();
    c_gui_widget *focused = list_tab_focused_widget(screen);
    if ((void *)m_submenu_owner_list_item == 0 && get_enabled() && focused != 0 &&
        focused->m_type == 5 && gui_widget_get_parent_list_widget(focused) == this)
    {
        c_gui_list_item_widget *item = (c_gui_list_item_widget *)focused;
        if (m_definition.rows == 0 || m_definition.rows == 1) {
            int event = message->get_event_type();
            int component = message->get_component();
            if (event == 13 && !list_tab_scrolls_horizontally(this) && (component == 6 || component == 7)) {
                handled = handle_tab_direction(item, (e_list_scroll_direction)((component - 6 == 0) + 3));
            } else if (!list_tab_scrolls_horizontally(this)) {
                switch (event) {
                case 1: handled = handle_tab_direction(item, _list_scroll_direction_left); break;
                case 3: handled = handle_tab_direction(item, _list_scroll_direction_up); break;
                }
            } else {
                switch (event) {
                case 2: handled = handle_tab_direction(item, _list_scroll_direction_left); break;
                case 4: handled = handle_tab_direction(item, _list_scroll_direction_up); break;
                }
            }
        } else {
            if (!list_tab_scrolls_horizontally(this)) {
                int event = message->get_event_type();
                int component = message->get_component();
                if (event == 13 && (component == 6 || component == 7))
                    handle_tab_direction(item, (component == 6) ? _list_scroll_direction_right : _list_scroll_direction_down);
            }
            handled = handle_grid_tab(item, message->get_event_type());
        }
    }
    if (!handled) return c_gui_widget::handle_tab(message);
    return handled;
}

bool c_gui_list_widget::invoke_submenu(long submenu_name, long datasource_name, bool transfer_focus)
{
    bool result = false;
    c_gui_group_widget *group = (c_gui_group_widget *)gui_widget_get_parent_group_widget(this);
    s_list_widget_block submenu_definition;
    if (group != 0 && group->contains_submenu_control_definition(submenu_name, &submenu_definition))
        result = invoke_submenu(&submenu_definition, datasource_name, transfer_focus);
    return result;
}

void c_gui_list_widget::initialize(const s_list_widget_block *list_widget_block)
{
    if (list_widget_block != 0) {
        bool use_defaults = false;
        if (list_widget_block->list_widget_template.index != -1) {
            add_definition_fields(
                    (const s_list_widget_definition *)tag_get('lst3', list_widget_block->list_widget_template.index),
                    use_defaults);
            use_defaults = true;
        }
        add_definition_fields((const s_list_widget_definition *)&list_widget_block->definition.core, use_defaults);
    }
    this->m_name = this->m_definition.name;
    this->create_and_add_child_list_item_widgets((const s_tag_block *)&this->m_definition.list_items, this->m_definition.skin_index);
    create_and_add_additional_items_indicators_bitmaps();
    c_gui_widget::initialize();
}

// @address 0x4e1958 ??0c_gui_list_widget@@QAA@XZ
c_gui_list_widget::c_gui_list_widget()
    : c_gui_widget(_gui_list_widget), m_definition(), m_datasource_name(-1), m_more_items_preceeding_indicator(0),
      m_more_items_following_indicator(0), m_focused_item_index(0), m_last_scroll_direction(0), m_scroll_position(0),
      m_selectable_item_cap_count(-1), m_submenu_owner_list_item(0), m_is_submenu(false), m_is_submenu_that_needs_disposal(false)
{
}
