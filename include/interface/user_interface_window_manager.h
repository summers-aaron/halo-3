#ifndef INTERFACE_USER_INTERFACE_WINDOW_MANAGER_H
#define INTERFACE_USER_INTERFACE_WINDOW_MANAGER_H

#include "interface/c_window_manager.h"
#include "memory/data_types.h"
#include "shell/shell.h"
#include "main/loading.h"

#if defined(_XBOX) && !defined(__clang__)
#include "interface/user_interface_controller_globals.h"

class c_behavior_state;
// Xbox places these three independently owned globals consecutively.
struct s_loading_cache_controller_storage
{
    loading_map_path_cache cache;
    c_behavior_state *new_state_data;
    s_user_interface_controller_globals controllers;
};
typedef char loading_cache_new_state_offset[(offsetof(s_loading_cache_controller_storage, new_state_data) == 0x10c) ? 1 : -1];
typedef char loading_cache_controllers_offset[(offsetof(s_loading_cache_controller_storage, controllers) == 0x110) ? 1 : -1];
extern "C" s_loading_cache_controller_storage g_loading_cache_controller_storage;
#endif

// Functions defined in tu/interfacelibxbox/user_interface_window_manager.cpp.

class c_gui_screen_widget;

struct s_screen_handle : s_datum_header
{
    c_gui_screen_widget *screen;
};

extern "C" c_gui_screen_widget *window_manager_get_alert_screen(e_window_index window_index);
extern "C" void window_manager_play_input_sound(int override_index, int input_type, unsigned int input_component);
extern "C" void window_manager_add_widgets_to_render_list_recursive(void);

extern "C" void window_manager_close_screen_animated(c_window_manager *window_manager, c_gui_screen_widget *screen, int animation_mode);

union rectangle2d;
struct s_window_manager_screen_render_data;
class c_gui_widget;

extern "C" void window_manager_assemble_widget_render_data(
    const rectangle2d *viewport_bounds, c_gui_widget *widget,
    s_window_manager_screen_render_data *render_data, long controller_index);
extern "C" void window_manager_render_window(
    c_window_manager *window_manager, e_window_index window_index, long user_index,
    const rectangle2d *window_bounds, bool is_screenshot);

// The singleton used by window-manager methods and their GUI callers.
extern "C" c_window_manager g_window_manager;
extern "C" bool window_manager_is_controller_input_suppressed(c_window_manager *window_manager, e_controller_index controller_index);

extern "C" const char k_file_share_keyboard_task_name[];

#endif // INTERFACE_USER_INTERFACE_WINDOW_MANAGER_H
