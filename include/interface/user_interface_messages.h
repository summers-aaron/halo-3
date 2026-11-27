#ifndef INTERFACE_USER_INTERFACE_MESSAGES_H
#define INTERFACE_USER_INTERFACE_MESSAGES_H

// The user interface's message classes (Bungie's interface\user_interface_messages.cpp: the March 2007 debug
// build asserts there in c_load_screen_message's constructor, lines 807-809). Shapes and method lists from the
// original source file.
//
// The vtables: c_message, c_controller_input_message, c_dialog_result_message and c_xenon_message share
// one vtable at 0x8204e480 (identical-COMDAT folding): { scalar deleting destructor 0x824c4bb8, initialize
// 0x825bec98, update 0x825bec98 } (both virtuals are the shared empty function). c_load_screen_message
// (0x8204e48c) adds apply_initial_state, empty in the base (0x825bec98).

#include "shell/shell.h"
#include "interface/user_interface_error_manager.h"

class c_gui_screen_widget;

enum e_ui_message_type {
    _ui_controller_input = 1,
    _ui_xenon = 2,
    _ui_load_screen = 3,
    _ui_xenon_message = 5,
    _message_type_dialog_result = 6,
};

// Enumerators are the ones units have named so far; the full lists are not recovered.
enum e_event_type {
    _event_type_none = 0,
    _event_type_tab_up = 1,
    _event_type_tab_left = 2,
    _event_type_tab_down = 3,
    _event_type_tab_right = 4,
    _event_type_alt_stick_up = 5,
    _event_type_alt_stick_left = 6,
    _event_type_alt_stick_down = 7,
    _event_type_alt_stick_right = 8,
    _event_type_alt_tab_up = 9,
    _event_type_alt_tab_left = 10,
    _event_type_alt_tab_down = 11,
    _event_type_alt_tab_right = 12,
    _event_type_button_press = 13,
};

enum e_controller_component {
    _controller_component_none = 0,
    _controller_component_button_b = 1,
    _controller_component_button_x = 2,
    _controller_component_button_y = 3,
    _controller_component_dpad_up = 8,
    _controller_component_dpad_down = 9,
    _controller_component_dpad_left = 10,
    _controller_component_dpad_right = 11,
    _controller_component_button_start = 12,
    _controller_component_button_back = 13,
    _controller_component_button_left_thumb = 14,
    _controller_component_button_right_thumb = 15,
    _controller_component_left_stick = 16,
    _controller_component_right_stick = 17,
};

enum e_gui_dialog_choice {
    _gui_dialog_choice_none = -1,
    _gui_dialog_choice_first = 0,
    _gui_dialog_choice_second = 1,
};

#include "interface/screen_transition_types.h"

class c_message
{
public:
    c_message(e_ui_message_type type, long screen_name, e_controller_index controller, e_window_index window);
    virtual ~c_message() {}
    virtual void initialize();
    virtual void update();

    e_ui_message_type get_type() const { return m_message_type; }
    long get_screen_name() const { return m_screen_name; }
    e_controller_index get_controller() const { return m_controller; }
    e_window_index get_window() const { return m_window; }
    long get_game_time_at_creation() const { return m_game_time_at_creation; }

    long m_game_time_at_creation;
    e_ui_message_type m_message_type;
    long m_screen_name;
    e_controller_index m_controller;
    e_window_index m_window;
};
typedef char user_interface_messages_size_c_message[(sizeof(void *) != 4 || ((sizeof(c_message) == 0x18) )) ? 1 : -1];

class c_controller_input_message : public c_message
{
public:
    c_controller_input_message(long screen_name, e_controller_index controller, e_window_index window,
        e_event_type event_type, e_controller_component component, long event_value);

    e_event_type get_event_type() const { return m_event_type; }
    e_controller_component get_component() const { return m_component; }
    long get_event_value() const { return m_event_value; }

    e_event_type m_event_type;
    e_controller_component m_component;
    long m_event_value;
    bool m_handled; // Set once the message has been handled; initially false
};
typedef char user_interface_messages_size_c_controller_input_message[(sizeof(void *) != 4 || ((sizeof(c_controller_input_message) == 0x28) )) ? 1 : -1];

class c_xenon_message : public c_message
{
public:
    enum e_xenon_message_type { k_xenon_message_type_count };

    c_xenon_message(e_controller_index controller, e_xenon_message_type xenon_message_type, long event_value);
    // Used by start-menu panes.
    c_xenon_message(long value, e_controller_index controller, e_window_index window, long event_value = -1);

    e_xenon_message_type get_xenon_message_type() const { return m_xenon_message_type; }
    long get_event_value() const { return m_event_value; }

    e_xenon_message_type m_xenon_message_type;
    long m_event_value;
};
typedef char user_interface_messages_size_c_xenon_message[(sizeof(void *) != 4 || ((sizeof(c_xenon_message) == 0x20) )) ? 1 : -1];

class c_dialog_result_message : public c_message
{
public:
    c_dialog_result_message(long screen_name, e_controller_index controller, e_window_index window, long dialog_name, e_gui_dialog_choice dialog_result);

    e_gui_dialog_choice get_dialog_result() const { return m_dialog_result; }
    long get_dialog_name() const { return m_dialog_name; }
    long get_dispose_on_success_screen_index() const { return m_dispose_on_success_screen_index; }
    void set_dispose_on_success_screen_index(long screen_index) { m_dispose_on_success_screen_index = screen_index; }

