// interfacelibxbox/gui_start_menu_hq_transfers.obj: from the records 0x4af9a0 0x4b81d8 0x4b81e0 0x4b8258 0x4b8360 0x4b83d0 0x4b8420 0x4b8470 0x4b8560 0x4b8600 0x4b89c8 0x4b8b48 0x4b8c18
#include "saved_games/saved_game_item_metadata.h"
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "saved_games/saved_game_files.h"
// @address 0x51ee8 ??_7c_active_transfers_datasource@@6B@
// @address 0x4b89c8 ?update_render_state@c_gui_start_menu_hq_transfers@@UAAXK@Z
#include "interface/gui_widget_tree.h"
#include "interface/gui_screen_widget_legacy.h"
// @address 0x485230 ?initialize_datasource@c_gui_screen_widget@@UAAXXZ
#include "interface/gui_widget_types.h"
#include "game/map_variant.h"
#include "interface/user_interface_messages.h"
#include "networking/online/online_file_work_item_types.h"
// @address 0x4b8258 ?get_column_names@c_active_transfers_datasource@@UAAXQAJPAJ@Z
// @address 0x4b8470 ?get_string_id_value@c_active_transfers_datasource@@UAA_NJJPAJ@Z
// @address 0x4b83d0 ?get_qword_value@c_active_transfers_datasource@@UAA_NJJPA_K@Z
// @address 0x4b8420 ?get_real_value@c_active_transfers_datasource@@UAA_NJJPAM@Z
// @address 0x4b8360 ?get_integer_value@c_active_transfers_datasource@@UAA_NJJPAJ@Z
// @address 0x48a580 ?update_render_state@c_gui_widget@@UAAXK@Z
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x489840 ?set_visible@c_gui_widget@@QAAX_N@Z
// @address 0x48b228 ?set_full_animation_state@c_gui_widget@@QAAXPBUs_animation_transform@@_N@Z
// @address 0x48c070 gui_widget_find_child_by_name
// @address 0x48c1e8 gui_widget_get_next_list_item
// @address 0x3b7a18 online_files_is_transfer_ready
// @address 0x3bb638 ?get_transfer_status@c_online_files_active_transfer_tracker@@QBA_NJPAUs_queued_work_item_status@@@Z
// @address 0x48c258 gui_widget_set_alternate_state
// @address 0x492858 gui_start_menu_hq_service_record_file_share_set_map_image
// @address 0x4e8828 ?update@c_start_menu_pane_screen_widget@@UAAXK@Z
// @address 0x4b8600 ?update@c_gui_start_menu_hq_transfers@@UAAXK@Z
// @address 0x483240 ??0c_load_screen_message@@QAA@JW4e_controller_index@@W4e_window_index@@J@Z
// @address 0x486578 ?get_render_window@c_gui_screen_widget@@QAA?AW4e_window_index@@XZ
// @address 0x846a8 ?internal_critical_section_enter@@YAXJ@Z
// @address 0x847d0 ?internal_critical_section_leave@@YAXJ@Z
// @address 0x484f88 ?user_interface_free@@YAXPBX@Z
// @address 0x484520 ?user_interface_get_percentage_string@@YA_NMPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
#include <ppcintrinsics.h>
#include <math.h>
#include "interface/gui_data.h"
#include <stdlib.h>
#include "multithreading/synchronization.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_allocation.h"
#include "text/font_cache.h"
#include "interface/queued_work_item_status_state_types.h"
#include "interface/c_gui_widget.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share.h"
#include "networking/online/online_files.h"
#include "interface/c_deletable.h"
#include "interface/user_interface_utilities.h"


class c_gui_list_widget;
template<long k_maximum_count> class c_static_wchar_string;



struct s_task;
#define SLOT(n) virtual void slot##n()=0;
class c_gui_list_item_widget;
class c_gui_screen_widget;
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers_item_selected.h"
#undef SLOT
#include <new>
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_list_item_widget.h"



// 0x4af9a0
void c_load_transfers_item_selected_screen_message::apply_initial_state(c_gui_screen_widget *screen_widget) const
{
    c_gui_start_menu_hq_transfers_item_selected *screen = (c_gui_start_menu_hq_transfers_item_selected *)screen_widget;
    screen->setup(m_name, &m_status, m_enabled);
}

// 0x4b81d8
long c_active_transfers_datasource::get_current_item_count_internal()
{
    return m_transfer_count;
}

class c_online_file_manager;



void c_active_transfers_datasource::update()
{
    const c_online_files_active_transfer_tracker *transfers = c_online_file_manager::get()->get_active_transfers();
    if (transfers) {
        m_transfer_count = 0;
        for (long index = 0; index < transfers->get_transfer_count(); ++index) {
            if (transfers->get_transfer_status(index, &m_transfers[m_transfer_count]))
                ++m_transfer_count;
        }
    }
}

void c_active_transfers_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _start_menu_string_name;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_description;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_progress;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_status;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_type;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_author;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_date;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_length;
    ++*column_count;
    column_names[*column_count] = _start_menu_string_size;
    ++*column_count;
}


