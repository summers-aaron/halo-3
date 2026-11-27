// ailibxbox/ai_sector_structures.obj: from the records 0xb36b8 0x56a038
// @flags /TP /O2
// @address 0xb36b8 ?valid@c_sector_ref@@QBA_NXZ

#include "structures/structure_bsp_tag.h"
#include "ai/pathfinding_data.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "ai/sector_ref.h"
#include "scenario/scenario.h"
#include "tag_files/tag_groups.h"
#include "ai/hint_ref.h"

static __forceinline bool structure_bsp_active(unsigned int index)
{
    return index <= 15 && (g_active_structure_bsp_mask & (1 << index)) != 0;
}

bool c_sector_ref::valid() const
{
    bool result = false;
    short bsp_index = structure_bsp_index;
    if (bsp_index != -1 && bsp_index >= 0 && bsp_index < global_scenario->structure_bsps.count && structure_bsp_active(bsp_index)) {
        scenario_structure_bsp_reference *entry = &((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[bsp_index];
        structure_bsp *definition = (structure_bsp *)tag_get(entry->structure_bsp.group_tag, entry->structure_bsp.index);
        if (definition->pathfinding_data.count > 0 && definition->pathfinding_data.address) {
            result = (unsigned short)sector_index < ((struct pathfinding_data *)definition->pathfinding_data.address)->sectors.count;
        }
    }
    return result;
}

static __forceinline scenario *hint_global_scenario_get()
{
    return global_scenario;
}

static __forceinline structure_bsp *hint_structure_bsp_get(scenario *scenario_view, long bsp_index)
{
    scenario_structure_bsp_reference *entry = &((scenario_structure_bsp_reference *)scenario_view->structure_bsps.address)[bsp_index];
    return (structure_bsp *)tag_get(entry->structure_bsp.group_tag, entry->structure_bsp.index);
}

static __forceinline pathfinding_data *hint_pathfinding_data_get(structure_bsp *bsp)
{
    return (pathfinding_data *)bsp->pathfinding_data.address;
}

bool c_hint_ref::valid() const
{
    scenario *scenario_view;
    struct pathfinding_data *data = 0;
    bool result = false;
    if (structure_bsp_index >= 0) {
        scenario_view = hint_global_scenario_get();
        if (structure_bsp_index < scenario_view->structure_bsps.count && structure_bsp_active(structure_bsp_index)) {
        structure_bsp *bsp = hint_structure_bsp_get(scenario_view, structure_bsp_index);
        if (bsp->pathfinding_data.count > 0) {
            data = hint_pathfinding_data_get(bsp);
        }
    }
    }
    if (data) {
        result = hint_index >= 0 && hint_index < data->hints.count;
    }
    return result;
}
