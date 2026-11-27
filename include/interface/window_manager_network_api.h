#ifndef INTERFACE_WINDOW_MANAGER_NETWORK_API_H
#define INTERFACE_WINDOW_MANAGER_NETWORK_API_H

class c_window_manager;
enum e_controller_index;

extern "C" bool window_manager_is_controller_ui_active(c_window_manager *window_manager, e_controller_index controller_index);

#endif // INTERFACE_WINDOW_MANAGER_NETWORK_API_H
