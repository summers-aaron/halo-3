// blamlibxbox/scenario_lightmap_definitions.obj: from the records 0x141348
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x141348 ?scenario_lightmap_find_bsp@@YAPAUs_scenario_lightmap_bsp_data@@JJ@Z

/* Written from the binary's machine code */
#include "scenario/scenario_lightmap_definitions.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "scenario/scenario_lightmap_types.h"
static __forceinline long scenario_lightmap_find_bsp_data_index(long handle, long identifier)
{
    long result = -1;
    if (handle != -1) {
        const s_scenario_lightmap_definition *definition = TAG_GET('sLdT', s_scenario_lightmap_definition, handle);
        for (long i = 0; i < definition->lightmaps.count; i++) {
            const s_scenario_lightmap_bsp_data *bsp = TAG_BLOCK_GET_ELEMENT(&definition->lightmaps, i, s_scenario_lightmap_bsp_data);
            if (bsp->identifier == identifier) { result = i; break; }
        }
    }
    return result;
}

s_scenario_lightmap_bsp_data *scenario_lightmap_find_bsp(long handle, long identifier)
{
    s_scenario_lightmap_bsp_data *result = 0;
    long index = scenario_lightmap_find_bsp_data_index(handle, identifier);
    if (index != -1) {
        s_scenario_lightmap_definition *definition = TAG_GET('sLdT', s_scenario_lightmap_definition, handle);
        result = TAG_BLOCK_GET_ELEMENT(&definition->lightmaps, index, s_scenario_lightmap_bsp_data);
    }
    return result;
}
