#ifndef INTERFACE_GUI_SCREENS_START_MENU_GUI_START_MENU_HQ_TRANSFERS_H
#define INTERFACE_GUI_SCREENS_START_MENU_GUI_START_MENU_HQ_TRANSFERS_H

class c_gui_widget;
class c_gui_data;
#include "interface/gui_data.h"
#include "networking/online/online_file_work_item_types.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "cseries/cseries.h"

class c_active_transfers_datasource : public c_gui_ordered_data
{
public:
    __forceinline c_active_transfers_datasource() : m_transfer_count(0) {}
    virtual void update();
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
    virtual bool get_qword_value(long element_handle, long value_name, unsigned __int64 *value);
    virtual bool get_real_value(long element_handle, long value_name, float *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    s_queued_work_item_status m_transfers[16];
    long m_transfer_count;
protected:
    virtual long get_current_item_count_internal();
};

typedef char active_transfers_datasource_size[(sizeof(void *) != 4 || ((sizeof(c_active_transfers_datasource) == 0x1398))) ? 1 : -1];
extern "C" unsigned char online_files_is_transfer_ready(char *manager);

// The headquarters transfers pane: lists the active online file transfers.
class c_gui_start_menu_hq_transfers : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_gui_start_menu_hq_transfers(long name) : c_start_menu_pane_screen_widget(name)
    {
        m_translation = 0.0f;
        m_rotation = 0.0f;
        m_automatic_button_key = NONE;
    }
    virtual ~c_gui_start_menu_hq_transfers() {}
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    virtual void update(unsigned long current_milliseconds);
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual void initialize_datasource();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *datasource);
protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *message) { return false; }
};

#endif // INTERFACE_GUI_SCREENS_START_MENU_GUI_START_MENU_HQ_TRANSFERS_H
