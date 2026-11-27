#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SERVICE_RECORD_FILE_SHARE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_CHOOSE_ITEM_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SERVICE_RECORD_FILE_SHARE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_CHOOSE_ITEM_H

#include "interface/c_gui_screen_widget.h"
#include "interface/c_player_profile.h"
#include "cseries/async_xoverlapped_types.h"
#include "networking/online/virtual_keyboard_task.h"
#include "networking/online/online_file_work_item_types.h"
#include "saved_games/saved_game_file_types.h"
#include "interface/gui_data.h"
#include "tag_files/file_reference.h"
#include "tag_files/file_last_modification_date.h"
#include "saved_games/saved_game_item_metadata.h"
#include "memory/static_arrays.h"
#include "multithreading/synchronized_value.h"

// String ids of the values the file-share item lists answer.
enum e_file_share_item_string_id
{
    _file_share_item_string_title = 0x29d,
    _file_share_item_string_description = 0x29e,
    _file_share_item_string_author = 0x10208,
    _file_share_item_string_date = 0x10209,
    _file_share_item_string_size = 0x1020b,
    _file_share_item_string_corrupt_file = 0x102ff,
    _file_share_item_string_image_index = 0x10327,
};

struct s_gui_content_file_share_item
{
    s_file_reference file_reference;
    long valid;
    s_saved_game_item_metadata metadata;
    s_file_last_modification_date modification_date;
};
typedef char file_share_metadata_author_id_offset[(offsetof(s_saved_game_item_metadata, author_id) == 0xc0) ? 1 : -1];
typedef char file_share_content_metadata_offset[(sizeof(void *) != 4 || ((offsetof(s_gui_content_file_share_item, metadata) == 0x118) )) ? 1 : -1];
typedef char file_share_content_modification_date_offset[
    (sizeof(void *) != 4 || ((offsetof(s_gui_content_file_share_item, modification_date) == 0x210))) ? 1 : -1];
typedef char file_share_content_item_size[(sizeof(void *) != 4 || ((sizeof(s_gui_content_file_share_item) == 0x218) )) ? 1 : -1];


class c_gui_content_item_datasource : public c_gui_ordered_data
{
public:
    __forceinline c_gui_content_item_datasource(e_controller_index controller, e_saved_game_file_type file_type)
        : m_controller(controller), m_file_type(file_type), m_item_count(0), m_enumeration_complete(false) {}
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual long get_current_item_count_internal();
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual void update();

    e_controller_index m_controller;
    e_saved_game_file_type m_file_type;
    s_gui_content_file_share_item m_items[151];
    long m_item_count;
    bool m_enumeration_complete;
    __forceinline const s_saved_game_item_metadata *get_metadata(long element_handle) const
    {
        const s_saved_game_item_metadata *metadata = 0;
        if (element_handle >= 0 && element_handle < m_item_count)
            metadata = &m_items[element_handle].metadata;
        return metadata;
    }
private:
    void update_enumeration();
};


typedef char file_share_content_items_offset[(sizeof(void *) != 4 || offsetof(c_gui_content_item_datasource, m_items) == 0x118) ? 1 : -1];
typedef char file_share_content_count_offset[(sizeof(void *) != 4 || offsetof(c_gui_content_item_datasource, m_item_count) == 0x13d40) ? 1 : -1];
typedef char file_share_content_datasource_size[(sizeof(void *) != 4 || sizeof(c_gui_content_item_datasource) == 0x13d48) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/gui_start_menu_hq_service_record_file_share_choose_item.cpp.

class c_gui_start_menu_hq_service_record_file_share_choose_item;

extern "C" bool parse_quota_used(void *context, wchar_t *buffer, unsigned long buffer_length);
extern "C" bool parse_quota_total(void *context, wchar_t *buffer, unsigned long buffer_length);

class c_gui_start_menu_hq_service_record_file_share_choose_item : public c_gui_screen_widget
{
public:
    __forceinline c_gui_start_menu_hq_service_record_file_share_choose_item(long name) :
        c_gui_screen_widget(name),
        m_datasource(0),
        m_pending_action(0),
        m_file_name(),
        m_task("..\\interface\\gui_screens\\start_menu\\panes\\hq_service_record_file_share\\"
            "gui_start_menu_hq_service_record_file_share_choose_item.cpp", 219),
        m_keyboard(c_virtual_keyboard_task::get_instance(
            "..\\interface\\gui_screens\\start_menu\\panes\\hq_service_record_file_share\\"
            "gui_start_menu_hq_service_record_file_share_choose_item.cpp",
            223, _controller_index_none, 0, 0, 0, 256, 0)),
        m_async_task_id(NONE),
        m_async_complete(),
        m_async_succeeded(),
        m_unused(0)
    {
    }
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data);
    virtual void update(unsigned long milliseconds);
    virtual void initialize();
    virtual void initialize_datasource();
    virtual void dispose();
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
private:
    void update_prepare_to_upload();

public:
    void setup(unsigned __int64 owner_xuid, unsigned __int64 target_xuid, long a, long b, long c, e_saved_game_file_type file_type)
    {
        m_owner_xuid = owner_xuid;
        m_target_xuid = target_xuid;
        m_a = a;
        m_quota_used = b;
        m_quota_total = c;
        m_file_type = file_type;
    }
    long get_quota_used() { return m_quota_used; }
    long get_quota_total() { return m_quota_total; }

    unsigned __int64 m_owner_xuid;
    unsigned __int64 m_target_xuid;
    long m_a;
    long m_quota_used;
    long m_quota_total;
    int m_file_type;
    c_gui_content_item_datasource *m_datasource;
    long m_pending_action;
    s_online_file_description m_description;
    c_static_string<256> m_file_name;
    c_string_verify_task m_task;
    long m_element_handle;
    c_virtual_keyboard_task *m_keyboard;
    wchar_t m_new_name[256];
    long m_selected_item_index;
    long m_async_task_id;
    c_synchronized_long m_async_complete;
    c_synchronized_long m_async_succeeded;
    long m_unused;
};
typedef char file_share_choose_item_file_name_offset[(sizeof(void *) != 4
    || offsetof(c_gui_start_menu_hq_service_record_file_share_choose_item, m_file_name) == 0x1bb8) ? 1 : -1];
typedef char file_share_choose_item_task_offset[(sizeof(void *) != 4
    || offsetof(c_gui_start_menu_hq_service_record_file_share_choose_item, m_task) == 0x1cb8) ? 1 : -1];
typedef char file_share_choose_item_keyboard_offset[(sizeof(void *) != 4
    || offsetof(c_gui_start_menu_hq_service_record_file_share_choose_item, m_keyboard) == 0x2500) ? 1 : -1];
typedef char file_share_choose_item_size[(sizeof(void *) != 4
    || sizeof(c_gui_start_menu_hq_service_record_file_share_choose_item) == 0x2718) ? 1 : -1];

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SERVICE_RECORD_FILE_SHARE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_CHOOSE_ITEM_H
