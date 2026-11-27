// blamlibxbox/scenario_pvs.obj: from the records 0xb0858 0xd59d8
// @flags /TP /O2 /Oc
#include "objects/object_reference_types.h"
#include "tag_files/tag_groups.h"
// @address 0xb0858 ?scenario_zone_set_pvs_get_row@@YAXJPAUs_scenario_pvs_row@@JUs_cluster_reference@@_N@Z
// @address 0xd59d8 ?scenario_zone_set_pvs_write_row@@YAXPAUs_game_cluster_bit_vectors@@PBUs_scenario_pvs_row@@@Z
#include <string.h>
#include "scenario/scenario_pvs.h"
#include "scenario/scenario_definitions.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"


#include "scenario/scenario_pvs_types.h"
#include "structures/structure_bsp_tag.h"
#include "structures/cluster_bit_vectors.h"
#include "math/integer_math.h"

void scenario_zone_set_pvs_get_row(long scenario_index,s_scenario_pvs_row *output,long zone_index,s_cluster_reference reference,bool doors) {
    scenario *scenario_definition=(scenario *)tag_get('scnr',scenario_index);
    s_scenario_zone_set *zone=&((s_scenario_zone_set *)scenario_definition->zone_sets.address)[zone_index];
    s_scenario_zone_set_visibility *visibility=&((s_scenario_zone_set_visibility *)scenario_definition->zone_set_pvs.address)[zone->pvs_index];
    long bsp_index=count_bits(zone->bsp_active_mask&(unsigned long)((1i64<<(reference.bsp_index+1))-1))-1;
    s_scenario_bsp_pvs *pvs=&((s_scenario_bsp_pvs *)visibility->structure_bsp_pvs.address)[bsp_index];
    s_tag_block *clusters=doors?&pvs->cluster_pvs_doors_closed:&pvs->cluster_pvs;
    s_scenario_cluster_pvs *cluster_index=TAG_BLOCK_GET_ELEMENT(clusters,reference.cluster_index,s_scenario_cluster_pvs);
    long pvs_bsp_index=0;
    for(long structure_bsp_index=0;structure_bsp_index<16;structure_bsp_index++) {
        if(zone->bsp_active_mask&(1<<structure_bsp_index)) {
            s_scenario_pvs_bit_vector *bits=TAG_BLOCK_GET_ELEMENT(&cluster_index->pvs,pvs_bsp_index,s_scenario_pvs_bit_vector);
            const scenario_structure_bsp_reference *bsp_reference = TAG_BLOCK_GET_ELEMENT(
                &scenario_definition->structure_bsps, structure_bsp_index, scenario_structure_bsp_reference);
            const structure_bsp *bsp = TAG_GET('sbsp', structure_bsp, bsp_reference->structure_bsp.index);
            long count=bsp->clusters.count;
            output->vectors[structure_bsp_index]=(unsigned long *)bits->bit_vector.address;
            output->counts[structure_bsp_index]=count;
            pvs_bsp_index++;
        } else {
            output->vectors[structure_bsp_index]=0;
            output->counts[structure_bsp_index]=0;
        }
    }
}

void scenario_zone_set_pvs_write_row(s_game_cluster_bit_vectors *clusters, const s_scenario_pvs_row *row)
{
    memset(clusters, 0, sizeof(*clusters));
    for (long structure_bsp_index = 0; structure_bsp_index < 16; structure_bsp_index++)
    {
        if (row->counts[structure_bsp_index] != 0)
        {
            memcpy(clusters->bit_vectors[structure_bsp_index].get_writeable_bits_direct(),
                row->vectors[structure_bsp_index],
                ((row->counts[structure_bsp_index] + 31) >> 5) * sizeof(unsigned long));
        }
    }
}
