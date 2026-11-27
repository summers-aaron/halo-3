// ailibxbox/ai_scenario_definitions.obj: from the records 0x5627d0
// @flags /TP /O2 /Oc
#include "objects/object_reference_types.h"
// @address 0x5627d0 ?ai_scenario_find_cluster_index@@YA?AUs_cluster_reference@@PBTreal_point3d@@PBTreal_vector3d@@@Z
// @address 0x9e788 ?scenario_cluster_reference_from_leaf_index@@YA?AUs_cluster_reference@@JJ@Z
// @address 0x9bb88 ?scenario_leaf_index_from_point@@YAJPBTreal_point3d@@PAJ@Z
#include "math/real_math.h"
#include "scenario/scenario.h"
#include "ai/ai_scenario_definitions.h"
#include "cubemaps/cubemap_sample.h"



static __forceinline s_cluster_reference ai_scenario_definitions_cluster_reference_from_point(const real_point3d *point)
{
    long structure_bsp_index;
    long leaf_index = scenario_leaf_index_from_point(point, &structure_bsp_index);
    s_cluster_reference reference;
    if (leaf_index != -1)
    {
        reference = scenario_cluster_reference_from_leaf_index(structure_bsp_index, leaf_index);
    }
    else
    {
        cluster_reference_invalidate(&reference);
    }
    return reference;
}

s_cluster_reference ai_scenario_find_cluster_index(const real_point3d *point, const real_vector3d *vector)
{
    real_point3d test_point;
    point_from_line3d(point, vector, 0.1f, &test_point);
    return ai_scenario_definitions_cluster_reference_from_point(&test_point);
}
