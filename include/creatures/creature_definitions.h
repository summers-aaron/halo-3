#ifndef CREATURES_CREATURE_DEFINITIONS_H
#define CREATURES_CREATURE_DEFINITIONS_H

#include "tag_files/tag_types.h"
#include "objects/object_definitions.h"
#include "physics/character_physics_definition_types.h"

// The creature tag ('crea'). Only the fields after the shared object definition that cleaned units read are named.
// The character physics definition at 0x110 is what motor_system_creature passes to character_physics_*_initialize.
struct creature_definition : s_object_definition
{
    unsigned long flags;                            // damage.cpp tests bit 1 in area_effect_infection_form_damage
    short default_team;                             // creatures.cpp creatures_creature_definition
    short motion_sensor_blip_size;
    float turning_velocity_maximum;                 // creature_ground_motor_program.cpp creature_update_turning
    float turning_acceleration_maximum;
    float casual_turning_modifier;                  // scales both while the control asks for casual turning
    float autoaim_width;
    s_character_physics_definition physics;
    s_tag_reference attached_damage_effect;
    s_tag_reference attached_shield_damage_effect;
    s_tag_block campaign_metagame;
    float destroy_after_death_time_lower;
    float destroy_after_death_time_upper;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char creature_definition_metagame_offset[(offsetof(creature_definition, campaign_metagame) == 0x1e4) ? 1 : -1];
typedef char creature_definition_blip_size_offset[(offsetof(creature_definition, motion_sensor_blip_size) == 0xfe) ? 1 : -1];
typedef char creature_definition_offset_flags[(offsetof(creature_definition, flags) == 0xf8) ? 1 : -1];
typedef char creature_definition_offset_default_team[(offsetof(creature_definition, default_team) == 0xfc) ? 1 : -1];
typedef char creature_definition_offset_turning_velocity_maximum[(offsetof(creature_definition, turning_velocity_maximum) == 0x100) ? 1 : -1];
typedef char creature_definition_offset_physics[(offsetof(creature_definition, physics) == 0x110) ? 1 : -1];
typedef char creature_definition_offset_attached_damage_effect[(offsetof(creature_definition, attached_damage_effect) == 0x1c4) ? 1 : -1];
typedef char creature_definition_offset_attached_shield_damage_effect[(offsetof(creature_definition, attached_shield_damage_effect) == 0x1d4) ? 1 : -1];
typedef char creature_definition_offset_destroy_after_death_time[(offsetof(creature_definition, destroy_after_death_time_lower) == 0x1f0) ? 1 : -1];
#endif

#endif // CREATURES_CREATURE_DEFINITIONS_H
