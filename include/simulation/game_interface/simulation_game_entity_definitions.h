#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ENTITY_DEFINITIONS_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ENTITY_DEFINITIONS_H

#include <stddef.h>
#include "tag_files/tag_types.h"

struct s_scenery_model_damage_info_definition
{
    unsigned long flags;
    long global_indirect_material_name;
    short indirect_damage_section;
    short collision_damage_reporting_info;
    long vitality_parameters[46];
    s_tag_block damage_sections;
    s_tag_block nodes;
    short global_shield_material_type;
    short global_indirect_material_type;
    float shield_recharge_velocity;
    float health_recharge_velocity;
    s_tag_block damage_seats;
    s_tag_block damage_constraints;
};

typedef char scenery_damage_sections_offset[(offsetof(s_scenery_model_damage_info_definition, damage_sections) == 196) ? 1 : -1];
typedef char scenery_damage_constraints_offset[(offsetof(s_scenery_model_damage_info_definition, damage_constraints) == 244) ? 1 : -1];

struct s_simulation_game_entity_entry
{
    long state_header[18];
    long entity_index1;
    long gamestate_index1;
    long entity_index2;
    long gamestate_index2;
};

typedef char simulation_game_entity_entry_index1_offset[(offsetof(s_simulation_game_entity_entry, entity_index1) == 72) ? 1 : -1];
typedef char simulation_game_entity_entry_size[(sizeof(s_simulation_game_entity_entry) == 88) ? 1 : -1];

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_ENTITY_DEFINITIONS_H
