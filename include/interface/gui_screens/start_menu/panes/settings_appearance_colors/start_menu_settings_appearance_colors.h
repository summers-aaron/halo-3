#ifndef INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_APPEARANCE_COLORS_START_MENU_SETTINGS_APPEARANCE_COLORS_H
#define INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_APPEARANCE_COLORS_START_MENU_SETTINGS_APPEARANCE_COLORS_H

// Functions defined in tu/interfacelibxbox/start_menu_settings_appearance_colors.cpp.

#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/c_gui_list_item_widget.h"

class c_start_menu_settings_appearance_colors : public c_start_menu_pane_screen_widget
{
public:
    c_start_menu_settings_appearance_colors(long name);
    virtual void initialize();
    virtual void post_initialize();
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
protected:
    virtual c_gui_list_item_widget *create_list_item_widget(const s_list_item_widget_block *list_item_widget_block);
private:
    void set_color_values_from_profile();
    void update_model_widget();
    void update_color_picker_enabled_state();
};

enum e_appearance_color_selector
{
    _appearance_color_primary = 0x10129,
    _appearance_color_secondary = 0x1012a,
    _appearance_color_tertiary = 0x1012b,
    _appearance_color_emblem_primary = 0x1012c,
    _appearance_color_emblem_secondary = 0x1012d,
    _appearance_color_emblem_background = 0x1012e,
};

extern "C" bool start_menu_settings_appearance_colors_get_primary(void *context, wchar_t *buffer, long count);
extern "C" bool start_menu_settings_appearance_colors_get_secondary(void *context, wchar_t *buffer, long count);
extern "C" bool start_menu_settings_appearance_colors_get_tertiary(void *context, wchar_t *buffer, long count);
extern "C" bool start_menu_settings_appearance_colors_get_emblem_primary(void *context, wchar_t *buffer, long count);
extern "C" bool start_menu_settings_appearance_colors_get_emblem_secondary(void *context, wchar_t *buffer, long count);
extern "C" bool start_menu_settings_appearance_colors_get_emblem_background(void *context, wchar_t *buffer, long count);
extern "C" bool start_menu_settings_appearance_colors_get_focused_color_text(void *context, wchar_t *buffer, long count, int widget_name);
extern "C" bool start_menu_settings_appearance_colors_get_focused_item_text(void *context, wchar_t *output, long capacity);

// Game tag registered by the focused-item text parser.
extern "C" {
extern const wchar_t k_appearance_focused_item_tag[];
}

#endif // INTERFACE_GUI_SCREENS_START_MENU_PANES_SETTINGS_APPEARANCE_COLORS_START_MENU_SETTINGS_APPEARANCE_COLORS_H
