#ifndef NETWORKING_TRANSPORT_TRANSPORT_TYPES_H
#define NETWORKING_TRANSPORT_TRANSPORT_TYPES_H

#include <stddef.h>

typedef void (*transport_transition_function_t)(void *);

// Transport globals hold init state, a function count, and transition tables.
// register/reset routines attest the count and four eight-entry tables.
struct s_transport_globals
{
    bool initialized;
    bool started;
    bool network_available;
    long transition_function_count;
    transport_transition_function_t startup_functions[8];
    transport_transition_function_t shutdown_functions[8];
    transport_transition_function_t reset_functions[8];
    void *callback_datas[8];
};

typedef char transport_globals_initialized_offset[(offsetof(s_transport_globals, initialized) == 0) ? 1 : -1];
typedef char transport_globals_started_offset[(offsetof(s_transport_globals, started) == 1) ? 1 : -1];
typedef char transport_globals_network_available_offset[(offsetof(s_transport_globals, network_available) == 2) ? 1 : -1];
typedef char transport_globals_count_offset[(offsetof(s_transport_globals, transition_function_count) == 4) ? 1 : -1];
#if !defined(_WIN64)
typedef char transport_globals_startup_offset[(offsetof(s_transport_globals, startup_functions) == 8) ? 1 : -1];
typedef char transport_globals_shutdown_offset[(offsetof(s_transport_globals, shutdown_functions) == 0x28) ? 1 : -1];
typedef char transport_globals_reset_offset[(offsetof(s_transport_globals, reset_functions) == 0x48) ? 1 : -1];
typedef char transport_globals_context_offset[(offsetof(s_transport_globals, callback_datas) == 0x68) ? 1 : -1];
typedef char transport_globals_size[(sizeof(s_transport_globals) == 0x88) ? 1 : -1];
#endif

#endif // NETWORKING_TRANSPORT_TRANSPORT_TYPES_H
