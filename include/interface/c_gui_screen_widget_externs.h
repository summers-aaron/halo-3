#ifndef INTERFACE_C_GUI_SCREEN_WIDGET_EXTERNS_H
#define INTERFACE_C_GUI_SCREEN_WIDGET_EXTERNS_H

#include "math/real_math_types.h"
class c_gui_widget;
class c_gui_screen_widget;
class c_gui_list_widget;
class c_window_manager;
class c_controller_input_message;
class c_dialog_result_message;
class c_player_profile_interface;

extern "C" void gui_screen_widget_adjust_for_splitscreen(c_gui_screen_widget *screen);
extern "C" void gui_screen_widget_set_display_group(c_gui_screen_widget *screen, long type, long name);
extern "C" c_window_manager g_window_manager;
extern "C" void gui_widget_remove_child(c_gui_widget *parent, c_gui_widget *child);
extern "C" void gui_widget_append_child(c_gui_widget *parent, c_gui_widget *child);
extern "C" bool gui_screen_widget_spin_models_with_dpad(c_gui_screen_widget *screen, const c_controller_input_message *message);
extern "C" c_gui_widget *gui_widget_find_child_by_name(c_gui_widget *widget, long type);
extern "C" c_gui_list_widget *gui_widget_get_parent_list_widget(c_gui_widget *widget);
extern "C" c_gui_widget *gui_widget_get_next_sibling_of_type(c_gui_widget *widget, long type);
extern "C" void rasterizer_main_get_display_and_render_bounds(rectangle2d *display_bounds,
    rectangle2d *title_safe_bounds, rectangle2d *render_bounds,
    rectangle2d *render_title_safe_bounds);
extern "C" c_gui_widget *gui_widget_get_next_list_item(c_gui_widget *widget, bool require_valid);

extern "C" {
extern const wchar_t k_focused_item_tag_text[];
bool gui_screen_widget_get_focused_item_text(void *context, wchar_t *buffer, long buffer_length);
int global_preferences_get_subtitle_setting(c_player_profile_interface *profile);
int global_preferences_get_brightness_setting(void);
}

#endif
