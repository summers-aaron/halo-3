#ifndef INTERFACE_C_WINDOW_MANAGER_H
#define INTERFACE_C_WINDOW_MANAGER_H

#include "shell/shell_types.h"

#include "interface/gui_widget_enums.h"

#include "interface/screen_transition_types.h"

#include "math/real_math_types.h"
#include "multithreading/synchronized_value.h"
#include <stddef.h>

class c_gui_widget;
class c_gui_screen_widget;
class c_load_screen_message;
struct s_data_array;
struct s_event_record;

// March lists 38 methods, all nonvirtual; its asserts name user_interface_window_manager.cpp,
// not a header. user_interface_window_manager.h includes this definition beside its free functions.
// Tracks the last update time used for sound fade timing.
class c_window_manager
{
public:
    enum e_widget_debug_state
    {
        _widget_debug_name,
        _widget_debug_bounds,
        _widget_debug_animation_state,
        _widget_debug_rotation_origin
    };

    c_window_manager();
    ~c_window_manager() {}
    void initialize();
    void dispose();
    void initialize_for_new_map();
    void dispose_from_old_map();
    void prepare_for_tag_reload();
    void update(unsigned long current_milliseconds);
    void handle_global_controller_event(s_event_record *event);
    void render(e_window_index window_index, long user_index, const rectangle2d *bounds, bool is_screenshot);
    bool controller_input_should_be_suppressed(e_controller_index controller_index);
    bool any_ui_active();
    bool any_ui_active_in_window(e_window_index window_index);
    // Locates a screen definition tag by name.
    static long locate_screen_definition_tag_by_name(long name);
    e_window_index get_render_window_for_screen(c_gui_screen_widget *screen)
    {
        e_window_index result = _window_none;
        for (long window_index = 0; window_index < 5; ++window_index)
            for (long screen_index = 0; screen_index < m_screen_count[window_index]; ++screen_index)
                if (screen == m_screens[window_index][screen_index])
                {
                    result = (e_window_index)window_index;
                    break;
                }
        return result;
    }
    c_gui_screen_widget *get_screen_by_name(e_window_index window_index, long name);
    c_gui_screen_widget *get_screen_by_screen_index(long screen_index);
    c_gui_screen_widget *get_responding_screen_in_window(e_controller_index controller_index, e_window_index window_index);
    c_gui_screen_widget *get_responding_screen(e_controller_index controller_index, e_window_index *window_index);
    // Tests whether a named screen is already queued to load in a window.
    static bool is_screen_queued_to_be_loaded(long name, e_window_index window_index);
    void begin_transition_out(c_gui_screen_widget *screen, e_screen_transition_type transition_type);
    void dispose_screen(c_gui_screen_widget *screen);
    void print_active_screens();
    void debug_widget_state(e_gui_widget_type type, long name, e_widget_debug_state state, bool enabled, bool recursive);
    void debug_widget_state_for_screen(c_gui_screen_widget *screen, e_gui_widget_type type, long name,
        e_widget_debug_state state, bool enabled, bool recursive);
    void debug_widget_state_for_widget(c_gui_widget *widget, e_widget_debug_state state, bool enabled, bool recursive);
    void reset_screens();
    // Closes every screen except the listed exclusions.
    void close_all_screens(c_gui_screen_widget *const *excluded_screens, long excluded_screen_count);
    c_gui_screen_widget *get_topmost_screen(e_window_index window_index)
    {
        c_gui_screen_widget *result = 0;
        if (m_screen_count[window_index] > 0)
            result = m_screens[window_index][m_screen_count[window_index] - 1];
        return result;
    }
    c_gui_screen_widget *get_screen_below(e_window_index window_index, c_gui_screen_widget *screen);
    c_gui_screen_widget *get_bottommost_screen(e_window_index window_index)
    {
        c_gui_screen_widget *result = 0;
        if (m_screen_count[window_index] > 0)
            result = m_screens[window_index][0];
        return result;
    }
    c_gui_screen_widget *get_screen_above(e_window_index window_index, c_gui_screen_widget *screen);

private:
    void process_unhandled_events(unsigned long current_milliseconds);
    c_gui_screen_widget *load_screen(e_controller_index controller_index, bool unused, const c_load_screen_message *message, e_window_index window_index);
    c_gui_screen_widget *allocate_named_screen(long name);
    static bool named_screen_defined_in_code(long name);
    c_gui_screen_widget *allocate_codeless_screen(long name);
    void insert_screen_after_named_screen(e_window_index window_index, c_gui_screen_widget *screen, long name);

public:
    s_data_array *m_active_screens; // March initialize asserts this name
    unsigned long m_last_update_time; // last update time for sound fade timing
    c_gui_screen_widget *m_screens[5][10];
    c_synchronized_long m_screen_count[5];
    rectangle2d m_window_bounds[5];
    long m_controller_screen_counts[4];
    long m_script_thread_index;
    float m_fade; // sound fade interpolation
    bool m_fade_in;
    bool m_fade_held;
};

typedef char window_manager_size[(sizeof(void *) != 4 || ((sizeof(c_window_manager) == 0x128) )) ? 1 : -1];
typedef char window_manager_screens_offset[(sizeof(void *) != 4 || ((offsetof(c_window_manager, m_screens) == 0x8) )) ? 1 : -1];
typedef char window_manager_count_offset[(sizeof(void *) != 4 || ((offsetof(c_window_manager, m_screen_count) == 0xd0) )) ? 1 : -1];
typedef char window_manager_fade_offset[(sizeof(void *) != 4 || ((offsetof(c_window_manager, m_fade) == 0x120) )) ? 1 : -1];

extern "C" c_window_manager g_window_manager;

#endif // INTERFACE_C_WINDOW_MANAGER_H
