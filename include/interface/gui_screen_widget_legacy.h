#include "interface/c_gui_screen_widget.h"
#ifndef INTERFACE_GUI_SCREEN_WIDGET_LEGACY_H
#define INTERFACE_GUI_SCREEN_WIDGET_LEGACY_H

class c_gui_screen_widget;
class c_gui_widget;

// Legacy helper names delegate to the canonical screen-widget methods.
static __forceinline void gui_screen_widget_initialize(c_gui_screen_widget *screen)
{
    ((c_gui_screen_widget *)(screen))->c_gui_screen_widget::initialize();
}
static __forceinline void gui_screen_widget_initialize_datasource(c_gui_screen_widget *screen)
{
    ((c_gui_screen_widget *)(screen))->c_gui_screen_widget::initialize_datasource();
}
static __forceinline void gui_screen_widget_update(c_gui_screen_widget *screen, unsigned long current_milliseconds)
{
    ((c_gui_screen_widget *)(screen))->c_gui_screen_widget::update(current_milliseconds);
}
template<typename t_screen>
static __forceinline void gui_screen_widget_transfer_focus_without_animations(t_screen *screen, c_gui_widget *widget, bool animate, bool restart)
{
    screen->transfer_focus_without_animations(widget, animate, restart);
}

#endif // INTERFACE_GUI_SCREEN_WIDGET_LEGACY_H
