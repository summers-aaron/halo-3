#ifndef SCENARIO_SCENARIO_INTERPOLATOR_TYPES_H
#define SCENARIO_SCENARIO_INTERPOLATOR_TYPES_H

#include <stddef.h>

struct s_scenario_interpolator_state
{
    float current;
    float start;
    float target;
    float last_time;
    float start_time;
    float end_time;
    float integrated;
    unsigned char flags;
};

struct s_scenario_interpolator_globals
{
    long name;
    s_scenario_interpolator_state states[16];
};

typedef char scenario_interpolator_state_size[(sizeof(s_scenario_interpolator_state) == 32) ? 1 : -1];
typedef char scenario_interpolator_flags_offset[(offsetof(s_scenario_interpolator_state, flags) == 28) ? 1 : -1];
typedef char scenario_interpolator_globals_size[(sizeof(s_scenario_interpolator_globals) == 516) ? 1 : -1];

#endif // SCENARIO_SCENARIO_INTERPOLATOR_TYPES_H
