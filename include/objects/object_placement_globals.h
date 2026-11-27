#ifndef OBJECTS_OBJECT_PLACEMENT_GLOBALS_H
#define OBJECTS_OBJECT_PLACEMENT_GLOBALS_H

#include "cseries/cseries.h"

struct s_data_array;

// March objects\object_placement.cpp:153 reaches the globals through
// object_placement_globals->placed_structure_bsp_mask; the allocation is 4 bytes.
struct s_object_placement_globals {
    unsigned long placed_structure_bsp_mask;
};
typedef char object_placement_globals_size[(sizeof(s_object_placement_globals) == 4) ? 1 : -1];

extern "C" {
extern __declspec(thread) s_object_placement_globals *g_object_placement_globals_address;
extern __declspec(thread) s_data_array *g_object_activation_regions;
}

extern c_static_array<long, 14> g_scenario_object_offsets;

#endif // OBJECTS_OBJECT_PLACEMENT_GLOBALS_H
