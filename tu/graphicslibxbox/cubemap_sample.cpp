// graphicslibxbox/cubemap_sample.obj: from the records 0x15c670 0x175f40
// @flags /TP /O2 /Oc
#include "objects/object_reference_types.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
#include "scenario/scenario.h"
#include "scenario/cluster_reference_helpers.h"
#include "tag_files/tag_groups.h"
// @address 0x175f40 ?get_cluster_cubemap_index@c_dynamic_cubemap_sample@@SAJUs_cluster_reference@@@Z
// @address 0x15c670 ?search_for_cubemap_sample_in_cluster@c_dynamic_cubemap_sample@@QAA_NUs_cluster_reference@@PBTreal_point3d@@M@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "scenario/scenario_structure_bsp_reference.h"
#include "scenario/scenario_tag.h"



#include "structures/structure_bsp_tag.h"



// @address 0x7cff08 ?global_scenario@@3PAUscenario@@A
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B

#include "cubemaps/cubemap_sample.h"

static __forceinline void *cubemap_sample_block_get(const s_tag_block *block, long index, long size)
{
    return (char *)block->address + index * size;
}

static __forceinline structure_bsp *cubemap_sample_bsp_get(long index)
{
    const scenario *current_scenario = (const scenario *)global_scenario;
    const scenario_structure_bsp_reference *scenario_bsp = (const scenario_structure_bsp_reference *)cubemap_sample_block_get(
        &current_scenario->structure_bsps, index, sizeof(scenario_structure_bsp_reference));
    return TAG_GET(0, structure_bsp, scenario_bsp->structure_bsp.index);
}

static __forceinline structure_bsp *cubemap_sample_bsp_try_get(long index)
{
    const scenario *current_scenario = (const scenario *)global_scenario;
    const scenario_structure_bsp_reference *scenario_bsp = (const scenario_structure_bsp_reference *)cubemap_sample_block_get(
        &current_scenario->structure_bsps, index, sizeof(scenario_structure_bsp_reference));
    if (scenario_bsp && scenario_bsp->structure_bsp.index != -1)
        return TAG_GET(0, structure_bsp, scenario_bsp->structure_bsp.index);
    return 0;
}

// 0x15c670
bool c_dynamic_cubemap_sample::search_for_cubemap_sample_in_cluster(s_cluster_reference cluster_reference, const real_point3d *point, float tolerance)
{
    bool result;

    if (cluster_reference_valid(&cluster_reference))
    {
        result = false;
        structure_bsp *bsp = cubemap_sample_bsp_get(cluster_reference.bsp_index);
        structure_cluster *cluster = (structure_cluster *)cubemap_sample_block_get(
            &bsp->clusters, cluster_reference.cluster_index, sizeof(structure_cluster));
        s_structure_cluster_cubemap_info *previous_point = 0;
        if (cluster_reference_valid(&m_current.cluster_reference) && m_current.cubemap_index != -1)
        {
            structure_bsp *previous_bsp = cubemap_sample_bsp_try_get(m_current.cluster_reference.bsp_index);
            if (previous_bsp && m_current.cluster_reference.cluster_index < previous_bsp->clusters.count)
            {
                structure_cluster *previous_cluster = (structure_cluster *)cubemap_sample_block_get(
                    &previous_bsp->clusters, m_current.cluster_reference.cluster_index, sizeof(structure_cluster));
                if (m_current.cubemap_index < previous_cluster->cluster_cubemaps.count)
                    previous_point = (s_structure_cluster_cubemap_info *)cubemap_sample_block_get(
                        &previous_cluster->cluster_cubemaps, m_current.cubemap_index, sizeof(s_structure_cluster_cubemap_info));
            }
        }

        float best_distance_squared = 3.4028234663852886e+38f;
        s_structure_cluster_cubemap_info *best_point = 0;
        long best_index = -1;
        for (long index = 0; index < cluster->cluster_cubemaps.count; index++)
        {
            s_structure_cluster_cubemap_info *candidate = (s_structure_cluster_cubemap_info *)cubemap_sample_block_get(
                &cluster->cluster_cubemaps, index, sizeof(s_structure_cluster_cubemap_info));
            float distance_squared = distance_squared3d(&candidate->position, point);
            if (distance_squared < best_distance_squared)
            {
                best_distance_squared = distance_squared;
                best_point = candidate;
                best_index = index;
            }
        }

        bool replace;
        if (!best_point)
        {
            replace = false;
        }
        else if (previous_point && m_current.cluster_reference.bsp_index == cluster_reference.bsp_index &&
            m_current.cluster_reference.cluster_index == cluster_reference.cluster_index)
        {
            float previous_distance_squared = distance_squared3d(&previous_point->position, point);
            // the only caller (0x15c608) passes a tolerance of 0.1f
            replace = best_distance_squared < (1.0f - 0.1f) * (1.0f - 0.1f) * previous_distance_squared;
        }
        else
        {
            replace = true;
        }

        if (replace && best_point)
        {
            m_last = m_current;
            m_blend_factor = 0.0f;
            m_current.cluster_reference = cluster_reference;
            m_current.cubemap_index = best_index;
            m_current.next_cubemap_index = best_point->cubemap_bitmap_index;
            if ((unsigned long)m_last.cubemap_index == 0xffff || !previous_point)
            {
                m_last = m_current;
                m_blend_factor = 1.0f;
            }
            result = true;
        }
    }
    return result;
}

long c_dynamic_cubemap_sample::get_cluster_cubemap_index(s_cluster_reference cluster_reference)
{
    long result = -1;
    if (cluster_reference_valid(&cluster_reference))
    {
        structure_bsp *bsp = cubemap_sample_bsp_get(cluster_reference.bsp_index);
        structure_cluster *cluster = (structure_cluster *)cubemap_sample_block_get(
            &bsp->clusters, cluster_reference.cluster_index, sizeof(structure_cluster));
        if (cluster->cluster_cubemaps.count > 0)
        {
            s_structure_cluster_cubemap_info *sample = (s_structure_cluster_cubemap_info *)cubemap_sample_block_get(
                &cluster->cluster_cubemaps, 0, sizeof(s_structure_cluster_cubemap_info));
            result = sample->cubemap_bitmap_index;
        }
    }
    return result;
}
