#ifndef INTERFACE_C_GUI_SCREEN_WIDGET_H
#define INTERFACE_C_GUI_SCREEN_WIDGET_H

#include "shell/shell_types.h"

#include "interface/screen_transition_types.h"

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "interface/core_widget_definition.h"
#include "interface/gui_widget.h"
#include "interface/gui_data.h"
#include "interface/gui_location_manager.h"

class c_gui_data;
class c_gui_group_widget;
class c_gui_list_widget;
class c_gui_list_item_widget;
class c_game_tag_parser;
class c_message;
class c_dialog_result_message;
class c_controller_input_message;
struct s_screen_widget_definition;
struct s_emblem_info;
enum e_render_data_size;
struct s_runtime_screen_widget_definition : s_runtime_core_widget_definition {
    long string_list_index;                 // the strings tag (text_group_get_string)
    long initial_button_key_name;
    s_tag_block datasources;
    s_tag_block groups;
    s_tag_block button_keys;
    long sound_override_index;              // use: passed to window_manager_play_input_sound
    char script_name[32];
    short script_index;
    short padding;

    s_runtime_screen_widget_definition();
};

typedef char runtime_screen_widget_definition_size[(sizeof(s_runtime_screen_widget_definition) == 0x88) ? 1 : -1];

struct s_window_manager_render_list_entry {
    long type;
    long render_data_offset;
    float rendered_depth;
    long render_depth_bias;
};

struct s_window_manager_screen_render_data {
    s_window_manager_screen_render_data() : render_data_buffer(0), buffer_size(0), current_buffer_offset(0) { clear(); }
    bool valid() { return render_data_buffer != 0; }
    void clear() { current_buffer_offset = 0; current_count = 0; }

    char *render_data_buffer;
    long buffer_size;
    long current_buffer_offset;
    s_window_manager_render_list_entry render_list[384];
    unsigned long current_count;
    rectangle2d render_bounds; // the window bounds the render list was assembled for
};

typedef char window_manager_screen_render_data_size[(sizeof(void *) != 4 || ((sizeof(s_window_manager_screen_render_data) == 0x1818) )) ? 1 : -1];

class c_gui_screen_widget;
extern "C" void gui_screen_widget_set_display_group(c_gui_screen_widget *screen, long type, long name);
extern "C" void start_menu_settings_appearance_initialize_parser(c_gui_screen_widget *screen);
extern "C" void start_menu_settings_flags_initialize_parser(c_gui_screen_widget *screen);
extern "C" void start_menu_settings_appearance_colors_initialize_parser(c_gui_screen_widget *screen);

class c_gui_screen_widget : public c_gui_widget {
    friend void gui_screen_widget_set_display_group(c_gui_screen_widget *screen, long type, long name);
    friend void start_menu_settings_appearance_initialize_parser(c_gui_screen_widget *screen);
    friend void start_menu_settings_flags_initialize_parser(c_gui_screen_widget *screen);
    friend class c_window_manager;
    friend void gui_screen_take_screenshot_initialize_parsers(void *screen);
    friend void start_menu_settings_appearance_colors_initialize_parser(c_gui_screen_widget *screen);
    friend void gui_game_details_initialize(c_gui_screen_widget *screen);
public:
    enum e_display_group_type {
        _display_group_type_group = 0,
        _display_group_type_button_key,
        k_display_group_type_count
    };

    c_gui_screen_widget(long name);
    virtual ~c_gui_screen_widget() {}
    virtual void initialize(); // slot 2
    virtual void post_initialize(); // slot 3
    virtual void dispose(); // slot 4
    virtual s_runtime_core_widget_definition *get_core_definition(); // slot 9
    virtual void update(unsigned long current_milliseconds); // slot 23
    virtual bool get_string_by_string_id(long string_identifier, c_static_wchar_string<1024> *buffer); // slot 31
protected:
    virtual e_render_data_size get_render_data_size(); // slot 32
public:
    virtual e_gui_location get_gui_location() const { return _gui_location_none; } // slot 33
    virtual void reconstruct(); // slot 34
    virtual void initialize_datasource(); // slot 35
    virtual void reload_assets(); // slot 36
    virtual void reload_next_frame(); // slot 37
    virtual bool can_receive_focus() const { return false; } // slot 38
    virtual void update_render() {}
    virtual bool process_message(const c_message *message); // slot 40
    virtual bool handle_focused_widget_selected(const c_controller_input_message *message, c_gui_widget *widget); // slot 41
    virtual __forceinline bool handle_list_item_chosen(
        const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *datasource) { return false; }
    virtual bool handle_controller_input_message(const c_controller_input_message *message); // slot 43
    virtual bool handle_dialog_result(const c_dialog_result_message *message); // slot 44
    virtual void submenu_invoked(c_gui_list_widget *submenu_widget); // slot 45
    virtual bool transitioning_in(); // slot 46
    virtual bool transitioning_out(); // slot 47
    virtual void transition_out_with_transition_type(long transition_out, e_screen_transition_type transition_type);
    virtual bool get_emblem_info_for_widget(c_gui_widget *widget, s_emblem_info *emblem_info) { return false; }

