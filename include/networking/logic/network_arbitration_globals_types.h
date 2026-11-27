#ifndef NETWORKING_LOGIC_NETWORK_ARBITRATION_GLOBALS_TYPES_H
#define NETWORKING_LOGIC_NETWORK_ARBITRATION_GLOBALS_TYPES_H

#include "memory/data_types.h"
#include "networking/online/online_arbitration_types.h"
#include <stddef.h>

struct s_network_arbitration_globals
{
    bool initialized;
    long index;
    long status;
    s_online_arbitration_result result;
};

typedef char network_arbitration_globals_result_offset[(offsetof(s_network_arbitration_globals, result) == 16) ? 1 : -1];
typedef char network_arbitration_globals_size[(sizeof(s_network_arbitration_globals) == 0x198) ? 1 : -1];

#endif // NETWORKING_LOGIC_NETWORK_ARBITRATION_GLOBALS_TYPES_H
