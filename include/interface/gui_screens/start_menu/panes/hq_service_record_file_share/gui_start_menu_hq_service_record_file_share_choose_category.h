#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SERVICE_RECORD_FILE_SHARE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_CHOOSE_CATEGORY_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SERVICE_RECORD_FILE_SHARE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_CHOOSE_CATEGORY_H

#include "shell/shell.h"
#include "interface/user_interface_messages.h"
#include <stddef.h>

// Functions defined in tu/interfacelibxbox/gui_start_menu_hq_service_record_file_share_choose_category.cpp that other units call.

#include "interface/c_gui_screen_widget.h"

class c_gui_start_menu_hq_service_record_file_share_choose_category : public c_gui_screen_widget
{
public:
    __forceinline c_gui_start_menu_hq_service_record_file_share_choose_category(long name) : c_gui_screen_widget(name) {}
    virtual void initialize();
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *data);
    virtual void update_render_state(unsigned long current_milliseconds);
    __int64 m_a;
    __int64 m_b;
    long m_c;
    long m_d;
    long m_e;
};

bool gui_start_menu_hq_service_record_file_share_choose_category_remaining_size_text(
    const c_gui_start_menu_hq_service_record_file_share_choose_category *widget,
    wchar_t *destination, unsigned long max_chars);
extern "C" bool file_share_is_available(e_controller_index controller_index);

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SERVICE_RECORD_FILE_SHARE_GUI_START_MENU_HQ_SERVICE_RECORD_FILE_SHARE_CHOOSE_CATEGORY_H
