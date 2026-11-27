#ifndef AI_CS_SCENARIO_DEFINITIONS_H
#define AI_CS_SCENARIO_DEFINITIONS_H

// Command-script points in the scenario tag (ai\cs_scenario_definitions.cpp in the March 2007 debug build:
// cs_point_set_index_by_name, cs_point_index_by_name).
//
// March cs_point_index_by_name walks point_set->points with tag_block_get_element_with_size(&points, i, 60) and
// compares the name at +0; cl_engine.obj cs_scenario_get_script_data reads element 0 (132 bytes) of
// scenario->script_data and cs_get_point_set reads point_sets with an element size of 52.

#include "tag_files/tag_types.h"
#include "math/real_math_types.h"
#include "ai/ai_point3d.h"
#include "ai/sector_ref.h"
#include "scenario/scenario_access.h"

struct cs_point
{
    char name[32];
    c_ai_point3d position;                        // point, reference frame, structure bsp
    c_sector_ref sector;                          // ai_atoms.cpp tests it with valid() before moving to the point
    real_euler_angles2d facing;                   // vector3d_from_euler_angles2d(&facing) in cs_start_custom_animation_internal
};

typedef char cs_point_size[(sizeof(cs_point) == 0x3c) ? 1 : -1];

struct cs_point_set
{
    char name[32];
    s_tag_block points;                           // cs_point
    short structure_bsp_index;
    short manual_reference_frame;
    long flags;
};

typedef char cs_point_set_size[(sizeof(cs_point_set) == 0x34) ? 1 : -1];

// The tag layout pads the element to 132 bytes after the block.
struct cs_script_data
{
    s_tag_block point_sets;                       // cs_point_set
    char spare_after_point_sets[0x84 - 0x0c];
};

typedef char cs_script_data_size[(sizeof(cs_script_data) == 0x84) ? 1 : -1];

// These two helpers fetch script data and point sets; they
// sit beside the types until cl_engine.cpp, which still has its own copy of cs_get_point_set, adopts this header.
static __forceinline cs_script_data *cs_scenario_get_script_data(scenario *scenario)
{
    return TAG_BLOCK_GET_ELEMENT(&scenario->script_data, 0, cs_script_data);
}

static __forceinline cs_point_set *cs_get_point_set(long point_set_index)
{
    return TAG_BLOCK_GET_ELEMENT(&cs_scenario_get_script_data(global_scenario_get())->point_sets, point_set_index, cs_point_set);
}

#endif // AI_CS_SCENARIO_DEFINITIONS_H
