#ifndef INTERFACE_C_CONTROLLER_H
#define INTERFACE_C_CONTROLLER_H

#include "shell/shell.h"

// Functions defined in tu/interfacelibxbox/c_controller.cpp.

extern "C" void controller_update_all(unsigned long milliseconds);
extern "C" void controller_update_signin_cache(void);
extern "C" unsigned long controller_get_signed_in_mask1();
extern "C" unsigned long controller_get_signed_in_mask();
extern "C" void controller_handle_xenon_messages();

struct c_controller_interface;

extern "C" bool controller_is_live_gold_nonguest(c_controller_interface *controller);
extern "C" short controller_get_or_allocate_user_index(c_controller_interface *controller);
extern "C" e_controller_index controller_get_first_signed_in_non_guest();
extern "C" bool controller_is_live_content_available(e_controller_index controller_index, long *out_fallback);
e_controller_index controller_get_first_signed_in_controller();
bool voice_device_is_connected(e_controller_index controller_index);

extern "C" bool g_controller_silver_or_gold_live_cache[4];
extern "C" bool g_controller_live_gold_nonguest_cache[4];

#endif // INTERFACE_C_CONTROLLER_H
