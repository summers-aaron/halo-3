#ifndef ITEMS_WEAPON_DEFINITIONS_H
#define ITEMS_WEAPON_DEFINITIONS_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"
#include "items/weapon_definition_flags.h"
#include "cseries/cseries_wrapped.h"
#include "items/item_definitions.h"

struct s_weapon_melee_damage
{
    s_tag_reference damage;
    s_tag_reference response;
};

struct s_magazine_ammunition {
    short rounds;
    unsigned long equipment_group_tag;
    TAG_POINTER(const char) equipment_name;
    long equipment_name_length;
    long definition_index;
};
struct s_weapon_magazine_definition {
    unsigned long flags;
    short rounds_recharged;
    short rounds_total_initial;
    short rounds_total_maximum;
    short rounds_loaded_maximum;
    short rounds_inventory_maximum;         /* (rounds_total_maximum) */
    float reload_time;
    short rounds_reload;
    float chamber_time;
    // Reserved bytes in the serialized magazine definition.
    char spare_after_chamber_time[0x34 - 0x1c];
    unsigned long reload_effect_group_tag;
    TAG_POINTER(const char) reload_effect_name;
    long reload_effect_name_length;
    long reload_effect;
    s_tag_reference reload_damage_effect;
    unsigned long chamber_effect_group_tag;
    TAG_POINTER(const char) chamber_effect_name;
    long chamber_effect_name_length;
    long chamber_effect;
    s_tag_reference chamber_damage_effect;
    long ammunition_count;
    TAG_POINTER(s_magazine_ammunition) ammunition;
    long ammunition_definition;
};
struct s_weapon_trigger_definition {
    long flags;
    short input;
    short behavior;
    short primary_barrel;
    short barrel_index;
    short prediction_type; // simulation_game_build_predicted_weapon: +0x0c
    float autofire_time;
    float autofire_threshold;
    short secondary_action;
    short primary_action;
    float charging_time;
    float charged_time;
    short overcharged_action;
    unsigned short cancelled_trigger_throw;
    float charged_illumination;
    float release_time;
    unsigned long charging_effect_group_tag;
    TAG_POINTER(const char) charging_effect_name;
    long charging_effect_name_length;
    long charging_effect; // tag-reference index
    unsigned long charging_damage_group_tag;
    TAG_POINTER(const char) charging_damage_name;
    long charging_damage_name_length;
    long charging_damage; // tag-reference index
    unsigned long charging_damage_response_group_tag;
    TAG_POINTER(const char) charging_damage_response_name;
    long charging_damage_response_name_length;
    long charging_damage_response; // tag-reference index
    float charging_age_degeneration;
    unsigned long release_effect_group_tag;
    TAG_POINTER(const char) release_effect_name;
    long release_effect_name_length;
    long release_effect; // tag-reference index
    unsigned long release_damage_group_tag;
    TAG_POINTER(const char) release_damage_name;
    long release_damage_name_length;
    long release_damage; // tag-reference index
    float lock_time;
    float locked_time;
    float tracking_time;
};
/* one barrel firing effect: its shot count range and the effect/damage/rider damage per firing kind (fire, misfire, empty, secondary) */
struct s_barrel_firing_effect {
    short shot_count_lower_bound;
    short shot_count_upper_bound;
    s_tag_reference effects[4];
    s_tag_reference damage_effects[4];
    s_tag_reference rider_damage_effects[4]; /* (0xc4 bytes) */
};
enum e_weapon_barrel_definition_flags
{
    _weapon_barrel_tracks_target = 13
};
struct s_weapon_barrel_definition {
    union
    {
        unsigned long flags;
        c_flags_no_init<e_weapon_barrel_definition_flags, unsigned long, 15> barrel_flags;
    };
    float rounds_per_second_lower;
    float rounds_per_second_upper;
    float acceleration_time;
    float deceleration_time;
    float firing_noise_scale;
    float firing_effect_threshold;
    short shots_minimum;
    short shots_maximum;
    float recovery_time;
    float recovery_fraction;
    short magazine_index;
    short minimum_rounds;
    short rounds_per_shot;
    short tracer_frequency;
    long marker_name;
    short prediction_type;
    short firing_noise;
    float firing_error_acceleration_time;
    float firing_error_deceleration_time;
    float direction_error_lower;
    float direction_error_upper;
    float pitch_min;
    float pitch_max;
    float pitch_exponent;
    float dual_error_acceleration_time;
    float dual_error_deceleration_time;
    float dual_error_rate;
    float dual_error_decay_rate;
    float dual_error_minimum;
    float dual_error_lower;
    float dual_error_upper;
    float dual_damage_scale;
    short distribution_function;
    short projectile_count;
    float spread_angle;
    float error_minimum;
    float error_lower;
    float error_upper;
    long first_person_offset_count;
    TAG_POINTER(real_point3d) first_person_offsets;
    long first_person_offset_definition;
    unsigned char damage_reporting_type;
    unsigned long projectile_group_tag;
    TAG_POINTER(const char) projectile_name;
    long projectile_name_length;
    long projectile_index;
    unsigned long secondary_projectile_index_group_tag;
    TAG_POINTER(const char) secondary_projectile_index_name;
    long secondary_projectile_index_name_length;
    long secondary_projectile_index; // tag-reference index
    unsigned long damage_effect_index_group_tag;
    TAG_POINTER(const char) damage_effect_index_name;
    long damage_effect_index_name_length;
    long damage_effect_index; // tag-reference index
    unsigned long crate_index_group_tag;
    TAG_POINTER(const char) crate_index_name;
    long crate_index_name_length;
    long crate_index; // tag-reference index
    float crate_velocity;
    float ejection_port_threshold;
    float ejection_port_recovery_time;
    float heat_recovery_threshold;          /* (heat generated per round) */
    float amount_a;
    float amount_b;
    float overload_time;
    float firing_recoil_minimum;
    float firing_recoil_maximum;
    float recoil_acceleration_time;
    float recoil_deceleration_time;
    short firing_recoil_function;
    float error_rate;
    float error_decay_rate;
    float illumination_recovery_rate;
    float ejection_port_recovery_rate;
    float rate_of_fire_acceleration;
    float rate_of_fire_deceleration;
    float error_rate_b;
    float error_decay_rate_b;
    long firing_effect_count;
    TAG_POINTER(s_barrel_firing_effect) firing_effects;
    long firing_effect_definition;          /* (0x134 bytes) */
};
typedef char weapon_item_definition_size[(sizeof(s_item_definition) == 0x1ac) ? 1 : -1];
struct weapon_definition {
    s_item_definition item;
    union {
        unsigned long flags;
        c_flags_no_init<e_weapon_definition_flags, unsigned long, 30> m_flags;
    };
    unsigned long secondary_flags;          // actions.cpp melee dash suppression
    long unused_label;
    short secondary_trigger_mode;
    short maximum_alternate_shots_loaded;
    float ready_duration;
    float ready_time;
    unsigned long ready_effect_group_tag;
    TAG_POINTER(const char) ready_effect_name;
    long ready_effect_name_length;
    long ready_effect;
    unsigned long ready_damage_effect_group_tag;
    TAG_POINTER(const char) ready_damage_effect_name;
    long ready_damage_effect_name_length;
    long ready_damage_effect; // tag-reference index
    float heat_recovery_threshold;
    float overheated_threshold;
    float heat_detonation_threshold;
    float heat_detonation_fraction;
    float heat_loss_per_second;
    float heat_illumination;
    float overheated_heat_loss_per_second;
    unsigned long overheated_effect_group_tag;
    TAG_POINTER(const char) overheated_effect_name;
    long overheated_effect_name_length;
    long overheated_effect; // tag-reference index
    unsigned long overheated_damage_effect_group_tag;
    TAG_POINTER(const char) overheated_damage_effect_name;
    long overheated_damage_effect_name_length;
    long overheated_damage_effect; // tag-reference index
    s_tag_reference detonation_effect;
    s_tag_reference detonation_damage_effect;
    unsigned long melee_damage_group_tag;
    TAG_POINTER(const char) melee_damage_name;
    long melee_damage_name_length;
    long melee_damage;
    unsigned long melee_response_group_tag;
    TAG_POINTER(const char) melee_response_name;
    long melee_response_name_length;
    long melee_response;
    real_vector3d melee_bounds;
    s_weapon_melee_damage melee_damage_parameters[6];
    unsigned long clang_effect_group_tag;
    TAG_POINTER(const char) clang_effect_name;
    long clang_effect_name_length;
    long clang_effect;
    unsigned char reporting_type;
    short zoom_level_count;
    float zoom_magnification_minimum;
    float zoom_magnification_maximum;
    float autoaim_angle;
    float autoaim_range;
    float autoaim_falloff_range;
    float magnetism_angle;
    float magnetism_range;
    float magnetism_falloff_range;
    float deviation_angle;
    // Reserved bytes in the serialized aim-assist parameters.
    char spare_after_deviation_angle[0x378 - 0x364];
    short movement_penalty_type;
    float movement_penalty_forward;
    float movement_penalty_sideways;
    float scariness;
    float power_on_time;
    float power_off_time;
    unsigned long turn_on_effect_group_tag;
    TAG_POINTER(const char) turn_on_effect_name;
    long turn_on_effect_name_length;
    long turn_on_effect;
    unsigned long turn_off_effect_group_tag;
    TAG_POINTER(const char) turn_off_effect_name;
    long turn_off_effect_name_length;
    long turn_off_effect; // tag-reference index
    float heat_loss_age_scale;
    float heat_rate_scale;
    float misfire_start;
    float misfire_chance;
    unsigned long pickup_sound_group_tag;
    TAG_POINTER(const char) pickup_sound_name;
    long pickup_sound_name_length;
    long pickup_sound; // tag-reference index
    unsigned long zoom_in_sound_group_tag;
    TAG_POINTER(const char) zoom_in_sound_name;
    long zoom_in_sound_name_length;
    long zoom_in_sound;
    unsigned long zoom_out_sound_group_tag;
    TAG_POINTER(const char) zoom_out_sound_name;
    long zoom_out_sound_name_length;
    long zoom_out_sound;
    float active_camo_ding;
    float active_camo_regrowth_rate;
    long marker_name;
    long animation_weapon_class;
    long animation_weapon_type;
    short multiplayer_weapon_type;
    short reload_interrupt_mode;
    short tracking_type;
    s_tag_reference shared_interface;
    s_tag_block first_person_interfaces;
    s_tag_reference chud_interface;
    s_tag_block predicted_resources;
    c_typed_tag_block<s_weapon_magazine_definition> magazines;
    union {
        struct {
            long trigger_count;
            TAG_POINTER(s_weapon_trigger_definition) triggers;
            long trigger_definition;
        };
        c_typed_tag_block<s_weapon_trigger_definition> trigger_definitions;
    };
    long barrel_count;
    TAG_POINTER(s_weapon_barrel_definition) barrels;
    long barrel_definition;
    float turn_on_speed;
    float turn_off_speed;
    float movement_acceleration;
    float movement_deceleration;
    float turning_acceleration;
    float turning_deceleration;
    unsigned long deployed_vehicle_group_tag;
    TAG_POINTER(const char) deployed_vehicle_name;
    long deployed_vehicle_name_length;
    long deployed_vehicle;                  // actions.cpp deploy_turret
    unsigned long replacement_definition_group_tag;
    TAG_POINTER(const char) replacement_definition_name;
    long replacement_definition_name_length;
    long replacement_definition_index;
    unsigned long depleted_effect_group_tag;
    TAG_POINTER(const char) depleted_effect_name;
    long depleted_effect_name_length;
    long depleted_effect; // tag-reference index
    unsigned long ammunition_weapon_definition_group_tag;
    TAG_POINTER(const char) ammunition_weapon_definition_name;
    long ammunition_weapon_definition_name_length;
    long ammunition_weapon_definition_index;
    unsigned long full_age_material_effects_group_tag;
    TAG_POINTER(const char) full_age_material_effects_name;
    long full_age_material_effects_name_length;
    long full_age_material_effects;
    float age_per_round;
    float campaign_age_per_round;
    real_point3d first_person_weapon_offset;
    real_vector2d first_person_scope_size;
    real_bounds third_person_pitch_bounds;
    float field_of_view_change_time;
    float ready_duration_b;
    long stowed_marker_name;
};

