#ifndef INTERFACE_GUI_SCREENS_C_GUI_SCREEN_MAXIMUM_PARTY_SIZE_H
#define INTERFACE_GUI_SCREENS_C_GUI_SCREEN_MAXIMUM_PARTY_SIZE_H

#include "interface/c_gui_screen_widget.h"

class c_gui_screen_maximum_party_size : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_maximum_party_size(long name) : c_gui_screen_widget(name) {}
    virtual void initialize();
    virtual void update(unsigned long current_milliseconds);
    void update_availability();
    virtual void post_initialize();
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
};

typedef char c_gui_screen_maximum_party_size_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_maximum_party_size) == 0x1a88))) ? 1 : -1];

#endif
