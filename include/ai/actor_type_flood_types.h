#ifndef INCLUDE_AI_ACTOR_TYPE_FLOOD_TYPES_H
#define INCLUDE_AI_ACTOR_TYPE_FLOOD_TYPES_H

#include <stddef.h>
#include "ai/squad_types.h"
#include "tag_files/tag_types.h"

extern "C" void actor_set_berserk(long actor_index, bool berserk, bool latch);
extern "C" bool clump_can_berserk(long index, long actor_index, long latch_index);
struct character_general_properties {
    unsigned long flags;
    unsigned short type;
    short rank;
    short follower_positioning;
    float leader_distance;
    float leader_distance_override;
    float scariness;
    short grenade_type;
    short behavior_tree_root;
};

struct style_definition
{
    char name[32];
    short combat_status;
    short attitude;
    unsigned long style_control;
    s_style_flags style;
    c_typed_tag_block<long> special_movement;
    s_tag_block behavior_list;
};

typedef char style_definition_movement_offset[(offsetof(style_definition, special_movement) == 0x44) ? 1 : -1];

typedef char style_definition_style_offset[(offsetof(style_definition, style) == 0x28) ? 1 : -1];

typedef char character_general_leader_distance_offset[(offsetof(character_general_properties, leader_distance) == 0xc) ? 1 : -1];

typedef char character_general_scariness_offset[(offsetof(character_general_properties, scariness) == 0x14) ? 1 : -1];

typedef char character_general_properties_size[(sizeof(character_general_properties) == 0x1c) ? 1 : -1];
typedef char character_general_follower_positioning_offset[(offsetof(character_general_properties, follower_positioning) == 8) ? 1 : -1];
typedef char character_general_behavior_root_offset[(offsetof(character_general_properties, behavior_tree_root) == 0x1a) ? 1 : -1];
typedef char style_definition_size[(sizeof(style_definition) == 0x5c) ? 1 : -1];
typedef char style_definition_control_offset[(offsetof(style_definition, style_control) == 0x24) ? 1 : -1];
typedef char style_definition_behavior_list_offset[(offsetof(style_definition, behavior_list) == 0x50) ? 1 : -1];

#endif