bool c_active_transfers_datasource::get_integer_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_transfer_count)
    {
        s_queued_work_item_status *transfer = &m_transfers[element_handle];
        if (transfer)
        {
            if (value_name == _start_menu_string_sort_order)
            {
                *value = transfer->priority;
                result = true;
            }
            if (!result)
                return saved_game_files_try_and_get_metadata_property(
                    &transfer->description.metadata, value_name, value);
        }
    }
    return result;
}

bool c_active_transfers_datasource::get_qword_value(long element_handle, long value_name, unsigned __int64 *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_transfer_count)
    {
        s_queued_work_item_status *transfer = &m_transfers[element_handle];
        if (transfer)
        {
            if (value_name == _start_menu_string_date)
            {
                *value = transfer->description.metadata.date;
                result = true;
            }
        }
    }
    return result;
}

bool c_active_transfers_datasource::get_real_value(long element_handle, long value_name, float *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_transfer_count)
    {
        s_queued_work_item_status *transfer = &m_transfers[element_handle];
        if (transfer)
        {
            if (value_name == _start_menu_string_progress)
            {
                *value = transfer->progress;
                result = true;
            }
        }
    }
    return result;
}

bool c_active_transfers_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_transfer_count)
    {
        s_queued_work_item_status *transfer = &m_transfers[element_handle];
        if (transfer)
        {
            switch (value_name)
            {
            case _start_menu_string_status:
                *value = -1;
                switch (transfer->state)
                {
                case 1: *value = _start_menu_string_upload_pending; break;
                case 2: *value = _start_menu_string_preparing_before_upload; break;
                case 3:
                case 4: *value = _start_menu_string_uploading; break;
                case 5: *value = _start_menu_string_download_pending; break;
                case 6: *value = _start_menu_string_downloading; break;
                case 7:
                case 8: *value = _start_menu_string_preparing_after_download; break;
                }
                result = *value != -1;
                break;
            }
        }
    }
    return result;
}



bool c_active_transfers_datasource::get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value)
{
    bool result = false;
    if (element_handle >= 0 && element_handle < m_transfer_count)
    {
        s_queued_work_item_status *transfer = &m_transfers[element_handle];
        if (transfer)
        {
            if (value_name == _start_menu_string_progress)
            {
                if (transfer->progress > 0.0f)
                    result = user_interface_get_percentage_string(transfer->progress, value);
                else
                    result = false;
            }
            if (!result)
                result = transfer->description.metadata.get_metadata_text(value_name, value);
        }
    }
    return result;
}

void c_gui_start_menu_hq_transfers::update_render_state(unsigned long current_milliseconds)
{
    c_gui_widget::update_render_state(current_milliseconds);
    c_gui_widget *list_widget = get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items);
    if (list_widget == 0)
        return;
    c_gui_data *data = ((c_gui_list_widget *)list_widget)->get_data();
    if (data == 0)
        return;
    for (c_gui_widget *item = gui_widget_find_child_by_name(list_widget, _gui_list_item_widget); item != 0; item = gui_widget_get_next_list_item(item, true)) {
        float scale = 0.0f;
        if (data->get_real_value(item->get_element_handle(), _start_menu_string_progress, &scale)) {
            c_gui_widget *animated = item->get_child_widget(_gui_bitmap_widget, _start_menu_string_progress);
            item->set_child_visible(_gui_bitmap_widget, _start_menu_string_progress_border, scale > 0.0f);
            if (animated != 0) {
                s_animation_transform state = animated->m_animated_state;
                state.scale.i = state.scale.i * scale;
                animated->set_full_animation_state(&state, false);
                animated->set_visible(scale > 0.0f);
            }
        }
    }
}
// @address 0x4b8b48 ?initialize_datasource@c_gui_start_menu_hq_transfers@@UAAXXZ
void c_gui_start_menu_hq_transfers::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    void *memory = user_interface_malloc_tracked(sizeof(c_active_transfers_datasource), 0, 0);
    if (memory != 0) {
        c_active_transfers_datasource *task = new (memory) c_active_transfers_datasource();
        if (task->initialize(_start_menu_string_transfers)) {
            add_datasource(task);
            return;
        }
        task->~c_active_transfers_datasource();
        user_interface_free(task);
    }
}


// @address 0x4b2e50 ?create_bitmap_widget@c_gui_start_menu_hq_transfers@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *c_gui_start_menu_hq_transfers::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _start_menu_string_game_variant)
        return new(0, 0, _ui_allocation_marker_dummy) c_gui_custom_bitmap_widget();
    return new(0, 0, _ui_allocation_marker_dummy) c_gui_bitmap_widget();
}
// @address 0x515e4 ??_7c_load_transfers_item_selected_screen_message@@6B@

static __forceinline bool gui_start_menu_hq_transfers_is_in_progress(long state)
{
    return state == 1 || state == 2 || state == 3 || state == 4;
}
static __forceinline void gui_start_menu_hq_transfers_set_child_flag(c_gui_widget *w, e_gui_widget_type type, long name, bool value)
{
    c_gui_widget *child = w->get_child_widget(type, name);
    if (child != 0)
        gui_widget_set_alternate_state(child, value);
}

