// blamlibxbox/scenario_definitions.obj: from the records 0x1274c8
// @compiler xdk6130
// @flags /TP /O2
// @address 0x1274c8 ?scenario_zone_set_structure_bsp_cluster_visible_sky_index_get@@YAJPBUscenario@@JJJ@Z
#include "tag_files/tag_groups.h"
#include "scenario/scenario_definitions.h"

#include "scenario/scenario_tag.h"
#include "math/integer_math.h"


long scenario_zone_set_structure_bsp_cluster_visible_sky_index_get(
    const scenario *scenario_definition, long zone_set_index, long structure_bsp_index,
    long cluster_index)
{
    const s_scenario_zone_set *zone_set =
        &((const s_scenario_zone_set *)scenario_definition->zone_sets.address)[zone_set_index];
    const s_scenario_zone_set_visibility *pvs =
        &((const s_scenario_zone_set_visibility *)scenario_definition->zone_set_pvs.address)[zone_set->pvs_index];
    long pvs_bsp_index = count_bits(
        zone_set->bsp_active_mask & (unsigned long)((1i64 << structure_bsp_index) - 1));
    const s_scenario_bsp_pvs *bsp_pvs =
        &((const s_scenario_bsp_pvs *)pvs->structure_bsp_pvs.address)[pvs_bsp_index];
    signed char sky_index = ((const signed char *)bsp_pvs->cluster_visible_sky_indices.address)[cluster_index];
    return sky_index;
}
