#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SCREENSHOTS_GUI_START_MENU_HQ_SCREENSHOTS_SUBMENU_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SCREENSHOTS_GUI_START_MENU_HQ_SCREENSHOTS_SUBMENU_H

#include "cseries/cseries.h"
#include "interface/gui_screens/campaign/gui_screen_screenshot_viewer.h"
#include "interface/gui_data.h"
#include "interface/user_interface_messages.h"
#include "interface/virtual_keyboard_task.h"
#include "tag_files/string_ids.h"

struct s_autosave_queue;

// Set 1 string ids this pane reads (names from the cache string table).
enum e_hq_screenshots_submenu_string
{
    _hq_screenshots_submenu_string_cancel = 0x10020,
    _hq_screenshots_submenu_string_author = 0x10208,
    _hq_screenshots_submenu_string_date = 0x10209,
    _hq_screenshots_submenu_string_size = 0x1020b,
    _hq_screenshots_submenu_string_delete = 0x10241,
    _hq_screenshots_submenu_string_screenshots_preview = 0x1029d,
    _hq_screenshots_submenu_string_screenshots_option = 0x1029e,
    _hq_screenshots_submenu_string_edit_title = 0x10303,
    _hq_screenshots_submenu_string_edit_description = 0x10304,
    _hq_screenshots_submenu_string_image_index = 0x10327,
    _hq_screenshots_submenu_string_updating_metadata_title = 0x100e5,
    _hq_screenshots_submenu_string_updating_metadata_message = 0x100e6,
    _hq_screenshots_submenu_string_screenshot_title_keyboard_name = 0x1057b,
    _hq_screenshots_submenu_string_screenshot_title_keyboard_description = 0x1057c,
    _hq_screenshots_submenu_string_screenshot_description_keyboard_name = 0x1057d,
    _hq_screenshots_submenu_string_screenshot_description_keyboard_description = 0x1057e,
    _hq_screenshots_submenu_string_confirm_saved_game_delete = 0x30029,
};

enum e_hq_screenshots_submenu_state
{
    _hq_screenshots_submenu_state_idle = 0,
    _hq_screenshots_submenu_state_confirm_delete,
    _hq_screenshots_submenu_state_delete_item,
    _hq_screenshots_submenu_state_wait_for_delete,
    _hq_screenshots_submenu_state_edit_title,
    _hq_screenshots_submenu_state_wait_for_title_keyboard,
    _hq_screenshots_submenu_state_save_title,
    _hq_screenshots_submenu_state_edit_description,
    _hq_screenshots_submenu_state_wait_for_description_keyboard,
    _hq_screenshots_submenu_state_save_description,
    _hq_screenshots_submenu_state_wait_for_metadata_update,
};

class c_gui_start_menu_hq_screenshots_submenu : public c_gui_screenshot_screen
{
public:
    __forceinline c_gui_start_menu_hq_screenshots_submenu(long name) :
        c_gui_screenshot_screen(name), controller_index(_controller_index_none)
    {
        keyboard_task = c_virtual_keyboard_task::get_instance(
            k_gui_start_menu_hq_screenshots_submenu_source_file, 33, _controller_index_none, 0, 0, 0, 256, 0);
        state = _hq_screenshots_submenu_state_idle;
        list = 0;
        selected_index = NONE;
        timestamp = NONE;
        message_shown_time = NONE;
    }

    virtual void initialize();
    virtual void dispose();
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *item, c_gui_data *data);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual bool handle_dialog_result(const c_dialog_result_message *message);

    long controller_index;
    c_virtual_keyboard_task *keyboard_task;
    wchar_t name[16];
    wchar_t description[128];
    e_hq_screenshots_submenu_state state;
    s_autosave_queue *list;
    long selected_index;
    long timestamp;
    long message_shown_time;
};

typedef c_gui_start_menu_hq_screenshots_submenu s_gui_start_menu_hq_screenshots_submenu;

typedef char screenshots_submenu_controller_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_start_menu_hq_screenshots_submenu, controller_index) == 0x1a88))) ? 1 : -1];
typedef char screenshots_submenu_state_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_start_menu_hq_screenshots_submenu, state) == 0x1bb0))) ? 1 : -1];
typedef char screenshots_submenu_selection_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_start_menu_hq_screenshots_submenu, selected_index) == 0x1bb8))) ? 1 : -1];
typedef char screenshots_submenu_message_shown_time_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_start_menu_hq_screenshots_submenu, message_shown_time) == 0x1bc0))) ? 1 : -1];
typedef char screenshots_submenu_size[(sizeof(void *) != 4 || ((sizeof(c_gui_start_menu_hq_screenshots_submenu) == 0x1bc8))) ? 1 : -1];

// Opens the screenshot item options screen on the focused screenshot.
class c_load_hq_screenshots_item_options_screen_message : public c_load_screen_message
{
public:
    __forceinline c_load_hq_screenshots_item_options_screen_message(
        e_controller_index controller, e_window_index window, long layered_position, long selected_index)
        : c_load_screen_message(_string_id_start_menu_hq_screenshots_item_options, controller, window, layered_position)
    {
        m_selected_index = selected_index;
    }
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;

    long m_selected_index;
};

extern "C" void gui_start_menu_hq_screenshots_submenu_get_item_name(c_gui_start_menu_hq_screenshots_submenu *screen);
extern "C" void gui_start_menu_hq_screenshots_submenu_get_item_description(c_gui_start_menu_hq_screenshots_submenu *screen);
extern "C" void start_menu_game_editor_update_loading_message(
    c_gui_start_menu_hq_screenshots_submenu *screen, bool loading, bool *message_active);

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_HQ_SCREENSHOTS_GUI_START_MENU_HQ_SCREENSHOTS_SUBMENU_H
