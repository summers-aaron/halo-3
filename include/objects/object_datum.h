#ifndef OBJECTS_OBJECT_DATUM_H
#define OBJECTS_OBJECT_DATUM_H

#include <stddef.h>

#include "math/real_math_types.h"
#include "cseries/cseries_wrapped.h"
#include "memory/static_arrays_flags.h"
#include "objects/object_reference_types.h"
#include "objects/object_types.h"
#include "objects/object_flags.h"
#include "objects/object_damage_flags.h"
#include "tag_files/tag_types.h"

enum e_object_recycling_flags;

// Base object state shared by all object types.
struct object_datum {
    union {
        datum_index definition_index;
        long definition;
    };
    union {
        unsigned long flags;
        unsigned long object_flags;
        unsigned char flags_bytes[4];
        c_flags_no_init<e_object_data_flags, volatile unsigned long, 32> m_object_flags;
        c_flags_no_init<e_object_data_flags, unsigned long, 32> flag_bits;
    };
    datum_index next_object_index;
    datum_index first_child_object_index;
    union {
        datum_index parent_object_index;
        long parent;
    };
    char parent_node_index;
    union {
        unsigned char inhibited_flags;
        unsigned char flags15;
    };
    union {
        short scenario_datum_index;
        short name_index;
    };
    short map_variant_index;
    short pad1a;
    s_location location;
    real_point3d bounding_center;
    float bounding_radius;
    real_point3d bounding_sphere_center;
    float bounding_sphere_radius;
    real_point3d dynamic_light_sphere_center;
    float dynamic_light_sphere_radius;
    long first_cluster_reference;
    real_point3d position;
    real_vector3d forward;
    real_vector3d up;
    union {
        real_vector3d linear_velocity;
        real_vector3d translational_velocity;
    };
    real_vector3d angular_velocity;
    float scale;
    union {
        c_object_identifier object_identifier;
        c_object_identifier identifier;
        c_object_identifier parent_object_identifier;
        struct {
            long identifier_unique_id;
            short identifier_origin_bsp_index;
            union {
                signed char identifier_type;
                signed char object_type;
                signed char type;
            };
            signed char identifier_source;
        };
    };
    short object_name_index;
    char bsp_placement_policy;
    union {
        char netgame_equipment_index;
        unsigned char keyframed_contact_ticks;
    };
    union {
        datum_index havok_component_index;
        long havok_component;
        long component_index;
        long component;
    };
    long local_physics_object_index;
    long last_motion_time;                                  // object_physics_update: last tick the object was not at rest
    union {
        unsigned long physics_flags;
        c_flags_no_init<e_object_physics_flags, unsigned long, 19> m_physics_flags;
    };
    unsigned char high_frequency_rest_ticks;
    unsigned char low_frequency_rest_ticks;
    unsigned char phantom_power;
    unsigned char phantom_speed;
    short physics_reconnect_time;
    unsigned char scripted_timeout_ticks;
    union {
        unsigned char scripted_lifetime_ticks;
        unsigned char collision_damage_scale;
    };
    short environment_contact_ticks;
    signed char variant_index;
    union {
        unsigned char ai_flags;
        unsigned char flags_bb;
    };
    unsigned long damage_identifier;
    s_damage_owner damage_owner;
    short structure_bsp_fake_lightprobe_index;
    short padce;
    union {
        datum_index gamestate_index;
        long simulation_entity_index;
    };
    union {
        short destroyed_regions;
        short owner_team_index;
    };
    unsigned char flags_d6;
    signed char marker;
    long simulation_object_index;
    real_point3d simulation_update_position;
    datum_index first_widget_index;
    union {
        unsigned short broken_constraints;
        unsigned short destroyed_constraints;
    };
    unsigned short loosened_constraints;
    float maximum_body_vitality;
    float maximum_shield_vitality;
    float body_vitality;
    float shield_vitality;
    float current_shield_damage;
    float current_body_damage;
    float shield_damage;
    float body_damage;
    union {
        short shield_stun_ticks;
        unsigned short damage_recharge_flags;
    };
    short body_stun_ticks;
    union {
        unsigned long damage_flags;
        unsigned short damage_flags_words[2];
        c_flags_no_init<e_object_damage_flags, unsigned long, 32> m_damage_flags;
    };
    union {
        char recycling_variant;
        unsigned char shield_damage_reset;
        signed char shield_decay_timer;
    };
    union {
        char recycling_permutation;
        unsigned char body_damage_reset;
    };
    union {
        unsigned char recycling_flags;
        c_flags_no_init<e_object_recycling_flags, unsigned char, 6> m_recycling_flags;
    };
    char pad11b;
    datum_index next_garbage_object_index;
    long garbage_time;
    long parent_recycling_group;
    long next_recycling_group_member;
    // Tracks the object type parts whose new_proc has run and delete_proc has not.
    c_static_flags_no_init<16> type_part_flags;
    short pad130;
    s_object_block_reference original_node_orientations;
    s_object_block_reference node_orientations;
    union {
        s_object_block_reference node_matrices;
        s_object_block_reference nodes;
        struct {
            short node_matrices_size;
            short node_matrices_offset;
        };
    };
    union {
        s_object_block_reference region_information;
        s_object_block_reference regions;
        struct {
            short block_size;
            short block_offset;
        };
        struct {
            short region_information_size;
            short region_information_offset;
        };
    };
    s_object_block_reference attached_lights;
    union {
        s_object_block_reference damage_sections;
        struct {
            short damage_section_bytes;
            short damage_section_offset;
        };
    };
    s_object_block_reference change_colors;
    s_object_block_reference animation;
    union {
        s_object_block_reference multiplayer_properties;
        struct {
            short multiplayer_properties_size;
            short multiplayer_data_offset;
        };
    };
    long cinematic_object_index;
    long cinematic_light_index;
};

