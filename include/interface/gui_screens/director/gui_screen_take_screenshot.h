#ifndef INTERFACE_GUI_SCREENS_DIRECTOR_GUI_SCREEN_TAKE_SCREENSHOT_H
#define INTERFACE_GUI_SCREENS_DIRECTOR_GUI_SCREEN_TAKE_SCREENSHOT_H

#include "cseries/cseries.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/virtual_keyboard_task.h"
#include "multithreading/synchronized_value.h"
#include "saved_games/saved_film_slot_types.h"
#include "tag_files/file_reference.h"

struct s_screenshot_capture_storage;

class c_gui_screen_take_screenshot : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_take_screenshot(long name) :
        c_gui_screen_widget(name),
        m_state(0),
        m_keyboard(c_virtual_keyboard_task::get_instance(
            "..\\interface\\gui_screens\\director\\gui_screen_take_screenshot.cpp",
            213, _controller_index_none, 0, 0, 0, 256, 0)),
        m_task_index(NONE)
    {
    }

    virtual void initialize();
    virtual void dispose();
    virtual void update(unsigned long time);
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *datasource);

private:
    void set_default_title();
    void set_default_description();

    long m_state;
    e_controller_index m_controller_index;
    c_virtual_keyboard_task *m_keyboard;
    char m_capture[256];
    wchar_t m_title[16];
    wchar_t m_description[128];
    s_file_reference m_file;
    long m_task_index;
    c_synchronized_long m_task_done;
    c_synchronized_long m_task_success;
    long m_content_item_index;
    unsigned long m_started_time;
};

typedef char gui_screen_take_screenshot_size[
    (sizeof(void *) != 4 || sizeof(c_gui_screen_take_screenshot) == 0x1dd8) ? 1 : -1];

class c_gui_screen_saved_film_snippet : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_saved_film_snippet(long name) : c_gui_screen_widget(name) {}

    virtual void initialize();
    virtual void dispose();
    virtual void update(unsigned long time);
    virtual void initialize_datasource();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *datasource);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
};

// String IDs used by the take-screenshot screen.
enum e_take_screenshot_string_id
{
    _take_screenshot_string_default_title_multiplayer = 0x1029f,
    _take_screenshot_string_default_title_campaign = 0x102a0,
    _take_screenshot_string_default_title_editor = 0x102a1,
    _take_screenshot_string_default_description_multiplayer = 0x102a2,
    _take_screenshot_string_default_description_campaign = 0x102a3,
    _take_screenshot_string_default_description_editor = 0x102a4,
    _take_screenshot_string_button_list = 0x1055f,
    _take_screenshot_string_snippet_discard = 0x1056f,
    _take_screenshot_string_snippet_preview = 0x10570,
    _take_screenshot_string_snippet_option_list = 0x10571,
    _take_screenshot_string_screenshot_option_list = 0x10577,
    _take_screenshot_string_save_as = 0x10578,
    _take_screenshot_string_save_auto = 0x10579,
    _take_screenshot_string_screenshot_title_keyboard_name = 0x1057b,
    _take_screenshot_string_screenshot_title_keyboard_description = 0x1057c,
    _take_screenshot_string_screenshot_description_keyboard_name = 0x1057d,
    _take_screenshot_string_screenshot_description_keyboard_description = 0x1057e,
};

extern "C" bool gui_screen_take_screenshot_parse_campaign_difficulty(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool parse_xml_game_engine_type(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool gui_screen_take_screenshot_parse_map_name(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool gui_screen_take_screenshot_begin_capture(s_screenshot_capture_storage *, short width, short height);

extern s_screenshot_capture_storage g_screenshot_capture_storage;

extern "C" const wchar_t k_magic_string_difficulty[];

extern "C" const wchar_t k_game_details_tag_campaign_name[];

extern "C" const wchar_t k_magic_string_map_name[];

extern "C" const wchar_t k_magic_string_date[];

#endif // INTERFACE_GUI_SCREENS_DIRECTOR_GUI_SCREEN_TAKE_SCREENSHOT_H
