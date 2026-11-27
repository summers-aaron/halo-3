#ifndef SCENARIO_SCENARIO_GAME_STATE_H
#define SCENARIO_SCENARIO_GAME_STATE_H

#include <stddef.h>

// Serialized by game_state.cpp; populated and compared by scenario.cpp.
struct s_scenario_game_state
{
    unsigned long flags;
    long zone_set_index;
    unsigned long active_designer_zone_mask;
    unsigned long active_structure_bsp_mask;
    unsigned long touched_structure_bsp_mask;
    unsigned long active_cinematic_zone_mask;
    unsigned long touched_cinematic_zone_mask;
};
typedef char scenario_game_state_size[(sizeof(s_scenario_game_state) == 0x1c) ? 1 : -1];
typedef char scenario_game_state_active_structure_offset[(offsetof(s_scenario_game_state, active_structure_bsp_mask) == 0xc) ? 1 : -1];
typedef char scenario_game_state_touched_cinematic_offset[(offsetof(s_scenario_game_state, touched_cinematic_zone_mask) == 0x18) ? 1 : -1];

#endif
