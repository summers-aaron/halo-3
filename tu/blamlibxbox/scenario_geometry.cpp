// OBJ: from the records 0x15f880 0x2af3d0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "tag_files/tag_types.h"
#include "scenario/scenario_lightmap_definitions.h"
#include "scenario/scenario_geometry.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "tag_files/tag_groups.h"
#include "structures/structure_bsp_tag.h"
#include "scenario/scenario_lightmap_types.h"


namespace
{
static __forceinline structure_bsp *scenario_structure_bsp_get(scenario *definition, long index)
{
    long tag_index = ((const scenario_structure_bsp_reference *)definition->structure_bsps.address)[index].structure_bsp.index;
    return (structure_bsp *)tag_get('sbsp', tag_index);
}
static __forceinline const s_render_geometry *scenario_structure_bsp_get_default_render_geometry(long scenario_index, long structure_bsp_index)
{
    scenario *definition = (scenario *)tag_get('scnr', scenario_index);
    const s_render_geometry *geometry = &scenario_structure_bsp_get(definition, structure_bsp_index)->render_geometry;
    return geometry;
}

}

bool scenario_structure_bsp_has_valid_lightmap_geometry(long scenario_index, long structure_bsp_index)
{
    bool valid = false;
    scenario *definition = (scenario *)tag_get('scnr', scenario_index);
    if (definition->lightmap.index != -1)
    {
        s_scenario_lightmap_bsp_data *lightmap = scenario_lightmap_find_bsp(definition->lightmap.index, structure_bsp_index);
        if (lightmap)
        {
            structure_bsp *bsp = scenario_structure_bsp_get((scenario *)definition, structure_bsp_index);
            if (lightmap->structure_checksum == bsp->import_info_checksum &&
                lightmap->render_geometry.meshes.count == bsp->render_geometry.meshes.count &&
                lightmap->coefficients_bitmap.index != -1 &&
                lightmap->dominant_light_bitmap.index != -1)
            {
                valid = true;
            }
        }
    }
    return valid;
}

const s_render_geometry *scenario_structure_bsp_get_render_geometry(long scenario_index, long structure_bsp_index)
{
    const s_render_geometry *geometry = scenario_structure_bsp_get_default_render_geometry(scenario_index, structure_bsp_index);
    if (scenario_structure_bsp_has_valid_lightmap_geometry(scenario_index, structure_bsp_index)) {
        scenario *definition = (scenario *)tag_get('scnr', scenario_index);
        s_scenario_lightmap_bsp_data *lightmap = scenario_lightmap_find_bsp(definition->lightmap.index, structure_bsp_index);
        geometry = &lightmap->render_geometry;
    }
    return geometry;
}
