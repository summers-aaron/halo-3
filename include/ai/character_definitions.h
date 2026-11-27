#ifndef AI_CHARACTER_DEFINITIONS_H
#define AI_CHARACTER_DEFINITIONS_H

#include <stddef.h>

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

// Functions defined in tu/ailibxbox/character_definitions.cpp that other units call.

struct character_charge_properties;
// The character tag's combatform properties block element (8 bytes; character_definition_combatform_multi_properties_get).
// combatform_berserk_control_get_impulse goes berserk inside berserk_distance of its target, or by berserk_chance.
struct character_combatform_properties
{
    float berserk_distance;
    float berserk_chance;
};
typedef char character_combatform_properties_size[(sizeof(character_combatform_properties) == 8) ? 1 : -1];
struct character_cover_properties;
struct character_engage_properties;
struct character_evasion_properties;
struct character_firing_pattern_properties;
struct character_general_properties;
struct character_grenade_properties;
struct character_idle_properties;
struct character_morph_properties;
struct character_movement_properties;
struct character_presearch_properties;
struct character_ready_properties;
struct character_retreat_properties;
struct character_search_properties;
// The character tag's swarm properties block element (0x38 bytes; character_definition_swarm_multi_properties_get).
// The first field is read as a signed short (lhz/extsh) against swarm_datum::death_count in
// behavior_swarm_scatter_get_desire.
struct character_swarm_properties
{
    short scatter_killed_count;
    float scatter_distance;
    float scatter_time;
    float hound_distance_minimum;
    float hound_distance_maximum;
    float infection_time_minimum;
    float infection_time_maximum;
    float acceleration;
    float noise_period_minimum;
    float noise_period_maximum;
    float noise_threshold_idle;
    float noise_threshold_active;
    float stuck_time;
    float stuck_distance;
};
typedef char character_swarm_scatter_distance_offset[
    (offsetof(character_swarm_properties, scatter_distance) == 0x4) ? 1 : -1];
typedef char character_swarm_properties_size[(sizeof(character_swarm_properties) == 0x38) ? 1 : -1];

// Vehicle steering, throttle, charging and cover settings.
struct character_vehicle_properties
{
    union
    {
        s_tag_reference unit;
        struct
        {
            unsigned long unit_group_tag;
            TAG_POINTER(const char) unit_name;
            long unit_name_length;
            long key;
        };
    };
    s_tag_reference style;
    unsigned long flags;
    float pathfinding_radius;
    float steering_distance;
    float deceleration_distance;
    float turning_radius;
    float minimum_turn_distance;
    float turn_radius;
    float banshee_steering_maximum_angle;
    float maximum_steering_angle;
    float maximum_steering_delta;
    float steering_angle_scale;
    real_bounds oversteering_bounds;
    float side_slip_distance;
    float obstacle_avoidance_distance;
    float minimum_urgency;
    float destination_behind_angle;
    float velocity_lookahead;
    float throttle_maximum;
    float goal_attenuation;
    float turn_attenuation;
    float direction_attenuation;
    float throttle_delta;
    float throttle_blend;
    float theoretical_maximum_speed;
    float error_scale;
    float maximum_aiming_deviation;
    float charge_distance;
    float charge_tight_angle;
    float charge_delay;
    float approach_scale;
    float charge_minimum_distance;
    float charge_maximum_distance;
    float ram_time;
    float ram_paralysis_time;
    float cover_damage_threshold;
    float target_lost_time;
    float cover_time;
    float cover_distance;
    float turtle_speed;
    float turtle_minimum_time;
    float turtle_maximum_time;
    unsigned short obstacle_ignore_size;
    unsigned short : 16; // Padding in the vehicle properties tag block.
    short charge_limit;
    short charge_minimum;
};

typedef char character_vehicle_unit_offset[(offsetof(character_vehicle_properties, unit) == 0) ? 1 : -1];
typedef char character_vehicle_banshee_steering_offset[
    (offsetof(character_vehicle_properties, banshee_steering_maximum_angle) == 0x3c) ? 1 : -1];
typedef char character_vehicle_oversteering_offset[
    (offsetof(character_vehicle_properties, oversteering_bounds) == 0x4c) ? 1 : -1];
typedef char character_vehicle_charge_angle_offset[
    (offsetof(character_vehicle_properties, charge_tight_angle) == 0x90) ? 1 : -1];
typedef char character_vehicle_charge_limit_offset[
    (offsetof(character_vehicle_properties, charge_limit) == 0xcc) ? 1 : -1];
