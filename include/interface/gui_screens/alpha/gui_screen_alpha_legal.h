#ifndef INTERFACE_GUI_SCREENS_ALPHA_GUI_SCREEN_ALPHA_LEGAL_H
#define INTERFACE_GUI_SCREENS_ALPHA_GUI_SCREEN_ALPHA_LEGAL_H

#include "interface/c_gui_screen_widget.h"

class c_gui_screen_alpha_legal : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_alpha_legal(long name) : c_gui_screen_widget(name),
        m_timeout_time(0x7fffffffL), m_bink_active(false), m_dismissed(false) {}
    virtual ~c_gui_screen_alpha_legal() {}
    void start_timeout();
    virtual void update(unsigned long current_milliseconds);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);

private:
    long m_timeout_time;
    bool m_bink_active;
    bool m_dismissed;
};

typedef char gui_screen_alpha_legal_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_alpha_legal) == 0x1a90))) ? 1 : -1];

#endif
