#ifndef NETWORKING_LOGIC_NETWORK_BANDWIDTH_GLOBALS_H
#define NETWORKING_LOGIC_NETWORK_BANDWIDTH_GLOBALS_H

#include <stddef.h>

class c_network_observer;
struct s_bandwidth_configuration;
struct s_bandwidth_history {
  long qos_count;
  long qos[8];
  long measurement_count;
  long good[8];
  long unsafe[8];
  long disputes;
};


struct s_bandwidth_globals
{
    bool initialized;
    c_network_observer *observer;
    s_bandwidth_configuration *settings;
    s_bandwidth_history *history;
    bool online_network_environment;
    bool tracking;
    bool restricted;
    long last_time;
    bool estimated;
    long bandwidth;
    long capacity;
};

typedef char bandwidth_globals_size[(sizeof(void *) != 4 || ((sizeof(s_bandwidth_globals) == 0x24))) ? 1 : -1];
typedef char bandwidth_globals_observer_offset[(sizeof(void *) != 4 || ((offsetof(s_bandwidth_globals, observer) == 4))) ? 1 : -1];
typedef char bandwidth_globals_last_time_offset[(sizeof(void *) != 4 || ((offsetof(s_bandwidth_globals, last_time) == 0x14))) ? 1 : -1];
typedef char bandwidth_globals_bandwidth_offset[(sizeof(void *) != 4 || ((offsetof(s_bandwidth_globals, bandwidth) == 0x1c))) ? 1 : -1];

extern "C" s_bandwidth_globals network_bandwidth_globals;

#endif