    e_gui_dialog_choice m_dialog_result;
    long m_dialog_name;
    long m_dispose_on_success_screen_index;
};
typedef char user_interface_messages_size_c_dialog_result_message[(sizeof(void *) != 4 || ((sizeof(c_dialog_result_message) == 0x24) )) ? 1 : -1];

class c_load_screen_message : public c_message
{
public:
    c_load_screen_message(long screen_name, e_controller_index controller, e_window_index window, long layered_position);
    virtual void apply_initial_state(c_gui_screen_widget *screen) const; // Empty in the base class.

    e_screen_transition_type get_transition_type() const { return m_transition_type; }
    bool get_respond_to_controller_events() const { return m_respond_to_controller_events; }
    long get_focus_on_load_list_name() const { return m_focus_on_load_list_name; }
    long get_focus_on_load_element_handle() const { return m_focus_on_load_element_handle; }
    long get_focus_on_load_column_name() const { return m_focus_on_load_column_name; }
    long get_focus_on_load_column_value() const { return m_focus_on_load_column_value; }
    long get_parent_screen_index() const { return m_parent_screen_index; }
    long get_layered_position() const { return m_layered_position; }
    bool get_applies_even_to_codeless_screens() const { return m_applies_even_to_codeless_screens; }

    void set_transition_type(e_screen_transition_type transition_type) { m_transition_type = transition_type; }
    void set_focus_on_load(long list_name, long element_handle)
    {
        m_focus_on_load_list_name = list_name;
        m_focus_on_load_element_handle = element_handle;
    }
    void set_focus_on_load_by_name(long list_name, long column_name, long column_value)
    {
        m_focus_on_load_list_name = list_name;
        m_focus_on_load_column_name = column_name;
        m_focus_on_load_column_value = column_value;
    }
    void set_parent_screen_index(long parent_screen_index) { m_parent_screen_index = parent_screen_index; }

    e_screen_transition_type m_transition_type;
    bool m_respond_to_controller_events;
    long m_focus_on_load_list_name;
    long m_focus_on_load_element_handle;
    long m_focus_on_load_column_name;
    long m_focus_on_load_column_value;
    long m_parent_screen_index;
    long m_layered_position;
    bool m_applies_even_to_codeless_screens;
};
typedef char user_interface_messages_size_c_load_screen_message[(sizeof(void *) != 4 || ((sizeof(c_load_screen_message) == 0x3c) )) ? 1 : -1];

class c_load_alert_screen_message : public c_load_screen_message
{
public:
    c_load_alert_screen_message(long screen_name, e_controller_index controller, e_window_index window, const c_gui_queued_error *error);
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;

    c_gui_queued_error m_error;
};
typedef char load_alert_error_offset[(sizeof(void *) != 4 || ((offsetof(c_load_alert_screen_message, m_error) == 0x3c))) ? 1 : -1];

class c_load_dialog_screen_message : public c_load_screen_message
{
public:
    c_load_dialog_screen_message(e_controller_index controller, e_window_index window, long layered_position, long dialog_name, long dialog_invoker);
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;

    void set_test_mode(bool test_mode) { m_test_mode = test_mode; }
    void set_dispose_on_success_screen_index(long screen_index) { m_dispose_on_success_screen_index = screen_index; }

    long m_dialog_name;
    long m_dialog_invoker;
    bool m_test_mode;
    long m_dispose_on_success_screen_index;

private:
    static long get_dialog_screen_name(long dialog_name);
};
typedef char user_interface_messages_size_c_load_dialog_screen_message[(sizeof(void *) != 4 || ((sizeof(c_load_dialog_screen_message) == 0x4c) )) ? 1 : -1];

class c_load_in_progress_screen_message : public c_load_screen_message
{
public:
    c_load_in_progress_screen_message(e_controller_index controller, e_window_index window, long title, long message);
    // Overload that also specifies the screen and display options.
    c_load_in_progress_screen_message(long screen_name, e_controller_index controller, e_window_index window,
        long title, long message, long respond_to_controller_events, long show_progress);
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;

    long m_title;
    long m_message;
    bool m_respond_to_controller_events; // Whether the screen responds to controller input; set by the full constructor above
    bool m_show_progress;
};
typedef char user_interface_messages_size_c_load_in_progress_screen_message[(sizeof(void *) != 4
    || ((sizeof(c_load_in_progress_screen_message) == 0x48) )) ? 1 : -1];

// Progress screen posted while a saved film loads or copies; it also sets the screen's alternate state.
class c_saved_film_load_in_progress_screen_message : public c_load_in_progress_screen_message
{
public:
    c_saved_film_load_in_progress_screen_message(long screen_name, e_controller_index controller, e_window_index window,
        long title, long message, long respond_to_controller_events, long show_progress)
        : c_load_in_progress_screen_message(screen_name, controller, window, title, message, respond_to_controller_events,
            show_progress)
        , m_alternate_state(false)
    {
    }
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;

    bool m_alternate_state;
};
typedef char user_interface_messages_size_c_saved_film_load_in_progress_screen_message[(sizeof(void *) != 4
    || ((sizeof(c_saved_film_load_in_progress_screen_message) == 0x4c) )) ? 1 : -1];


// Functions defined in tu/interfacelibxbox/user_interface_messages.cpp that other units call.

bool user_interface_message_queue_is_empty();
void user_interface_messaging_pop(const c_message *message);
void update_xenon_messages();
bool user_interface_messaging_get_next_message(long screen_name, e_controller_index controller, e_window_index window, const c_message **message_reference);

class c_message_globals;
extern c_message_globals g_message_globals;

#endif // INTERFACE_USER_INTERFACE_MESSAGES_H