typedef char weapon_definition_flags_offset[(offsetof(weapon_definition, flags) == 0x1ac) ? 1 : -1];
typedef char weapon_definition_magazines_offset[(offsetof(weapon_definition, magazines) == 0x444) ? 1 : -1];
typedef char weapon_definition_triggers_offset[(offsetof(weapon_definition, trigger_count) == 0x450) ? 1 : -1];
typedef char weapon_definition_picked_up_message_offset[(offsetof(weapon_definition, item.picked_up_message) == 0x118) ? 1 : -1];
typedef char weapon_definition_reporting_offset[(offsetof(weapon_definition, reporting_type) == 0x33c) ? 1 : -1];
typedef char weapon_definition_zoom_offset[(offsetof(weapon_definition, zoom_level_count) == 0x33e) ? 1 : -1];
typedef char weapon_definition_fov_offset[(offsetof(weapon_definition, field_of_view_change_time) == 0x4f4) ? 1 : -1];
typedef char weapon_definition_reload_interrupt_offset[(offsetof(weapon_definition, reload_interrupt_mode) == 0x406) ? 1 : -1];
typedef char weapon_definition_tracking_offset[(offsetof(weapon_definition, tracking_type) == 0x408) ? 1 : -1];
struct weapon_first_person_interface_definition;

