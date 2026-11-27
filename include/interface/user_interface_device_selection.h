#ifndef INTERFACE_USER_INTERFACE_DEVICE_SELECTION_H
#define INTERFACE_USER_INTERFACE_DEVICE_SELECTION_H

enum e_controller_index;

extern "C" void user_interface_show_device_selection_dialog(e_controller_index controller_index, long screen_name, long parent_screen_index);

#endif // INTERFACE_USER_INTERFACE_DEVICE_SELECTION_H