typedef char object_keyframed_contact_offset[(offsetof(object_datum, keyframed_contact_ticks) == 0x9f) ? 1 : -1];
typedef char object_collision_damage_scale_offset[(offsetof(object_datum, collision_damage_scale) == 0xb7) ? 1 : -1];
typedef char object_environment_contact_offset[(offsetof(object_datum, environment_contact_ticks) == 0xb8) ? 1 : -1];
typedef char object_cinematic_object_offset[(offsetof(object_datum, cinematic_object_index) == 0x158) ? 1 : -1];
typedef object_datum _object_datum;
typedef object_datum s_object_datum;

// Object header entries in object_header_data.
struct s_object_header {
    short identifier;
    unsigned char flags;
    unsigned char object_type;
    union {
        long pad4;
        struct {
            short cluster_index;
            short data_size;
        };
        s_cluster_reference cluster_reference;
    };
    union {
        long datum_offset;
        long pad8;
        int handle;
    };
    union {
        TAG_POINTER(object_datum) datum;
        TAG_POINTER(object_datum) object;
    };
};
typedef s_object_header object_header_datum;
typedef s_object_header s_object_header_datum;

typedef char object_dynamic_light_layout_1[(offsetof(object_datum, dynamic_light_sphere_center) == 0x40) ? 1 : -1];
typedef char object_dynamic_light_layout_2[(offsetof(object_datum, dynamic_light_sphere_radius) == 0x4c) ? 1 : -1];

typedef char obj_datum_size_datum[(sizeof(object_datum) == 0x160) ? 1 : -1];
typedef char obj_datum_offset_damage_identifier[(offsetof(object_datum, damage_identifier) == 0xbc) ? 1 : -1];
typedef char obj_datum_offset_damage_flags_words[(offsetof(object_datum, damage_flags_words) == 0x114) ? 1 : -1];
typedef char obj_datum_offset_shield_damage_reset[(offsetof(object_datum, shield_damage_reset) == 0x118) ? 1 : -1];
typedef char obj_datum_offset_body_damage_reset[(offsetof(object_datum, body_damage_reset) == 0x119) ? 1 : -1];
typedef char obj_datum_size_header[(sizeof(void *) != 4 || ((sizeof(s_object_header) == 16) )) ? 1 : -1];
typedef char obj_datum_offset_next_object_index[(offsetof(object_datum, next_object_index) == 0x08) ? 1 : -1];
typedef char obj_datum_offset_first_child_object_index[(offsetof(object_datum, first_child_object_index) == 0x0c) ? 1 : -1];
typedef char obj_datum_offset_parent_node_index[(offsetof(object_datum, parent_node_index) == 0x14) ? 1 : -1];
typedef char obj_datum_offset_scenario_datum_index[(offsetof(object_datum, scenario_datum_index) == 0x16) ? 1 : -1];
typedef char obj_datum_offset_map_variant_index[(offsetof(object_datum, map_variant_index) == 0x18) ? 1 : -1];
typedef char obj_datum_offset_location[(offsetof(object_datum, location) == 0x1c) ? 1 : -1];
typedef char obj_datum_offset_object_identifier[(offsetof(object_datum, object_identifier) == 0x94) ? 1 : -1];
typedef char obj_datum_offset_identifier_type[(offsetof(object_datum, identifier_type) == 0x9a) ? 1 : -1];
typedef char obj_datum_offset_physics_flags[(offsetof(object_datum, m_physics_flags) == 0xac) ? 1 : -1];
typedef char obj_datum_offset_type_part_flags[(offsetof(object_datum, type_part_flags) == 0x12c) ? 1 : -1];
typedef char obj_datum_offset_scripted_timeout_ticks[(offsetof(object_datum, scripted_timeout_ticks) == 0xb6) ? 1 : -1];
typedef char obj_header_offset_object_type[(offsetof(s_object_header, object_type) == 0x03) ? 1 : -1];
typedef char obj_header_offset_datum[(sizeof(void *) != 4 || ((offsetof(s_object_header, datum) == 0x0c) )) ? 1 : -1];

typedef char object_datum_offset_shield_damage[(offsetof(object_datum, shield_damage) == 0x108) ? 1 : -1];
typedef char object_datum_offset_body_damage[(offsetof(object_datum, body_damage) == 0x10c) ? 1 : -1];

typedef char object_datum_simulation_update_position_offset[(offsetof(object_datum, simulation_update_position) == 0xdc) ? 1 : -1];

#endif // OBJECTS_OBJECT_DATUM_H
