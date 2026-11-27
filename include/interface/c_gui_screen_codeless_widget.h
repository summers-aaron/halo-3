#ifndef INTERFACE_C_GUI_SCREEN_CODELESS_WIDGET_H
#define INTERFACE_C_GUI_SCREEN_CODELESS_WIDGET_H

#include "interface/c_gui_screen_widget.h"

// Adds the test datasource to a screen loaded directly from its definition.
class c_gui_screen_codeless_widget : public c_gui_screen_widget
{
public:
    __forceinline c_gui_screen_codeless_widget(long name) : c_gui_screen_widget(name)
    {
        m_codeless_mode = true;
    }
    virtual void initialize_datasource();
};

#endif