const weapon_first_person_interface_definition *weapon_definition_get_first_person_interface_by_character(
    const weapon_definition *definition, long character_type);

typedef char weapon_magazine_definition_size[(sizeof(s_weapon_magazine_definition) == 0x80) ? 1 : -1];
typedef char weapon_melee_damage_size[(sizeof(s_weapon_melee_damage) == 0x20) ? 1 : -1];
typedef char weapon_definition_melee_damage_offset[(offsetof(weapon_definition, melee_damage) == 0x24c) ? 1 : -1];
typedef char weapon_definition_melee_bounds_offset[(offsetof(weapon_definition, melee_bounds) == 0x260) ? 1 : -1];
typedef char weapon_definition_melee_parameters_offset[(offsetof(weapon_definition, melee_damage_parameters) == 0x26c) ? 1 : -1];
typedef char weapon_definition_zoom_levels_offset[(offsetof(weapon_definition, zoom_level_count) == 0x33e) ? 1 : -1];
typedef char weapon_definition_zoom_out_offset[(offsetof(weapon_definition, zoom_out_sound) == 0x3ec) ? 1 : -1];
typedef char weapon_definition_first_person_interfaces_offset[(offsetof(weapon_definition, first_person_interfaces) == 0x41c) ? 1 : -1];
typedef char weapon_definition_full_age_effects_offset[(offsetof(weapon_definition, full_age_material_effects) == 0x4cc) ? 1 : -1];

