#ifndef ITEMS_ITEM_DEFINITIONS_H
#define ITEMS_ITEM_DEFINITIONS_H
#include "objects/object_definitions.h"
// Item fields for pickup and damage aftermath handling.
// Message IDs at +0x108..+0x124 are read by chud_post_action_response.
struct s_item_definition {
    s_object_definition object;
    unsigned long flags;
    char spare_after_flags[0x108 - 0xfc];
    long pickup_message;
    long swap_message;
    long pickup_or_dual_wield_message;
    long swap_or_dual_wield_message;
    long picked_up_message;
    long switch_to_message;
    long switch_to_from_ai_message;
    long all_weapons_empty_message;
    char spare_before_detonation_damage_effect[0x144 - 0x128];
    s_tag_reference detonation_damage_effect;
    float detonation_time_lower;
    float detonation_time_upper;
    s_tag_reference detonation_effect;
    s_tag_reference detonating_effect;
    float scale[10];
    float angular_damping;
    float linear_damping;
};
typedef char item_definition_damage_offset[(offsetof(s_item_definition, detonation_damage_effect) == 0x144) ? 1 : -1];
typedef char item_definition_delay_offset[(offsetof(s_item_definition, detonation_time_lower) == 0x154) ? 1 : -1];
typedef char item_definition_effect_offset[(offsetof(s_item_definition, detonation_effect) == 0x15c) ? 1 : -1];
typedef char item_definition_scale_offset[(offsetof(s_item_definition, scale) == 0x17c) ? 1 : -1];
typedef char item_definition_damping_offset[(offsetof(s_item_definition, linear_damping) == 0x1a8) ? 1 : -1];
typedef char item_pickup_message_offset[(offsetof(s_item_definition, pickup_message) == 0x108) ? 1 : -1];
typedef char item_empty_message_offset[(offsetof(s_item_definition, all_weapons_empty_message) == 0x124) ? 1 : -1];
#endif
