#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_START_MENU_SETTINGS_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_START_MENU_SETTINGS_H

#include "cseries/cseries.h"
#include "interface/gui_data.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"

// Functions defined in tu/interfacelibxbox/start_menu_settings.cpp.

bool parse_xml_look_sensitivity(void *widget, wchar_t *buffer, long buffer_length);
bool parse_xml_look_centering(void *widget, wchar_t *buffer, long buffer_length);

class c_gui_widget;
class c_controller_input_message;
class c_gui_data;
class c_gui_list_item_widget;
class c_gui_list_widget;
class c_gui_custom_bitmap_widget;
struct s_autosave_queue;
class c_gui_selected_item;

// The settings pane of the start menu.
class c_start_menu_settings : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_start_menu_settings(long name) : c_start_menu_pane_screen_widget(name)
    {
        m_translation = 0.0f;
        m_rotation = 0.0f;
        m_automatic_button_key = NONE;
    }
    virtual ~c_start_menu_settings() {}
    virtual void initialize();
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_list_item_chosen(
        const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data);

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *message) { return true; }
};

struct s_gui_start_menu_settings_screenshots : c_start_menu_pane_screen_widget
{
    __forceinline s_gui_start_menu_settings_screenshots(long name) :
        c_start_menu_pane_screen_widget(name, NONE),
        last_scroll_position(NONE),
        array(0)
    {
    }

    virtual void initialize();
    virtual void dispose();
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    virtual void update(unsigned long current_milliseconds);
    virtual void initialize_datasource();
    virtual bool handle_list_item_chosen(
        const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *message) { return false; }

public:
    long last_scroll_position;
    s_autosave_queue *array;
};

typedef char settings_screenshots_scroll_position_offset[
    (sizeof(void *) != 4 || ((offsetof(s_gui_start_menu_settings_screenshots, last_scroll_position) == 0x1b08))) ? 1 : -1];
typedef char settings_screenshots_array_offset[(sizeof(void *) != 4 || ((offsetof(s_gui_start_menu_settings_screenshots, array) == 0x1b0c))) ? 1 : -1];
typedef char settings_screenshots_size[(sizeof(void *) != 4 || ((sizeof(s_gui_start_menu_settings_screenshots) == 0x1b10))) ? 1 : -1];

// Settings screenshots datasource: the ordered-data base owns the name and
// disabled elements; the controller and the screenshot queue follow its
// 0x10c bytes. Its rows are the queue's entries.
struct s_start_menu_settings_datasource : public c_gui_ordered_data
{
    __forceinline s_start_menu_settings_datasource(long controller_index)
    {
        player_index = controller_index;
        queue = 0;
    }
    virtual bool initialize(long name);
    virtual void dispose();
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_handle, long value_name, long *value);
    virtual bool get_text_value(long element_handle, long value_name, c_static_wchar_string<1024> *value);
    virtual const c_gui_selected_item *get_gui_selected_item(long element_handle) const;

protected:
    virtual long get_current_item_count_internal();

public:
    int player_index;
    s_autosave_queue *queue;
};

typedef char settings_datasource_size[(sizeof(void *) != 4 || ((sizeof(s_start_menu_settings_datasource) == 0x114))) ? 1 : -1];

extern "C" void start_menu_settings_update_list_bitmaps(s_gui_start_menu_settings_screenshots *screen);
extern "C" bool start_menu_settings_load_item_bitmap(s_gui_start_menu_settings_screenshots *screen, c_gui_list_item_widget *list_item);
extern "C" void start_menu_settings_copy_item_bitmap(s_gui_start_menu_settings_screenshots *screen,
    c_gui_widget *previous_widget, c_gui_list_item_widget *list_item);
extern "C" void start_menu_settings_reset_item_bitmap(s_gui_start_menu_settings_screenshots *screen, c_gui_list_item_widget *list_item);
extern "C" void start_menu_settings_update_preview_bitmap(s_gui_start_menu_settings_screenshots *screen,
    c_gui_list_widget *list, c_gui_custom_bitmap_widget *bitmap);

extern "C" bool parse_xml_focused_item_name(c_gui_widget *widget, wchar_t *buffer, unsigned int buffer_length);
extern "C" bool parse_xml_focused_item_description(c_gui_widget *widget, wchar_t *buffer, unsigned int buffer_length);
extern "C" bool parse_xml_focused_item_author(c_gui_widget *widget, wchar_t *buffer, unsigned int buffer_length);
extern "C" bool parse_xml_focused_item_date(c_gui_widget *widget, wchar_t *buffer, unsigned int buffer_length);
extern "C" bool parse_xml_focused_item_size(c_gui_widget *widget, wchar_t *buffer, unsigned int buffer_length);


#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_START_MENU_SETTINGS_H
