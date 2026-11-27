#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SERVICE_RECORD_FILE_SHARE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_ITEM_SELECTED_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SERVICE_RECORD_FILE_SHARE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_ITEM_SELECTED_H

#include <stddef.h>

// Functions defined in tu/interfacelibxbox/gui_start_menu_hq_service_record_file_share_item_selected.cpp.

#include "interface/c_gui_screen_widget.h"
#include "networking/online/online_file_work_item_types.h"
#include "networking/online/online_friend_xenon.h"
#include "networking/online/virtual_keyboard_task.h"
#include "networking/online/online_service_record.h"
#include "interface/user_interface_messages.h"
#include "memory/static_arrays.h"
#include "tag_files/string_ids.h"
#include <string.h>

class c_gui_start_menu_hq_service_record_file_share_item_selected;
extern "C" void file_share_item_selected_show_recommendation(c_gui_start_menu_hq_service_record_file_share_item_selected *screen);

#include "networking/online/file_share_slot_types.h"
#include "networking/online/online_files_deleter.h"

extern "C" const char k_file_share_keyboard_task_name[];
class c_gui_start_menu_hq_service_record_file_share_item_selected : public c_gui_screen_widget
{
public:
    __forceinline c_gui_start_menu_hq_service_record_file_share_item_selected(long name) :
        c_gui_screen_widget(name),
        m_deleter(),
        m_deleting(false),
        m_find_friend_task(),
        m_keyboard(c_virtual_keyboard_task::get_instance(k_file_share_keyboard_task_name, 135,
            _controller_index_none, 0, 0, 0, 256, 0)),
        m_state(0)
    {
    }
    virtual void dispose();
    virtual void update(unsigned long milliseconds);
    virtual void initialize_datasource();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data);
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
private:
    bool set_string_with_substitute(long string_id, e_saved_game_file_type file_type, const wchar_t *argument, c_static_wchar_string<256> *text);
    friend void file_share_item_selected_show_recommendation(c_gui_start_menu_hq_service_record_file_share_item_selected *screen);
public:
    unsigned __int64 m_owner_xuid;
    unsigned __int64 m_player_xuid;
    long m_size;
    s_file_share_slot m_file_share_slot;
    union {
        char raw[0x1c30 - 0x1bd0];
        s_service_record_identity identity;
    } m_identity;
    c_online_files_deleter m_deleter;
    bool m_deleting;
    c_online_find_friend_task m_find_friend_task;
    c_virtual_keyboard_task *m_keyboard;
    long m_state;
    wchar_t m_new_name[16];
};



typedef char file_share_slot_size_check[(sizeof(s_file_share_slot) == 0x130) ? 1 : -1];
typedef char file_share_slot_description_check[(offsetof(s_file_share_slot, description) == 0x28) ? 1 : -1];
typedef char file_share_screen_owner_check[(sizeof(void *) != 4
    || offsetof(c_gui_start_menu_hq_service_record_file_share_item_selected, m_owner_xuid) == 0x1a88) ? 1 : -1];
typedef char file_share_screen_identity_check[(sizeof(void *) != 4
    || offsetof(c_gui_start_menu_hq_service_record_file_share_item_selected, m_identity) == 0x1bd0) ? 1 : -1];
typedef char file_share_screen_find_friend_task_check[(sizeof(void *) != 4
    || offsetof(c_gui_start_menu_hq_service_record_file_share_item_selected, m_find_friend_task) == 0x2074) ? 1 : -1];
typedef char file_share_screen_keyboard_check[(sizeof(void *) != 4
    || offsetof(c_gui_start_menu_hq_service_record_file_share_item_selected, m_keyboard) == 0x6d18) ? 1 : -1];
typedef char file_share_screen_size_check[(sizeof(void *) != 4
    || sizeof(c_gui_start_menu_hq_service_record_file_share_item_selected) == 0x6d40) ? 1 : -1];

extern "C" void file_share_selected_update_pending_action(c_gui_start_menu_hq_service_record_file_share_item_selected *screen);

extern "C" const wchar_t k_file_share_recommendation_title_format[];

// Opens the screenshots file-share previewer on the chosen file.
class c_load_screenshots_file_share_previewer_screen_message : public c_load_screen_message
{
public:
    __forceinline c_load_screenshots_file_share_previewer_screen_message(e_controller_index controller, e_window_index window,
        long layered_position, unsigned __int64 owner_xuid, unsigned __int64 target_xuid, long slot,
        const s_online_file_description *description, const char *filename) :
        c_load_screen_message(_string_id_screenshots_file_share_previewer, controller, window, layered_position)
    {
        m_owner_xuid = owner_xuid;
        m_target_xuid = target_xuid;
        m_slot = slot;
        memcpy(&m_description, description, sizeof(m_description));
        memcpy(m_filename, filename, sizeof(m_filename));
    }
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;

    unsigned __int64 m_owner_xuid;
    unsigned __int64 m_target_xuid;
    long m_slot;
    char m_filename[33];
    s_online_file_description m_description;
};
typedef char load_screenshots_file_share_previewer_filename_offset[(sizeof(void *) != 4
    || offsetof(c_load_screenshots_file_share_previewer_screen_message, m_filename) == 0x54) ? 1 : -1];
typedef char load_screenshots_file_share_previewer_description_offset[(sizeof(void *) != 4
    || offsetof(c_load_screenshots_file_share_previewer_screen_message, m_description) == 0x78) ? 1 : -1];


#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SERVICE_RECORD_FILE_SHARE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_ITEM_SELECTED_H
