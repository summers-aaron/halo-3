#ifndef INTERFACE_GUI_SCREENS_MAIN_MENU_GUI_SCREEN_MAIN_MENU_H
#define INTERFACE_GUI_SCREENS_MAIN_MENU_GUI_SCREEN_MAIN_MENU_H

#include "interface/c_gui_screen_widget.h"
#include "interface/controller_types.h"

// String ids the main menu screen uses, named after their cache string table entries.
enum e_main_menu_string_id
{
    _main_menu_string_leave_game = 0x106,
    _main_menu_string_locked = 0x1fa,
    _main_menu_string_name = 0x29c,
    _main_menu_string_multiplayer = 0x333,
    _main_menu_string_top_most = 0x1000c,
    _main_menu_string_main_menu_offline = 0x10057,
    _main_menu_string_main_menu_online = 0x10058,
    _main_menu_string_matchmaking = 0x10073,
    _main_menu_string_campaign = 0x10074,
    _main_menu_string_mapeditor = 0x10078,
    _main_menu_string_theater = 0x10079,
    _main_menu_string_main_menu = 0x10080,
    _main_menu_string_alpha_legal = 0x100b6,
    _main_menu_string_alpha_locked_down = 0x100b7,
    _main_menu_string_start_new_campaign = 0x100f5,
    _main_menu_string_resume_campaign = 0x100f6,
    _main_menu_string_back_out_main_menu_no_profile = 0x3000f,
    _main_menu_string_need_to_select_storage_device_to_save = 0x3001e,
};

class c_main_menu_screen_widget : public c_gui_screen_widget {
public:
    c_main_menu_screen_widget(long name);
    virtual ~c_main_menu_screen_widget() {}
    long get_signed_in_controller_count(e_controller_index *first_controller_index);
    virtual void dispose();
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *data);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void update(unsigned long time);
    virtual void post_initialize();
    virtual e_gui_location get_gui_location() const { return _gui_location_main_menu; }
    void update_main_menu();
    long mode;
    char pad1a8c;
    bool active;
    char m_catalogues_busy;
    char block[32];
    long m_1ab0;
    long m_1ab4;
};

typedef char main_menu_screen_widget_size[(sizeof(void *) != 4 || ((sizeof(c_main_menu_screen_widget) == 0x1ab8))) ? 1 : -1];

#endif // INTERFACE_GUI_SCREENS_MAIN_MENU_GUI_SCREEN_MAIN_MENU_H
