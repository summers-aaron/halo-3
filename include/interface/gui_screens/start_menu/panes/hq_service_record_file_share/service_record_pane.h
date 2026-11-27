#ifndef INTERFACE_SERVICE_RECORD_PANE_H
#define INTERFACE_SERVICE_RECORD_PANE_H

#include "cseries/cseries.h"
#include "interface/gui_data.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"

class c_gui_start_menu_hq_service_record_pane : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_gui_start_menu_hq_service_record_pane(long name) : c_start_menu_pane_screen_widget(name)
    {
        m_translation = 0.0f;
        m_rotation = 0.0f;
        m_automatic_button_key = NONE;
    }
    virtual void initialize();
    virtual void update(unsigned long current_milliseconds);
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual void initialize_datasource();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list,
        c_gui_list_item_widget *item, c_gui_data *data);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual bool get_emblem_info_for_widget(c_gui_widget *widget, s_emblem_info *emblem_info);
    using c_start_menu_pane_screen_widget::get_owner_xuid;

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *) { return false; }
};
typedef char gui_start_menu_hq_service_record_pane_size[
    (sizeof(void *) != 4 || sizeof(c_gui_start_menu_hq_service_record_pane) == 0x1b08) ? 1 : -1];

// The pane's insignia_data datasource: one sub-datasource and its selected element per insignia column.
// Each element handle is a column index; values come from that column's selected element.
class c_gui_insignia_data : public c_gui_ordered_data
{
public:
    __forceinline c_gui_insignia_data(c_gui_data *training, c_gui_data *spartan, c_gui_data *officer, c_gui_data *campaign)
    {
        m_data[0] = training;
        m_data[1] = spartan;
        m_data[2] = officer;
        m_data[3] = campaign;
        for (long i = 0; i < 4; ++i)
            m_selected_items[i] = -1;
    }

    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);

    void set_selected_item(long element_handle, long value);

    __forceinline long get_bitmap_name(long element_handle)
    {
        long result = -1;
        if (VALID_INDEX(element_handle, 4))
            result = m_data[element_handle]->get_name();
        return result;
    }
    __forceinline bool get_sprite_frame(long element_handle, long *frame)
    {
        bool result = false;
        if (VALID_INDEX(element_handle, 4))
            result = get_integer_value(element_handle, 0x3f5, frame);
        return result;
    }
    __forceinline bool bitmap_is_visible(long element_handle, long name)
    {
        bool result = false;
        if (VALID_INDEX(element_handle, 4))
            result = name == m_data[element_handle]->get_name();
        return result;
    }

protected:
    virtual long get_current_item_count_internal() { return 4; }

public:
    long m_category;
    c_gui_data *m_data[4];
    long m_selected_items[4];
};

#endif
