#ifndef INTERFACE_USER_INTERFACE_CONTROLLER_GLOBALS_H
#define INTERFACE_USER_INTERFACE_CONTROLLER_GLOBALS_H

#include <stddef.h>
#include "interface/event_manager_types.h"

struct s_user_interface_controller
{
    unsigned long flags;
    unsigned char hopper_access_flags;
    unsigned char desires_veto;
    unsigned char desires_rematch;
    unsigned char griefer;
    unsigned char notification_pending;
    char reserved09[7];
    unsigned char reserved10;
    unsigned char reserved11[8];
};

struct s_user_interface_controller_globals
{
    s_user_interface_controller controllers[4];
    __declspec(align(8)) s_event_record events[4];
    unsigned char error_posted[4];
    unsigned char inhibited;
};

extern "C" s_user_interface_controller_globals g_user_interface_controller_globals;

typedef char ui_controller_size[(sizeof(s_user_interface_controller) == 0x1c) ? 1 : -1];
typedef char ui_controller_events_offset[(offsetof(s_user_interface_controller_globals, events) == 0x70) ? 1 : -1];
typedef char ui_controller_errors_offset[(offsetof(s_user_interface_controller_globals, error_posted) == 0xb0) ? 1 : -1];

#endif
