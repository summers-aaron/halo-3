#ifndef NETWORKING_ONLINE_ONLINE_GLOBALS_TYPES_H
#define NETWORKING_ONLINE_ONLINE_GLOBALS_TYPES_H

#include "interface/user_interface_message_globals.h"

static __forceinline bool online_guide_is_active()
{
    return g_message_globals.flag;
}

#endif // NETWORKING_ONLINE_ONLINE_GLOBALS_TYPES_H
