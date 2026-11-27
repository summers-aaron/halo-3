#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_DAMAGE_DATA_TYPES_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_DAMAGE_DATA_TYPES_H

#include "math/real_math_types.h"

struct s_simulation_breakable_surface_damage_event_data {
    long instanced_geometry_instance_index;
    long seed_surface_index;
    unsigned char location[4];
    real_point3d position;
};

struct s_simulation_damage_section_response_event_data {
    long damage_section_index;
    long response_index;
    long response_type;
};
struct s_simulation_breakable_surface_group_creation_data { unsigned short group_index; };
struct s_simulation_breakable_surface_group_state_data { unsigned char damaged_surfaces[4]; };

typedef char simulation_breakable_surface_damage_size[(sizeof(s_simulation_breakable_surface_damage_event_data) == 0x18) ? 1 : -1];
typedef char simulation_damage_section_response_size[(sizeof(s_simulation_damage_section_response_event_data) == 0xc) ? 1 : -1];

#endif
