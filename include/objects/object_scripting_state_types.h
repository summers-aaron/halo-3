#ifndef OBJECTS_OBJECT_SCRIPTING_STATE_TYPES_H
#define OBJECTS_OBJECT_SCRIPTING_STATE_TYPES_H

#include <stddef.h>

struct s_object_scripting_function_variable
{
    long variable_index;
    float start_value;
    float end_value;
    long start_time;
    long duration;
};
struct s_object_scripting_state {
    s_object_scripting_function_variable function_variables[32];
    long variable_object_indices[32];
    bool map_initialized;
};

typedef char object_scripting_variable_size[(sizeof(s_object_scripting_function_variable) == 20) ? 1 : -1];
typedef char object_scripting_indices_offset[(offsetof(s_object_scripting_state, variable_object_indices) == 0x280) ? 1 : -1];

#endif
