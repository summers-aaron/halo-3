// blamlibxbox/scenario_sky.obj: from the records 0x226588 0x2266a8
// @flags /TP /O2 /Oc
#include "objects/object_types.h"
#include "objects/objects.h"
#include "objects/object_header.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "objects/object_name_list.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_access.h"
#include "scenario/scenario_definitions.h"
#include "scenario/scenario_sky.h"
#include "structures/cluster_bit_vectors.h"
#include "structures/structure_bsp_tag.h"
#include "tag_files/tag_groups.h"
#include <string.h>

// Returns the attached sky for a cluster in the zone set.
static __forceinline long scenario_sky_cluster_attached_sky_index_get(const scenario *scenario_definition, long zone_set_index,
    long structure_bsp_index, long cluster_index)
{
    const s_scenario_zone_set *zone_set = TAG_BLOCK_GET_ELEMENT(&scenario_definition->zone_sets, zone_set_index, s_scenario_zone_set);
    const s_scenario_zone_set_visibility *visibility =
        TAG_BLOCK_GET_ELEMENT(&scenario_definition->zone_set_pvs, zone_set->pvs_index, s_scenario_zone_set_visibility);
    long visibility_bsp_index = count_bits(zone_set->bsp_active_mask & (unsigned long)((1i64 << structure_bsp_index) - 1));
    const s_scenario_bsp_pvs *bsp_pvs = TAG_BLOCK_GET_ELEMENT(&visibility->structure_bsp_pvs, visibility_bsp_index, s_scenario_bsp_pvs);
    return ((const signed char *)bsp_pvs->cluster_attached_sky_indices.address)[cluster_index];
}

// @address 0x2266a8 ?calculate_sky_object_cluster_visibility@c_scenerio_sky_objects_helper@@SAPBUs_game_cluster_bit_vectors@@JPAU2@@Z
const s_game_cluster_bit_vectors *c_scenerio_sky_objects_helper::calculate_sky_object_cluster_visibility(long object_index,
    s_game_cluster_bit_vectors *cluster_visibility)
{
    const scenario *scenario_definition = global_scenario_get();
    s_object_header *header = object_header_get(object_index);
    object_datum *object = (object_datum *)header->datum;
    memset(cluster_visibility, 0, sizeof(*cluster_visibility));
    for (long structure_bsp_index = global_structure_bsp_first_active_index_get(); structure_bsp_index != NONE;
        structure_bsp_index = global_structure_bsp_next_active_index_get(structure_bsp_index))
    {
        structure_bsp *structure_bsp_definition = global_structure_bsp_get(structure_bsp_index);
        long sky_index = object->object_identifier.get_sky_identifier();
        for (long cluster_index = 0; cluster_index < structure_bsp_definition->clusters.count; cluster_index++)
        {
            if (scenario_sky_cluster_attached_sky_index_get(scenario_definition,
                    scenario_zone_set_index_get(), structure_bsp_index, cluster_index) == sky_index)
                cluster_visibility->bit_vectors[structure_bsp_index].set(cluster_index, true);
        }
    }
    return cluster_visibility;
}

// @address 0x226588 ?create_sky_object@c_scenerio_sky_objects_helper@@SAJJ@Z
long c_scenerio_sky_objects_helper::create_sky_object(long sky_index)
{
    scenario_sky_reference *sky = TAG_BLOCK_GET_ELEMENT(&global_scenario_get()->skies, sky_index, scenario_sky_reference);
    long object_index = NONE;

    if (sky->object.group_tag == 'scen')
    {
        long definition_index = sky->object.index;
        object_placement_data data;
        object_placement_data_new(&data, definition_index, NONE, NULL);
        data.object_identifier.create_from_sky(_object_type_scenery, sky_index);

        if (data.object_identifier.find_object_index() == NONE)
        {
            data.flags |= 64;
            object_index = object_new(&data);
            if (object_index != NONE && sky->name_index != 0xffff)
            {
                short name_index = (short)sky->name_index;
                if (name_index >= 0 && name_index < global_scenario_get()->object_names.count)
                    tls_object_name_list()[name_index] = object_index;

                ((object_datum *)object_get_and_verify_type(object_index, NONE))->object_name_index = sky->name_index;
            }
        }
    }

    return object_index;
}

// @address 0x5bec98 ?scenario_sky_objects_initialize@@YAXXZ
void scenario_sky_objects_initialize(void)
{
}

// @address 0x5bec98 ?scenario_sky_objects_dispose@@YAXXZ
void scenario_sky_objects_dispose(void)
{
}

// @address 0x5bec98 ?scenario_sky_objects_initialize_for_new_map@@YAXXZ
void scenario_sky_objects_initialize_for_new_map(void)
{
}

// @address 0x5bec98 ?scenario_sky_objects_dispose_from_old_map@@YAXXZ
void scenario_sky_objects_dispose_from_old_map(void)
{
}
