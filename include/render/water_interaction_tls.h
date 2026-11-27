#ifndef RENDER_WATER_INTERACTION_TLS_H
#define RENDER_WATER_INTERACTION_TLS_H

#include "math/real_math_types.h"
#include "memory/static_arrays.h"

struct s_water_interaction_ripple
{
    float amplitude;
    long key;
    long start_time;
    short offset;
    short period;
};

struct s_water_interaction_data
{
    s_water_interaction_ripple ripples[32];
    float ripple_amplitudes[30];
    c_static_flags<32> active_ripples;
};

typedef char water_interaction_ripple_size[(sizeof(s_water_interaction_ripple) == 0x10) ? 1 : -1];
typedef char water_interaction_data_size[(sizeof(s_water_interaction_data) == 0x27c) ? 1 : -1];

struct s_water_interaction_event
{
    real_point3d position;
    real_vector3d direction;
    real_vector3d velocity;
    long type;
};

typedef char water_interaction_event_size[(sizeof(s_water_interaction_event) == 40) ? 1 : -1];

extern "C" {
extern __declspec(thread) s_water_interaction_data *g_water_interaction_data_address;
extern __declspec(thread) s_water_interaction_event *g_water_interaction_events;
}

#endif
