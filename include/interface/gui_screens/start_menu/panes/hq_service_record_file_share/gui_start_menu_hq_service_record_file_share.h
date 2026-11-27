#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SERVICE_RECORD_FILE_SHARE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SERVICE_RECORD_FILE_SHARE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_H

// Functions defined in tu/interfacelibxbox/gui_start_menu_hq_service_record_file_share.cpp.

class c_gui_file_share_slots_datasource;
template<long k_maximum_count> class c_static_wchar_string;

extern "C" bool gui_start_menu_hq_service_record_file_share_is_owner(void *screen);
bool parse_xml_quota_used(void *context, wchar_t *buffer, long buffer_length);
bool parse_xml_quota_total(void *context, wchar_t *buffer, long buffer_length);
bool parse_xml_quota_remaining(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool parse_xml_file_share_progress(void *self, wchar_t *buffer, unsigned long buffer_length);







class c_gui_custom_bitmap_widget;
class c_gui_data;
extern "C" void gui_start_menu_hq_service_record_file_share_set_map_image(c_gui_custom_bitmap_widget *widget, c_gui_data *data, long index, long unknown);



#include "interface/gui_data.h"
#include "networking/online/online_files_enumerator.h"
#include "networking/online/online_file_work_item_types.h"

#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/service_record_pane.h"

class c_controller_input_message;
class c_gui_list_item_widget;
class c_gui_list_widget;

// The file share pane (start_menu_file_share and start_menu_bungie_file_share screens).
class c_gui_start_menu_hq_service_record_file_share : public c_gui_start_menu_hq_service_record_pane
{
public:
    c_gui_start_menu_hq_service_record_file_share(long name);
    virtual ~c_gui_start_menu_hq_service_record_file_share() {}
    virtual void post_initialize();
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void initialize();
    virtual void update(unsigned long current_milliseconds);
    virtual void dispose();
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual void initialize_datasource();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data);
    unsigned __int64 m_owner_xuid;
    unsigned __int64 m_player_xuid;
    s_file_share_enumerator_state m_enumerator;
    long m_quota_used;
    long m_file_count;
    long m_controller_state;
    bool m_controller_state_changed;
    bool m_content_available;
    bool m_content_busy;
    __forceinline long get_quota_used() { return m_quota_used; }
    __forceinline long get_quota_total()
    {
        if (m_enumerator.is_busy())
            return -1;
        return (long)m_enumerator.get_quota_byte_count();
    }

private:
    void update_slots();
    void get_quota_flags(c_gui_list_widget *list, long element_handle, bool *has_first, bool *has_second);
    void update_empty();
};


typedef char gui_start_menu_hq_service_record_file_share_size[(sizeof(void *) != 4 || sizeof(c_gui_start_menu_hq_service_record_file_share) == 31504) ? 1 : -1];

struct s_gui_file_share_slot
{
    long type;
    union
    {
        s_online_file_description file;
        s_queued_work_item_status transfer;
    };
};

class c_gui_file_share_slots_datasource : public c_gui_ordered_data
{
public:
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual void update();
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);

    __forceinline c_gui_file_share_slots_datasource(c_online_files_enumerator *enumerator,
        long controller_index, bool show_quota);

protected:
    virtual long get_current_item_count_internal();
    c_online_files_enumerator *m_enumerator;
    long m_controller_index;
    s_gui_file_share_slot m_slots[25];
    long m_slot_count;
    bool m_show_quota;
public:
    virtual void get_column_names(long *const column_names, long *column_count);
};

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SERVICE_RECORD_FILE_SHARE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_H
