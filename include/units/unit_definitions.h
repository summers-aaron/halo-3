#ifndef UNITS_UNIT_DEFINITIONS_H
#define UNITS_UNIT_DEFINITIONS_H

#include <stddef.h>
#include "cseries/cseries_wrapped.h"
enum e_unit_definition_flags
{
    _unit_definition_has_no_aiming_bit = 10,
    _unit_definition_simple_creature_bit = 11,
    _unit_definition_allow_aim_while_opening_or_closing_bit = 29,
    k_unit_definition_flags_count = 29
};
#include "tag_files/tag_types.h"
#include "units/unit_seat.h"
#include "units/unit_camera.h"
#include "objects/object_definitions.h"

// Unit-tag fields shared by unit and camera readers.
// Offsets include the object definition. The trailing fields are read by the biped/giant paths.
struct unit_definition : s_object_definition
{
    union {
        unsigned long flags;
        struct {
#if defined(_XBOX) && !defined(__clang__)
            unsigned short flags_high;
            unsigned short flags_low;
#else
            unsigned short flags_low;
            unsigned short flags_high;
#endif
        };
        c_flags_no_init<e_unit_definition_flags, unsigned long, 29> m_flags;
    };
    short default_team;
    short constant_sound_volume;
    s_tag_block campaign_metagame;
    s_tag_reference integrated_light_toggle;
    float camera_field_of_view;
    float camera_stiffness;
    s_unit_camera camera;
    unit_seat_acceleration acceleration;
    float soft_ping_threshold;
    float soft_ping_time;                    // actions.cpp ping recovery

    float hard_ping_threshold;
    float hard_ping_time;
    float ping_threshold;
    float feign_death_threshold;
    float feign_death_time;
    float escape_distance;
    float stunned_movement_threshold;
    float feign_death_chance;
    float feign_repeat_chance;
    s_tag_reference spawned_actor;
    short spawned_actor_count_lower;
    short spawned_actor_count_upper;
    float spawned_velocity;
    float aiming_velocity_maximum;
    float aiming_acceleration_maximum;
    float aiming_scale;
    float looking_velocity_maximum;
    float looking_acceleration_maximum;
    long hand_markers[2];
    long weapon_marker;
    s_tag_reference melee_damage;
    s_tag_reference boarding_melee_damage;
    s_tag_reference boarding_response;
    s_tag_reference linked_seat_damage;
    s_tag_reference linked_seat_response;
    s_tag_reference landing_melee_damage;
    s_tag_reference flurry_melee_damage;
    s_tag_reference obstacle_smash_damage;
    short motion_sensor_blip_size;
    short scale_class; // item_adjust_scale selects its inventory size pair from +0x25a
    s_tag_block postures;
    c_typed_tag_block<s_tag_reference> hud_interfaces;
    s_tag_block dialogue_variants;
    float grenade_throw_speed;
    short grenade_type;
    short grenade_count;
    s_tag_block seat_accelerations;
    s_tag_block initial_weapons;
    c_typed_tag_block<unit_seat> seats;
    float emp_stun_time;
    s_tag_reference emp_effect;
    s_tag_reference boost_collision_damage;
    float boost_peak_power;
    float boost_peak_time;
    float boost_dead_time;
    float boost_decay_time;
    float boost_fraction;
    float lipsync_attack_weight;
    float lipsync_decay_weight;
    s_tag_reference detachment_damage;
    s_tag_reference detached_weapon;
};

typedef char unit_definition_camera_offset[(offsetof(unit_definition, camera) == 0x124) ? 1 : -1];

typedef char unit_definition_offset_camera_field_of_view[(offsetof(unit_definition, camera_field_of_view) == 0x11c) ? 1 : -1];
typedef char unit_definition_offset_flags[(offsetof(unit_definition, flags) == 0xf8) ? 1 : -1];
typedef char unit_definition_offset_acceleration[(offsetof(unit_definition, acceleration) == 0x160) ? 1 : -1];
typedef char unit_definition_offset_hand_markers[(offsetof(unit_definition, hand_markers) == 0x1cc) ? 1 : -1];
typedef char unit_definition_offset_melee_damage[(offsetof(unit_definition, melee_damage.index) == 0x1e4) ? 1 : -1];
typedef char unit_definition_offset_linked_seat_response[(offsetof(unit_definition, linked_seat_response.index) == 0x224) ? 1 : -1];
typedef char unit_definition_offset_hud_interfaces[(offsetof(unit_definition, hud_interfaces) == 0x268) ? 1 : -1];
typedef char unit_definition_offset_grenade_throw_speed[(offsetof(unit_definition, grenade_throw_speed) == 0x280) ? 1 : -1];
typedef char unit_definition_offset_grenade_type[(offsetof(unit_definition, grenade_type) == 0x284) ? 1 : -1];
typedef char unit_definition_offset_seats[(offsetof(unit_definition, seats) == 0x2a0) ? 1 : -1];
typedef char unit_definition_offset_boost_peak_power[(offsetof(unit_definition, boost_peak_power) == 0x2d0) ? 1 : -1];
typedef char unit_definition_offset_boost_decay_time[(offsetof(unit_definition, boost_decay_time) == 0x2dc) ? 1 : -1];

