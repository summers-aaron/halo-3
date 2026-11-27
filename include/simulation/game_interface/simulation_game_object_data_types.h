#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_OBJECT_DATA_TYPES_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_OBJECT_DATA_TYPES_H

#include "math/real_math_types.h"
#include <stddef.h>

struct s_simulation_object_creation_data
{
    long scenario_datum_index;
    short map_variant_index;
    bool allow_duplicate_scenario_objects;
    bool object_is_respawned;
    long definition_index;
    long child_variant_index;
};

struct s_simulation_object_state_data
{
    real_point3d position;
    real_vector3d forward;
    real_vector3d up;
    float scale;
    real_vector3d translational_velocity;
    real_vector3d angular_velocity;
    float body_vitality;
    bool body_stun_ticks_zero;
    float shield_vitality;
    bool shield_stun_ticks_zero;
    bool shield_stunned;
    bool shield_depleted;
    bool dead;
    short destroyed_regions;
    unsigned char region_count;
    unsigned char region_states[16];
    unsigned char constraint_count;
    unsigned short destroyed_constraints;
    unsigned short loosened_constraints;
    long parent_entity_index;
    long parent_gamestate_index;
    long parent_node_index;
    bool parent_attached;
    real_point3d parent_position;
    real_vector3d parent_up;
    real_vector3d parent_forward;
    unsigned short multiplayer_flags;
    short map_variant_index;
};

typedef char simulation_object_creation_data_size[(sizeof(s_simulation_object_creation_data) == 0x10) ? 1 : -1];
typedef char simulation_object_state_data_size[(sizeof(s_simulation_object_state_data) == 0xa0) ? 1 : -1];
typedef char simulation_object_state_parent_offset[(offsetof(s_simulation_object_state_data, parent_entity_index) == 0x68) ? 1 : -1];

#endif
