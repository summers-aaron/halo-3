#ifndef INTERFACE_GUI_START_MENU_HQ_TRANSFERS_ITEM_SELECTED_H
#define INTERFACE_GUI_START_MENU_HQ_TRANSFERS_ITEM_SELECTED_H
#include "interface/gui_data.h"
#include "interface/user_interface_messages.h"
#include <string.h>
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_render_data_size_types.h"
#include "networking/online/online_file_work_item_types.h"
class c_load_transfers_item_selected_screen_message : public c_load_screen_message
{
public:
    __forceinline c_load_transfers_item_selected_screen_message(e_controller_index controller, e_window_index window, long screen_name,
        const char *name, const s_queued_work_item_status *transfer_status, bool first)
        : c_load_screen_message(0x10090, controller, window, screen_name)
    {
        m_status = *transfer_status;
        m_enabled = first;
        strncpy(m_name, name, 17);
    }
    virtual void apply_initial_state(c_gui_screen_widget *screen_widget) const;
    char m_name[17];
    s_queued_work_item_status m_status;
    bool m_enabled;
};

class c_gui_transfers_selected_item_datasource : public c_gui_ordered_data
{
public:
    __forceinline void *operator new(size_t, void *storage) { return storage; }
    __forceinline c_gui_transfers_selected_item_datasource(const char *name,
        const s_queued_work_item_status *transfer_status, bool enabled)
    {
        status = *transfer_status;
        has_secondary_item = enabled;
        item_count = 0;
        strncpy(filename, name, sizeof(filename));
    }
    virtual bool initialize(long parameter);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_string_id_value(long, long, long *);
    char filename[17];
    s_queued_work_item_status status;
    volatile unsigned char has_secondary_item;
    volatile long item_ids[8];
    volatile long item_count;
protected:
    virtual long get_current_item_count_internal();
};

#include "networking/online/online_file_manager.h"

class c_gui_start_menu_hq_transfers_item_selected : public c_gui_screen_widget
{
public:
    __forceinline c_gui_start_menu_hq_transfers_item_selected(long name) : c_gui_screen_widget(name) {}
    virtual ~c_gui_start_menu_hq_transfers_item_selected() {}
    virtual bool handle_list_item_chosen(const c_controller_input_message *, long, c_gui_list_item_widget *, c_gui_data *);
    virtual void update(unsigned long);
    virtual void initialize_datasource();
    __forceinline void setup(const char *name, const s_queued_work_item_status *item_status, bool item_enabled)
    {
        strncpy(filename, name, 17);
        status = *item_status;
        enabled = item_enabled;
    }
    char filename[24];
    s_queued_work_item_status status;
    bool enabled;

protected:
    virtual e_render_data_size get_render_data_size() { return _render_data_size_alert; }
};

typedef char gui_start_menu_hq_transfers_item_selected_size[
    (sizeof(void *) != 4 || ((sizeof(c_gui_start_menu_hq_transfers_item_selected) == 0x1bd0))) ? 1 : -1];
typedef char transfers_selected_filename_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_start_menu_hq_transfers_item_selected, filename) == 0x1a88))) ? 1 : -1];
typedef char transfers_selected_status_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_start_menu_hq_transfers_item_selected, status) == 0x1aa0))) ? 1 : -1];
typedef char transfers_selected_count_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_transfers_selected_item_datasource, item_count) == 0x26c))) ? 1 : -1];
typedef char transfers_selected_datasource_size[(sizeof(void *) != 4 || ((sizeof(c_gui_transfers_selected_item_datasource) == 0x270))) ? 1 : -1];
typedef char transfers_selected_datasource_status_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_transfers_selected_item_datasource, status) == 0x120))) ? 1 : -1];
#endif
