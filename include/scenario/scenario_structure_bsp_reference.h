#ifndef SCENARIO_SCENARIO_STRUCTURE_BSP_REFERENCE_H
#define SCENARIO_SCENARIO_STRUCTURE_BSP_REFERENCE_H

#include <stddef.h>

#include "tag_files/tag_types.h"

struct scenario_structure_bsp_reference
{
    s_tag_reference structure_bsp;
    s_tag_reference structure_design;
    char spare_before_default_sky_index[0x42 - 32];
    short default_sky_index;
    short fade_start;
    short fade_end;
    s_tag_reference decorator_model;
    s_tag_reference wind;
    char spare_after_wind[108 - 0x68];
};

typedef char scenario_structure_fade_start_offset[(offsetof(scenario_structure_bsp_reference, fade_start) == 0x44) ? 1 : -1];
typedef char scenario_structure_fade_end_offset[(offsetof(scenario_structure_bsp_reference, fade_end) == 0x46) ? 1 : -1];

typedef char scenario_structure_bsp_reference_size[(sizeof(scenario_structure_bsp_reference) == 108) ? 1 : -1];
typedef char scenario_structure_bsp_reference_decorator_offset[(offsetof(scenario_structure_bsp_reference, decorator_model.index) == 0x54) ? 1 : -1];
typedef char scenario_structure_bsp_reference_wind_offset[(offsetof(scenario_structure_bsp_reference, wind.index) == 0x64) ? 1 : -1];
typedef char scenario_structure_bsp_reference_tag_index_offset[(offsetof(scenario_structure_bsp_reference, structure_bsp.index) == 12) ? 1 : -1];
// Water physics reads the structure design index and
// passes that index to tag_get('sddt', index).
typedef char scenario_structure_bsp_reference_design_index_offset[(offsetof(scenario_structure_bsp_reference, structure_design.index) == 28) ? 1 : -1];

#endif // SCENARIO_SCENARIO_STRUCTURE_BSP_REFERENCE_H
