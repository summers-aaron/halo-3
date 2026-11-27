#ifndef SCENARIO_CLUSTER_REFERENCE_HELPERS_H
#define SCENARIO_CLUSTER_REFERENCE_HELPERS_H

#include "objects/cluster_reference.h"
#include "scenario/scenario.h"
#include "scenario/scenario_access.h"
#include "cseries/cseries.h"

static __forceinline bool global_structure_bsp_is_active(long structure_bsp_index)
{
    return structure_bsp_index >= 0 && structure_bsp_index < 16 && (global_structure_bsp_active_mask_get() & FLAG(structure_bsp_index)) != 0;
}

static __forceinline const s_cluster_reference *cluster_reference_set(s_cluster_reference *reference, long bsp_index, long cluster_index)
{
    reference->bsp_index = (signed char)bsp_index;
    reference->cluster_index = (unsigned char)cluster_index;
    return reference;
}

inline bool cluster_reference_valid(const s_cluster_reference *cluster_reference)
{
    return cluster_reference->bsp_index != -1;
}

inline bool scenario_cluster_reference_valid(const s_cluster_reference *cluster_reference)
{
    bool valid = cluster_reference_valid(cluster_reference);
    return valid;
}

#endif
