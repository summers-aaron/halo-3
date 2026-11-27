#ifndef NETWORKING_NETWORK_TIME_TYPES_H
#define NETWORKING_NETWORK_TIME_TYPES_H

#include <stddef.h>

class c_network_session_manager;

// Network time state: March network_time.obj uses +0, +4 and +8 of one global.
// The private two-field copies omitted the session manager used by network_time.cpp.
struct s_network_time_override
{
    bool enabled;
    unsigned long time;
    c_network_session_manager *session_manager;
};

// Network time override storage at 0x828c447c (owner operations in network_globals.cpp).
extern "C" s_network_time_override g_network_time_globals;

typedef char s_network_time_override_enabled_offset[(offsetof(s_network_time_override, enabled) == 0) ? 1 : -1];
typedef char s_network_time_override_time_offset[(offsetof(s_network_time_override, time) == 4) ? 1 : -1];
typedef char s_network_time_override_session_manager_offset[(offsetof(s_network_time_override, session_manager) == 8) ? 1 : -1];
#if !defined(_WIN64)
typedef char s_network_time_override_size[(sizeof(s_network_time_override) == 12) ? 1 : -1];
#endif

#endif // NETWORKING_NETWORK_TIME_TYPES_H
