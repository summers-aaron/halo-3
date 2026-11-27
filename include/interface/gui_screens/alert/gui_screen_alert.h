#ifndef INTERFACE_GUI_SCREENS_ALERT_GUI_SCREEN_ALERT_H
#define INTERFACE_GUI_SCREENS_ALERT_GUI_SCREEN_ALERT_H
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_render_data_size_types.h"
#include "interface/user_interface_error_manager.h"

class c_gui_alert_screen_widget : public c_gui_screen_widget
{
public:
    __forceinline c_gui_alert_screen_widget(long name) : c_gui_screen_widget(name), m_error() {}
    virtual ~c_gui_alert_screen_widget() {}
    virtual void post_initialize() { c_gui_screen_widget::post_initialize(); }
    virtual void update(unsigned long current_milliseconds);
protected:
    virtual e_render_data_size get_render_data_size();
public:
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    c_gui_queued_error m_error;
};
typedef char gui_alert_error_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_alert_screen_widget, m_error) == 0x1a88))) ? 1 : -1];
typedef char gui_alert_size[(sizeof(void *) != 4 || ((sizeof(c_gui_alert_screen_widget) == 0x1eb0))) ? 1 : -1];
#endif
