#ifndef INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_SCREENSHOT_VIEWER_H
#define INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_SCREENSHOT_VIEWER_H

#include "interface/c_gui_screen_widget.h"

class c_gui_screenshot_screen : public c_gui_screen_widget
{
public:
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);

protected:
    __forceinline c_gui_screenshot_screen(long name) : c_gui_screen_widget(name) {}
};

class c_gui_screen_screenshot_viewer : public c_gui_screenshot_screen
{
public:
    virtual bool can_receive_focus() const { return true; }
    virtual wchar_t *get_selected_item_title(wchar_t *output, long capacity) = 0;
    virtual wchar_t *get_selected_item_description(wchar_t *output, long capacity) = 0;

protected:
    __forceinline c_gui_screen_screenshot_viewer(long name) : c_gui_screenshot_screen(name) {}
};

#endif // INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_SCREENSHOT_VIEWER_H
