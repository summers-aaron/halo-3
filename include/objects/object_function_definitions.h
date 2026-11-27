#ifndef OBJECTS_OBJECT_FUNCTION_DEFINITIONS_H
#define OBJECTS_OBJECT_FUNCTION_DEFINITIONS_H

#include <stddef.h>
#include "math/function_definitions.h"
#include "math/function_definition_types.h"

struct s_object_function_definition
{
    unsigned long flags;
    long scripting_name;
    long import_name;
    long turn_off_with;
    float min_value;
    c_function_definition function;
    long scale_by;
};

typedef char object_function_definition_size[(sizeof(s_object_function_definition) == 0x2c) ? 1 : -1];
typedef char object_function_definition_function_offset[(offsetof(s_object_function_definition, function) == 0x14) ? 1 : -1];
typedef char object_function_definition_scale_offset[(offsetof(s_object_function_definition, scale_by) == 0x28) ? 1 : -1];

#endif // OBJECTS_OBJECT_FUNCTION_DEFINITIONS_H
