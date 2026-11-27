#ifndef NETWORKING_SIMULATION_SIMULATION_EVENT_CONFIGURATION_H
#define NETWORKING_SIMULATION_SIMULATION_EVENT_CONFIGURATION_H

#include <stddef.h>

// The event configuration table holds 34 consecutive records with this layout.
// simulation_event_handler reads relevance/input/minimum/maximum; game action
// producers read the intervening delay word (damage aftermath: +0x478).
struct s_simulation_event_configuration
{
    float relevance;
    union
    {
        long delay_msec;
        long delay;
    };
    float input;
    float minimum;
    float maximum;
};

typedef char simulation_event_configuration_size[(sizeof(s_simulation_event_configuration) == 0x14) ? 1 : -1];
typedef char simulation_event_configuration_delay_offset[(offsetof(s_simulation_event_configuration, delay_msec) == 4) ? 1 : -1];
typedef char simulation_event_configuration_input_offset[(offsetof(s_simulation_event_configuration, input) == 8) ? 1 : -1];

extern "C" const float k_projectile_attached_minimum_relevance;
extern "C" const float k_player_respawn_request_relevance;

#endif
