#ifndef INTERFACE_GUI_SCREENS_SCREEN_ERROR_DIALOG_OK_H
#define INTERFACE_GUI_SCREENS_SCREEN_ERROR_DIALOG_OK_H

#include "cseries/cseries.h"
#include "interface/c_gui_screen_widget.h"

// The "error_ok" screen: an error dialog whose focused-widget selection is always handled.
class c_error_dialog_screen_widget : public c_gui_screen_widget
{
public:
    __forceinline c_error_dialog_screen_widget(long name) : c_gui_screen_widget(name), m_1a88(NONE), m_1a8c()
    {
        m_1a8c.index = NONE;
        m_1a8c.second = NONE;
    }
    virtual ~c_error_dialog_screen_widget() {}

protected:
    long m_1a88;
    struct
    {
        short index;
        long second;
    } m_1a8c;
    long m_1a94;
};

class c_error_dialog_ok_screen_widget : public c_error_dialog_screen_widget
{
public:
    __forceinline c_error_dialog_ok_screen_widget(long name) : c_error_dialog_screen_widget(name) {}
    virtual ~c_error_dialog_ok_screen_widget() {}
    virtual bool handle_focused_widget_selected(const c_controller_input_message *message, c_gui_widget *widget) { return true; }
};

typedef char c_error_dialog_ok_screen_widget_size[(sizeof(void *) != 4 || ((sizeof(c_error_dialog_ok_screen_widget) == 0x1a98))) ? 1 : -1];

#endif // INTERFACE_GUI_SCREENS_SCREEN_ERROR_DIALOG_OK_H
