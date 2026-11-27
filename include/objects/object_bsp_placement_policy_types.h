#ifndef OBJECTS_OBJECT_BSP_PLACEMENT_POLICY_TYPES_H
#define OBJECTS_OBJECT_BSP_PLACEMENT_POLICY_TYPES_H

enum e_object_bsp_placement_policy
{
    _object_bsp_placement_policy_invalid = -1,
    _object_bsp_placement_policy_default = 0,
    _object_bsp_placement_policy_always_placed,
    _object_bsp_placement_policy_manual_bsp_placement,

    k_object_bsp_placement_policy_count
};

#endif // OBJECTS_OBJECT_BSP_PLACEMENT_POLICY_TYPES_H
