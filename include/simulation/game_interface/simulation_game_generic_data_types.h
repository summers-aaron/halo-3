#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_GENERIC_DATA_TYPES_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_GENERIC_DATA_TYPES_H

#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "math/matrix_math_types.h"

struct s_simulation_generic_creation_data : s_simulation_object_creation_data
{
    long variant_name;
};

struct s_simulation_generic_state_data
{
    s_simulation_object_state_data object;
    unsigned long mask;
    real_matrix4x3 transforms[8];
};

typedef char simulation_generic_creation_size[(sizeof(s_simulation_generic_creation_data) == 0x14) ? 1 : -1];
typedef char simulation_generic_state_size[(sizeof(s_simulation_generic_state_data) == 0x244) ? 1 : -1];
typedef char simulation_generic_state_parent_offset[(offsetof(s_simulation_generic_state_data, object.parent_entity_index) == 0x68) ? 1 : -1];
typedef char simulation_generic_state_transforms_offset[(offsetof(s_simulation_generic_state_data, transforms) == 0xa4) ? 1 : -1];

extern "C" {
extern const char k_simulation_generic_rigid_body_transforms_name[];
extern const char k_simulation_generic_rigid_body_axes_name[];
}

#endif
