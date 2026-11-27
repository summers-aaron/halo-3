#ifndef OBJECTS_OBJECT_MARKER_GLOBALS_TYPES_H
#define OBJECTS_OBJECT_MARKER_GLOBALS_TYPES_H
#include <stddef.h>
#include "memory/static_arrays.h"

struct s_object_marker_globals
{
    bool active;
    long mark;
    union
    {
        long marks[4096];
        struct
        {
            c_static_array<long, 2048> object_marks;
            c_static_array<long, 2048> mark_data;
        };
    };
};
typedef char object_marker_globals_size[(sizeof(s_object_marker_globals) == 0x4008) ? 1 : -1];
typedef char object_marker_data_offset[(offsetof(s_object_marker_globals, mark_data) == 0x2008) ? 1 : -1];
#endif // OBJECTS_OBJECT_MARKER_GLOBALS_TYPES_H