typedef char weapon_age_per_round_offset[(offsetof(weapon_definition, age_per_round) == 0x4d0) ? 1 : -1];
typedef char weapon_campaign_age_per_round_offset[(offsetof(weapon_definition, campaign_age_per_round) == 0x4d4) ? 1 : -1];
typedef char weapon_fov_change_time_offset[(sizeof(void *) != 4 || (offsetof(weapon_definition, field_of_view_change_time) == 0x4f4)) ? 1 : -1];

typedef char weapon_secondary_flags_offset[(offsetof(weapon_definition, secondary_flags) == 0x1b0) ? 1 : -1];
typedef char weapon_deployed_vehicle_offset[(offsetof(weapon_definition, deployed_vehicle) == 0x48c) ? 1 : -1];

typedef char weapon_autoaim_offset[(offsetof(weapon_definition, autoaim_angle) == 0x348) ? 1 : -1];
typedef char weapon_deviation_offset[(offsetof(weapon_definition, deviation_angle) == 0x360) ? 1 : -1];
typedef char weapon_trigger_behavior_offset[(offsetof(s_weapon_trigger_definition, behavior) == 6) ? 1 : -1];
typedef char weapon_trigger_prediction_offset[(offsetof(s_weapon_trigger_definition, prediction_type) == 0xc) ? 1 : -1];
typedef char weapon_detonation_effect_offset[(offsetof(weapon_definition, detonation_effect.index) == 0x22c) ? 1 : -1];
typedef char weapon_detonation_damage_offset[(offsetof(weapon_definition, detonation_damage_effect.index) == 0x23c) ? 1 : -1];
typedef char weapon_definition_size[(sizeof(weapon_definition) == 0x500) ? 1 : -1];
typedef char weapon_trigger_definition_size[(sizeof(s_weapon_trigger_definition) == 0x90) ? 1 : -1];
typedef char weapon_barrel_definition_size[(sizeof(s_weapon_barrel_definition) == 0x134) ? 1 : -1];
typedef char weapon_magazine_ammunition_size[(sizeof(s_magazine_ammunition) == 0x14) ? 1 : -1];
typedef char weapon_magazine_reload_time_offset[(offsetof(s_weapon_magazine_definition, reload_time) == 0x10) ? 1 : -1];
typedef char weapon_magazine_chamber_time_offset[(offsetof(s_weapon_magazine_definition, chamber_time) == 0x18) ? 1 : -1];
typedef char weapon_magazine_reload_effect_offset[(offsetof(s_weapon_magazine_definition, reload_effect_group_tag) == 0x34) ? 1 : -1];
typedef char weapon_movement_penalty_type_offset[(offsetof(weapon_definition, movement_penalty_type) == 0x378) ? 1 : -1];
typedef char weapon_magazine_ammunition_group_offset[(offsetof(s_magazine_ammunition, equipment_group_tag) == 4) ? 1 : -1];
typedef char weapon_magazine_reload_damage_offset[(offsetof(s_weapon_magazine_definition, reload_damage_effect) == 0x44) ? 1 : -1];
typedef char weapon_magazine_chamber_damage_offset[(offsetof(s_weapon_magazine_definition, chamber_damage_effect) == 0x64) ? 1 : -1];
typedef char weapon_barrel_acceleration_offset[(offsetof(s_weapon_barrel_definition, acceleration_time) == 0xc) ? 1 : -1];
typedef char weapon_barrel_projectile_group_offset[(offsetof(s_weapon_barrel_definition, projectile_group_tag) == 0x98) ? 1 : -1];
typedef char weapon_barrel_overload_offset[(offsetof(s_weapon_barrel_definition, overload_time) == 0xf0) ? 1 : -1];
typedef char weapon_barrel_error_rate_offset[(offsetof(s_weapon_barrel_definition, error_rate) == 0x108) ? 1 : -1];
typedef char weapon_movement_penalty_offset[(offsetof(weapon_definition, movement_penalty_forward) == 0x37c) ? 1 : -1];
typedef char weapon_trigger_autofire_offset[(offsetof(s_weapon_trigger_definition, autofire_time) == 0x10) ? 1 : -1];
typedef char weapon_trigger_secondary_action_offset[(offsetof(s_weapon_trigger_definition, secondary_action) == 0x18) ? 1 : -1];
typedef char weapon_trigger_illumination_offset[(offsetof(s_weapon_trigger_definition, charged_illumination) == 0x28) ? 1 : -1];
typedef char weapon_heat_recovery_offset[(offsetof(weapon_definition, heat_recovery_threshold) == 0x1e4) ? 1 : -1];
typedef char weapon_heat_illumination_offset[(offsetof(weapon_definition, heat_illumination) == 0x1f8) ? 1 : -1];

#endif // ITEMS_WEAPON_DEFINITIONS_H