typedef char character_vehicle_charge_minimum_offset[
    (offsetof(character_vehicle_properties, charge_minimum) == 0xce) ? 1 : -1];

typedef char character_vehicle_obstacle_ignore_offset[(offsetof(character_vehicle_properties, obstacle_ignore_size) == 0xc8) ? 1 : -1];
typedef char character_vehicle_properties_size[(sizeof(character_vehicle_properties) == 0xd0) ? 1 : -1];
typedef char character_vehicle_key_offset[(offsetof(character_vehicle_properties, key) == 0xc) ? 1 : -1];
typedef char character_vehicle_flags_offset[(offsetof(character_vehicle_properties, flags) == 0x20) ? 1 : -1];
typedef char character_vehicle_pathfinding_radius_offset[(offsetof(character_vehicle_properties, pathfinding_radius) == 0x24) ? 1 : -1];
typedef char character_vehicle_steering_distance_offset[(offsetof(character_vehicle_properties, steering_distance) == 0x28) ? 1 : -1];
typedef char character_vehicle_deceleration_distance_offset[(offsetof(character_vehicle_properties, deceleration_distance) == 0x2c) ? 1 : -1];
typedef char character_vehicle_side_slip_distance_offset[(offsetof(character_vehicle_properties, side_slip_distance) == 0x54) ? 1 : -1];
typedef char character_vehicle_throttle_maximum_offset[(offsetof(character_vehicle_properties, throttle_maximum) == 0x68) ? 1 : -1];
typedef char character_vehicle_aiming_deviation_offset[(offsetof(character_vehicle_properties, maximum_aiming_deviation) == 0x88) ? 1 : -1];
typedef char character_vehicle_charge_limit_offset[(offsetof(character_vehicle_properties, charge_limit) == 0xcc) ? 1 : -1];
struct character_vitality_properties;
struct character_weapon_properties;

// The character tag's guardian properties block element (0x18 bytes; character_definition + 0x150). Read by
// actor_type_guardian.cpp: the surge behaviour submits surge_time as its action duration, the surge timer test
// waits surge_delay_time, the proximity impulse fires inside proximity_surge_distance, and the surge firing
// position search keeps current_position_distance from the actor and target_position_distance from its target.
struct character_guardian_properties
{
    float surge_time;
    float surge_delay_time;
    float proximity_surge_distance;
    float phase_time;
    float current_position_distance;
    float target_position_distance;
};
typedef char character_guardian_properties_size[(sizeof(character_guardian_properties) == 0x18) ? 1 : -1];

// Equipment selection weights and usage conditions.
struct character_equipment_condition
{
    short condition;
    short action;
    float normal_chance;
    float heroic_chance;
};

struct character_equipment_properties
{
    s_tag_reference equipment;
    unsigned long flags;
    float weight;
    long condition_count;
    TAG_POINTER(character_equipment_condition) conditions;
    long definition;
};

typedef char character_equipment_flags_offset[(offsetof(character_equipment_properties, flags) == 0x10) ? 1 : -1];

typedef char character_equipment_condition_size[(sizeof(character_equipment_condition) == 0xc) ? 1 : -1];
typedef char character_equipment_action_offset[(offsetof(character_equipment_condition, action) == 2) ? 1 : -1];
typedef char character_equipment_properties_size[(sizeof(character_equipment_properties) == 0x24) ? 1 : -1];
typedef char character_equipment_conditions_offset[(offsetof(character_equipment_properties, condition_count) == 0x18) ? 1 : -1];

