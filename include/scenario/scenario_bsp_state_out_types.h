#ifndef SCENARIO_SCENARIO_BSP_STATE_OUT_TYPES_H
#define SCENARIO_SCENARIO_BSP_STATE_OUT_TYPES_H

#include "memory/data_types.h"
#include <stddef.h>

struct scenario_bsp_state_out {
    bool valid;
    unsigned long active_structure_bsp_mask;
    unsigned long touched_structure_bsp_mask;
    unsigned long active_designer_zone_mask;
    unsigned long active_cinematic_zone_mask;
    unsigned long required_structure_bsp_mask;
    unsigned long required_designer_zone_mask;
    unsigned long required_cinematic_zone_mask;
};

typedef char scenario_bsp_state_out_size[(sizeof(scenario_bsp_state_out) == 0x20) ? 1 : -1];
typedef char scenario_bsp_state_out_active_structure_bsp_mask_offset[
    (offsetof(scenario_bsp_state_out, active_structure_bsp_mask) == 0x4) ? 1 : -1];
typedef char scenario_bsp_state_out_touched_structure_bsp_mask_offset[
    (offsetof(scenario_bsp_state_out, touched_structure_bsp_mask) == 0x8) ? 1 : -1];
typedef char scenario_bsp_state_out_active_designer_zone_mask_offset[
    (offsetof(scenario_bsp_state_out, active_designer_zone_mask) == 0xc) ? 1 : -1];
typedef char scenario_bsp_state_out_active_cinematic_zone_mask_offset[
    (offsetof(scenario_bsp_state_out, active_cinematic_zone_mask) == 0x10) ? 1 : -1];
typedef char scenario_bsp_state_out_required_structure_bsp_mask_offset[
    (offsetof(scenario_bsp_state_out, required_structure_bsp_mask) == 0x14) ? 1 : -1];
typedef char scenario_bsp_state_out_required_designer_zone_mask_offset[
    (offsetof(scenario_bsp_state_out, required_designer_zone_mask) == 0x18) ? 1 : -1];
typedef char scenario_bsp_state_out_required_cinematic_zone_mask_offset[
    (offsetof(scenario_bsp_state_out, required_cinematic_zone_mask) == 0x1c) ? 1 : -1];

#endif // SCENARIO_SCENARIO_BSP_STATE_OUT_TYPES_H