typedef char unit_definition_acceleration_offset[(offsetof(unit_definition, acceleration_scale) == 0x14) ? 1 : -1];
typedef char unit_definition_child_offset[(offsetof(unit_definition, crate_object.index) == 0x50) ? 1 : -1];
typedef char unit_definition_object_flags_offset[(offsetof(unit_definition, object_flags) == 2) ? 1 : -1];

typedef char unit_definition_gunner_character_offset[(offsetof(unit_definition, spawned_actor.index) == 0x1ac) ? 1 : -1];

typedef char unit_definition_soft_ping_time_offset[(offsetof(unit_definition, soft_ping_time) == 0x178) ? 1 : -1];
typedef char unit_definition_hard_ping_time_offset[(offsetof(unit_definition, hard_ping_time) == 0x180) ? 1 : -1];

typedef char unit_definition_blip_size_offset[(offsetof(unit_definition, motion_sensor_blip_size) == 0x258) ? 1 : -1];

typedef char unit_sweetener_offset[(offsetof(unit_definition, sweetener_size) == 0x1a) ? 1 : -1];
typedef char unit_material_effects_offset[(offsetof(unit_definition, material_effects.index) == 0x8c) ? 1 : -1];
typedef char unit_smash_damage_offset[(offsetof(unit_definition, obstacle_smash_damage.index) == 0x254) ? 1 : -1];

typedef char unit_definition_metagame_offset[(offsetof(unit_definition, campaign_metagame) == 0x100) ? 1 : -1];

typedef char unit_escape_distance_offset[(offsetof(unit_definition, escape_distance) == 0x190) ? 1 : -1];

typedef char unit_definition_boarding_melee_damage_index_offset[(offsetof(unit_definition, boarding_melee_damage.index) == 0x1f4) ? 1 : -1];
typedef char unit_definition_boarding_response_index_offset[(offsetof(unit_definition, boarding_response.index) == 0x204) ? 1 : -1];
typedef char unit_definition_linked_seat_damage_index_offset[(offsetof(unit_definition, linked_seat_damage.index) == 0x214) ? 1 : -1];
typedef char unit_definition_landing_melee_damage_index_offset[(offsetof(unit_definition, landing_melee_damage.index) == 0x234) ? 1 : -1];
typedef char unit_definition_flurry_melee_damage_index_offset[(offsetof(unit_definition, flurry_melee_damage.index) == 0x244) ? 1 : -1];
typedef char unit_definition_emp_effect_index_offset[(offsetof(unit_definition, emp_effect.index) == 0x2bc) ? 1 : -1];
typedef char unit_definition_detachment_damage_index_offset[(offsetof(unit_definition, detachment_damage.index) == 0x2f8) ? 1 : -1];
typedef char unit_definition_detached_weapon_index_offset[(offsetof(unit_definition, detached_weapon.index) == 0x308) ? 1 : -1];
typedef char unit_definition_size[(sizeof(unit_definition) == 0x30c) ? 1 : -1];
typedef char unit_definition_integrated_light_offset[(offsetof(unit_definition, integrated_light_toggle) == 0x10c) ? 1 : -1];
typedef char unit_definition_feign_death_offset[(offsetof(unit_definition, feign_death_threshold) == 0x188) ? 1 : -1];
typedef char unit_definition_spawned_velocity_offset[(offsetof(unit_definition, spawned_velocity) == 0x1b4) ? 1 : -1];
typedef char unit_definition_postures_offset[(offsetof(unit_definition, postures) == 0x25c) ? 1 : -1];
typedef char unit_definition_dialogue_variants_offset[(offsetof(unit_definition, dialogue_variants) == 0x274) ? 1 : -1];
typedef char unit_definition_boost_collision_offset[(offsetof(unit_definition, boost_collision_damage) == 0x2c0) ? 1 : -1];
typedef char unit_definition_lipsync_offset[(offsetof(unit_definition, lipsync_attack_weight) == 0x2e4) ? 1 : -1];

#endif // UNITS_UNIT_DEFINITIONS_H
