#ifndef SCENARIO_SCENARIO_ACCESS_H
#define SCENARIO_SCENARIO_ACCESS_H

#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "tag_files/tag_groups.h"
#include "math/integer_math.h"

struct structure_bsp;

static __forceinline bool structure_bsp_is_active(long structure_bsp_index)
{
    return (unsigned long)structure_bsp_index <= 15 &&
        ((1 << structure_bsp_index) & g_active_structure_bsp_mask) != 0;
}

// Accessors for the loaded scenario and active structure BSP state.

inline scenario *global_scenario_get()
{
    return global_scenario;
}

static __forceinline long scenario_zone_set_index_get()
{
    return global_zone_set_index;
}

static __forceinline unsigned long global_structure_bsp_active_mask_get()
{
    return g_active_structure_bsp_mask;
}

static __forceinline long global_structure_bsp_first_active_index_get()
{
    return lowest_bit_set(global_structure_bsp_active_mask_get());
}

// Returns the next active structure BSP index after the given index.
static __forceinline long global_structure_bsp_next_active_index_get(long structure_bsp_index)
{
    return lowest_bit_set(global_structure_bsp_active_mask_get() & ~(unsigned long)((1i64 << (structure_bsp_index + 1)) - 1));
}

static __forceinline structure_bsp *global_structure_bsp_get(long structure_bsp_index)
{
    scenario_structure_bsp_reference *reference = (scenario_structure_bsp_reference *)tag_block_get_element_with_size(
        &global_scenario_get()->structure_bsps, structure_bsp_index, sizeof(scenario_structure_bsp_reference));
    return (structure_bsp *)tag_get('sbsp', reference->structure_bsp.index);
}

#endif // SCENARIO_SCENARIO_ACCESS_H
