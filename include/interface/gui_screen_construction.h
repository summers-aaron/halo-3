#ifndef INTERFACE_GUI_SCREEN_CONSTRUCTION_H
#define INTERFACE_GUI_SCREEN_CONSTRUCTION_H

class c_gui_screen_widget;

// Out-of-line construction entry points used by the window manager.
// The boot-betrayer constructor installs the boot-betrayer vtable.
extern "C" c_gui_screen_widget *gui_screen_boot_betrayer_construct(c_gui_screen_widget *self, long name);

#endif // INTERFACE_GUI_SCREEN_CONSTRUCTION_H