    void add_definition_fields(const s_screen_widget_definition *definition, bool require_nonzero_bounds);
    void dispose_child_screens();
    e_window_index get_render_window();
    c_gui_data *get_data(long name, long *datasource_index);
    void transfer_focus_without_animations(c_gui_widget *widget, bool play_received_animation, bool play_lost_animation);
    void transfer_focus_to_list(c_gui_list_widget *list_widget, long element_handle, bool play_received_animation, bool play_lost_animation);
    void set_text_widget_string_id(long widget_name, long value);
    void set_text_widget_text(long widget_name, const wchar_t *value);
    void set_text_widget_text_from_datasource(long widget_name, long datasource_name, long element_handle, long value_name);
    void set_text_widget_string_id_from_datasource(long widget_name, long datasource_name, long element_handle, long value_name);
    long get_constants_datasource_integer(long name);
    __forceinline void add_datasource(c_gui_data *datasource)
    {
        m_datasource[m_datasource_count] = datasource;
        m_datasource_count++;
    }
    __forceinline long get_screen_index() const { return m_screen_index; }
    __forceinline void set_screen_index(long screen_index) { m_screen_index = screen_index; }
    __forceinline s_window_manager_screen_render_data *get_render_state() { return &m_render_data; }
    __forceinline bool get_responds_to_controller_events() const { return m_responds_to_controller_events; }
    __forceinline void set_responds_to_controller_events(bool responds) { m_responds_to_controller_events = responds; }
    __forceinline void set_suppress_focus(bool suppress_focus) { m_suppress_focus = suppress_focus; }
    __forceinline c_gui_widget *get_focused_widget() { return m_suppress_focus ? 0 : m_focused_widget; }
    __forceinline long get_datasource_count() const { return m_datasource_count; }
    __forceinline long get_initial_button_key_name() { return m_core_definition.initial_button_key_name; }
    __forceinline bool running_in_codeless_mode() const { return m_codeless_mode; }
    __forceinline void set_initial_focus(long list_name, long element_handle)
    {
        m_initial_focus_list_name = list_name;
        m_initial_focus_element_handle = element_handle;
    }
    __forceinline void set_initial_focus_by_name(long list_name, long column_name, long column_value)
    {
        m_initial_focus_list_name = list_name;
        m_initial_focus_column_name = column_name;
        m_initial_focus_column_value = column_value;
    }

protected:
    void add_game_tag_parser(c_game_tag_parser *parser);

private:
    void dispose_datasource();
    void set_initial_focused_widget();
public:
    __forceinline c_gui_data *find_data(long name, long *datasource_index)
    {
        c_gui_data *result = 0;
        if (datasource_index)
            *datasource_index = -1;
        for (long test_index = 0; test_index < m_datasource_count; ++test_index)
        {
            if (m_datasource[test_index]->get_name() == name)
            {
                result = m_datasource[test_index];
                if (datasource_index)
                    *datasource_index = test_index;
                break;
            }
        }
        return result;
    }
private:
    __forceinline void clear_display_groups()
    {
        for (long group_index = 0; group_index < 2; group_index++)
        {
            m_current_display_group_widgets[group_index] = 0;
            m_previous_display_group_widgets[group_index] = 0;
            m_display_group_types[group_index] = -1;
        }
    }
    __forceinline c_gui_group_widget *load_display_group_inline(
        const s_runtime_screen_widget_definition *definition, e_display_group_type group_type, long group);
    c_gui_group_widget *load_display_group(const s_runtime_screen_widget_definition *definition, e_display_group_type group_type, long group_index);
    void create_and_add_child_widgets_from_definition(s_runtime_screen_widget_definition *definition);
    void dispose_display_group(c_gui_group_widget *group);
    void swap_display_group_block();

public:
    long m_screen_index;
    unsigned long m_creation_time_milliseconds;
    unsigned long m_disposal_time_milliseconds;
    unsigned long m_last_focus_change_time_milliseconds;
    c_gui_widget *m_focused_widget;
    bool m_suppress_focus;
    bool m_render_in_screenshot;
    c_gui_group_widget *m_current_display_group_widgets[k_display_group_type_count];
    c_gui_group_widget *m_previous_display_group_widgets[k_display_group_type_count];
    long m_display_group_types[k_display_group_type_count];
    bool m_reload_next_frame;
    bool m_responds_to_controller_events;
    long m_initial_focus_list_name;
    long m_initial_focus_element_handle;
    long m_initial_focus_column_name;
    long m_initial_focus_column_value;
    s_runtime_screen_widget_definition m_core_definition;
    c_gui_data *m_datasource[32];
    long m_datasource_count;
    c_game_tag_parser *m_game_tag_parsers[20];
    long m_game_tag_parser_count;
    s_window_manager_screen_render_data m_render_data;
    bool m_codeless_mode;
};

typedef char gui_screen_widget_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_widget) == 0x1a88) )) ? 1 : -1];
typedef char gui_screen_widget_index_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_widget, m_screen_index) == 0xc8))) ? 1 : -1];
typedef char gui_screen_widget_definition_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_widget, m_core_definition) == 0x10c) )) ? 1 : -1];
typedef char gui_screen_widget_datasource_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_widget, m_datasource) == 0x194) )) ? 1 : -1];
typedef char gui_screen_widget_parsers_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_widget, m_game_tag_parsers) == 0x218) )) ? 1 : -1];
typedef char gui_screen_widget_render_data_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_widget, m_render_data) == 0x26c) )) ? 1 : -1];

#endif // INTERFACE_C_GUI_SCREEN_WIDGET_H