// The character tag ('char'). Every unit that reads it walks the parent chain the same way: take the first
// non-empty properties block of the character or of its parent_character, so parent_character.index is the
// tag index of the parent (-1 for none). The block offsets are attested by the units that read them
// (character_definitions.cpp, cl_engine.cpp, squads.cpp, actor_type_scarab.cpp, actor_type_guardian.cpp,
// actor_moving.cpp); the tag continues past morph_properties with blocks no unit reads.
struct character_definition
{
    unsigned long flags;
    s_tag_reference parent_character;
    s_tag_reference unit;
    s_tag_reference creature;
    s_tag_reference style;
    s_tag_reference major_character;
    s_tag_block variants;
    s_tag_block voice_properties;
    s_tag_block general_properties;
    s_tag_block vitality_properties;
    s_tag_block placement_properties;
    s_tag_block perception_properties;
    s_tag_block look_properties;
    s_tag_block movement_properties;
    s_tag_block flocking_properties;
    s_tag_block swarm_properties;
    s_tag_block ready_properties;
    s_tag_block engage_properties;
    s_tag_block charge_properties;
    s_tag_block evasion_properties;
    s_tag_block cover_properties;
    s_tag_block retreat_properties;
    s_tag_block search_properties;
    s_tag_block presearch_properties;
    s_tag_block idle_properties;
    s_tag_block vocalization_properties;
    s_tag_block boarding_properties;
    s_tag_block guardian_properties;
    s_tag_block combatform_properties;
    s_tag_block scarab_properties;
    s_tag_block weapons_properties;
    s_tag_block firing_pattern_properties;
    s_tag_block grenades_properties;
    s_tag_block vehicle_properties;
    s_tag_block morph_properties;
    s_tag_block equipment_properties;             // actor_place: first nonempty block in the parent chain
    s_tag_block campaign_metagame;
    s_tag_block postures;                         // behavior_posture_find: count +0x1c8, address +0x1cc
};
typedef char character_definition_metagame_offset[(offsetof(character_definition, campaign_metagame) == 0x1bc) ? 1 : -1];
typedef char character_definition_postures_offset[(offsetof(character_definition, postures) == 0x1c8) ? 1 : -1];
typedef char character_definition_equipment_offset[(offsetof(character_definition, equipment_properties) == 0x1b0) ? 1 : -1];
typedef char character_definition_parent_offset[(offsetof(character_definition, parent_character.index) == 0x10) ? 1 : -1];
typedef char character_definition_voice_offset[(offsetof(character_definition, voice_properties) == 0x60) ? 1 : -1];
typedef char character_definition_placement_offset[(offsetof(character_definition, placement_properties) == 0x84) ? 1 : -1];
typedef char character_definition_movement_offset[(offsetof(character_definition, movement_properties) == 0xa8) ? 1 : -1];
typedef char character_definition_cover_offset[(offsetof(character_definition, cover_properties) == 0xfc) ? 1 : -1];
typedef char character_definition_guardian_offset[(offsetof(character_definition, guardian_properties) == 0x150) ? 1 : -1];
typedef char character_definition_scarab_offset[(offsetof(character_definition, scarab_properties) == 0x168) ? 1 : -1];
typedef char character_definition_morph_offset[(offsetof(character_definition, morph_properties) == 0x1a4) ? 1 : -1];

character_general_properties *character_definition_general_properties_get(long tag_index);
character_movement_properties *character_definition_movement_properties_get(long tag_index);
character_charge_properties *character_definition_charge_multi_properties_get(long player_index);
character_vehicle_properties *character_vehicle_properties_get(long actor_index, long key);
character_idle_properties *character_definition_idle_multi_properties_get(long actor_index);
character_grenade_properties *actor_grenade_properties_get(long player_index);
character_grenade_properties *character_grenade_properties_get(long tag_index, short identifier);
character_engage_properties *character_definition_engage_multi_properties_get(long player_index);
character_weapon_properties *character_weapon_properties_get(long definition_index, long weapon_definition_index);
character_firing_pattern_properties *actor_firing_pattern_properties_get(long player_index);
character_evasion_properties *character_definition_evasion_multi_properties_get(long actor_index);
character_cover_properties *character_definition_cover_multi_properties_get(long player_index);
character_retreat_properties *character_definition_retreat_multi_properties_get(long player_index);
character_presearch_properties *character_definition_presearch_multi_properties_get(long player_index);
character_search_properties *character_definition_search_multi_properties_get(long player_index);
character_vitality_properties *character_definition_vitality_properties_get(long tag_index);
character_morph_properties *character_definition_morph_properties_get(long tag_index);
character_swarm_properties *character_definition_swarm_multi_properties_get(long player_index);
character_ready_properties *character_definition_ready_multi_properties_get(long player_index);
character_combatform_properties *character_definition_combatform_multi_properties_get(long player_index);
character_weapon_properties *actor_weapon_properties_get(long actor_index);

struct s_character_charge_entry;

extern "C" s_character_charge_entry *character_definition_charge_entry_get(long player_index);

typedef char character_vehicle_style_offset[(offsetof(character_vehicle_properties, style) == 0x10) ? 1 : -1];

typedef char character_vehicle_approach_offset[(offsetof(character_vehicle_properties, approach_scale) == 0x98) ? 1 : -1];

#endif // AI_CHARACTER_DEFINITIONS_H
