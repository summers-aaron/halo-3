#ifndef SCENARIO_SCENARIO_INTERPOLATOR_DEFINITION_H
#define SCENARIO_SCENARIO_INTERPOLATOR_DEFINITION_H

#include <stddef.h>
#include "math/function_definitions.h"

struct s_scenario_interpolator_definition
{
    long name;
    unsigned char data_0004[8];
    c_function_definition function;
    short source_index;
    short other_index;
};

typedef char scenario_interpolator_function_offset[(offsetof(s_scenario_interpolator_definition, function) == 12) ? 1 : -1];
#endif
