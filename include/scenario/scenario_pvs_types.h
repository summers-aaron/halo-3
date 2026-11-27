#ifndef SCENARIO_SCENARIO_PVS_TYPES_H
#define SCENARIO_SCENARIO_PVS_TYPES_H

#include <stddef.h>
#include "memory/static_arrays.h"
#include "tag_files/tag_types.h"

struct s_scenario_pvs_bit_vector
{
    s_tag_block bit_vector;
};

struct s_scenario_cluster_pvs
{
    s_tag_block pvs;
};

struct s_scenario_pvs_row
{
    c_static_array<unsigned long *, 16> vectors;
    c_static_array<long, 16> counts;
};

#if defined(_XBOX)
typedef char scenario_pvs_row_size[(sizeof(void *) != 4 || ((sizeof(s_scenario_pvs_row) == 128) )) ? 1 : -1];
typedef char scenario_pvs_row_counts_offset[(sizeof(void *) != 4 || ((offsetof(s_scenario_pvs_row, counts) == 64) )) ? 1 : -1];
#endif

#endif // SCENARIO_SCENARIO_PVS_TYPES_H