void c_gui_start_menu_hq_transfers::update(unsigned long current_milliseconds)
{
    c_start_menu_pane_screen_widget::update(current_milliseconds);
    c_gui_widget *screen = this;
    c_gui_widget *list = screen->get_child_widget(_gui_list_widget, _start_menu_string_start_menu_items);
    c_gui_data *data;
    if (list != 0)
        data = ((c_gui_list_widget *)list)->get_data();
    else
        data = 0;
    const c_online_files_active_transfer_tracker *transfers = c_online_file_manager::get()->get_active_transfers();
    screen->set_child_visible(_gui_group_widget, _start_menu_string_empty, transfers->get_transfer_count() == 0);
    screen->set_child_visible(_gui_text_widget, _start_menu_string_empty, transfers->get_transfer_count() == 0);
    screen->set_child_visible(_gui_group_widget, _start_menu_string_pause,
        transfers->get_transfer_count() > 0 && online_files_is_transfer_ready(((char *)&g_online_file_manager)) == 0);
    screen->set_child_visible(_gui_text_widget, _start_menu_string_pause,
        transfers->get_transfer_count() > 0 && online_files_is_transfer_ready(((char *)&g_online_file_manager)) == 0);
    screen->set_child_visible(_gui_group_widget, _start_menu_string_failed,
        transfers->get_transfer_count() > 0 && online_files_is_transfer_ready(((char *)&g_online_file_manager)) != 0
        && !c_online_file_manager::get()->m_status_known);
    screen->set_child_visible(_gui_text_widget, _start_menu_string_failed,
        transfers->get_transfer_count() > 0 && online_files_is_transfer_ready(((char *)&g_online_file_manager)) != 0
        && !c_online_file_manager::get()->m_status_known);
    if (list != 0 && data != 0) {
        c_gui_widget *selected_group = screen->get_child_widget(_gui_group_widget, _start_menu_string_file_info_block);
        for (c_gui_widget *item = gui_widget_find_child_by_name(list, _gui_list_item_widget); item != 0; item = gui_widget_get_next_list_item(item, true)) {
            c_gui_custom_bitmap_widget *progress_bitmap = (c_gui_custom_bitmap_widget *)item->get_child_widget(
                _gui_bitmap_widget, _start_menu_string_game_variant);
            s_queued_work_item_status status;
            bool valid = transfers->get_transfer_status(item->get_element_handle(), &status);
            item->set_child_visible(_gui_bitmap_widget, _start_menu_string_transfers, valid);
            long frame = valid && gui_start_menu_hq_transfers_is_in_progress(status.state);
            c_gui_bitmap_widget *icon = (c_gui_bitmap_widget *)item->get_child_widget(
                _gui_bitmap_widget, _start_menu_string_transfers);
            if (icon != 0)
                icon->set_sprite_frame(frame);
            gui_start_menu_hq_transfers_set_child_flag(item, _gui_bitmap_widget, _start_menu_string_transfers, valid && status.state != 1 && status.state != 5);
            if (progress_bitmap != 0)
                gui_start_menu_hq_service_record_file_share_set_map_image(progress_bitmap, data, item->get_element_handle(), 1);
        }
        if (selected_group != 0) {
            c_gui_custom_bitmap_widget *selected_bitmap = (c_gui_custom_bitmap_widget *)selected_group->get_child_widget(
                _gui_bitmap_widget, _start_menu_string_game_variant);
            if (selected_bitmap != 0)
                gui_start_menu_hq_service_record_file_share_set_map_image(
                    selected_bitmap, data, ((c_gui_list_widget *)list)->get_focused_element_handle(), 1);
        }
    }
}

namespace {
static __forceinline const char *get_transfer_name(const c_online_files_active_transfer_tracker *transfers, long index)
{
    internal_critical_section_enter(29);
    const char *name = transfers->m_transfers[index].filename;
    if (index >= 0)
        internal_critical_section_leave(29);
    return name;
}
}

bool c_gui_start_menu_hq_transfers::handle_list_item_chosen(const c_controller_input_message *input, long list_name,
                                                         c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    if (list_name == _start_menu_string_start_menu_items) {
        long transfer_index = item->get_element_handle();
        const c_online_files_active_transfer_tracker *transfers = c_online_file_manager::get()->get_active_transfers();
        s_queued_work_item_status status;
        if (transfer_index >= 0 && transfer_index < transfers->get_transfer_count() &&
            transfers->get_transfer_status(transfer_index, &status)) {
            void *storage = user_interface_malloc_tracked(sizeof(c_load_transfers_item_selected_screen_message), 0, 0);
            c_load_transfers_item_selected_screen_message *message = storage ? new (storage) c_load_transfers_item_selected_screen_message(
                input->get_controller(), get_render_window(), get_name(),
                get_transfer_name(transfers, transfer_index), &status, transfer_index == 0) : 0;
            if (message) {
                message->set_parent_screen_index(get_screen_index());
                ::user_interface_messaging_post(message);
            }
        }
        handled = true;
    }
    if (!handled)
        handled = false;
    return handled;
}
