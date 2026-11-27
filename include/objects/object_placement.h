#ifndef OBJECTS_OBJECT_PLACEMENT_H
#define OBJECTS_OBJECT_PLACEMENT_H

#include <stddef.h>

extern "C" signed char g_object_placement_sky_objects_enabled;

struct s_simulation_object_creation_data;
extern "C" long object_placement_create_from_simulation(const s_simulation_object_creation_data *creation);

union real_point3d;
struct surface_identifier;
extern "C" unsigned char object_placement_find_surface_from_sphere(
    unsigned long active_mask, const real_point3d *point, float radius, surface_identifier *output);

void object_placement_initialize(void);
void object_placement_dispose(void);
void object_placement_initialize_for_new_map(void);
void object_placement_dispose_from_old_map(void);
void object_placement_initialize_for_new_structure_bsp(unsigned long activating_structure_bsp_mask);
void object_placement_dispose_from_old_structure_bsp(unsigned long);
void object_placement_prepare_for_new_zone_set(unsigned long old_mask, unsigned long new_mask);

class c_scenario_resource_registry;
struct s_game_non_bsp_zone_set;
void object_placement_prepare_for_designer_zone_set_switch(
    const s_game_non_bsp_zone_set *old_zone_set, const s_game_non_bsp_zone_set *new_zone_set, c_scenario_resource_registry *pending_zone_registry);

void object_activation_regions_initialize(void);
void object_activation_regions_delete_all(void);
extern "C" void object_activation_regions_invalidate(void);
extern "C" void object_activation_regions_dispose(void);

#include "math/matrix_math_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_types.h"
#include "objects/multiplayer_object_types.h"

struct object_marker {
    short node_index;
    short padding;
    union {
        real_matrix4x3 node_to_marker;
        real_matrix4x3 node_matrix;
    };
    union {
        real_matrix4x3 matrix;
        real_matrix4x3 world_matrix;
    };
    union {
        float scale;
        float length;
        long marker_name;
        long region;
    };
};
typedef object_marker s_object_marker;

struct s_object_region_permutation {
    long region_name;
    long permutation_name;
};

// Initial state used when creating an object.
struct object_placement_data {
    long definition_index;
    c_object_identifier object_identifier;
    union {
        long model_variant_index;
        long variant;
    };
    long scenario_datum_index;
    char bsp_placement_policy;
    unsigned long flags;
    real_point3d position;
    real_vector3d forward;
    real_vector3d up;
    real_vector3d linear_velocity;
    real_vector3d angular_velocity;
    float scale;
    long player_index;
    long owner_object_index;
    union {
        long team_index;
        long owner_team;
        long team;
    };
    s_damage_owner damage_owner;
    unsigned long active_change_colors;
    real_rgb_color change_colors[4];
    long region_permutation_count;
    s_object_region_permutation region_permutations[16];
    union {
        unsigned long region_permutation_flags;
        long unconnected_region_mask;
    };
    unsigned short broken_constraints;
    unsigned short loosened_constraints;
    union {
        short destroyed_constraints;
        short ai_state_type;
    };
    short ai_state_size;
    short ai_state_alignment_bits;
    bool placed_by_script;
    s_location location;
    union {
        bool multiplayer_cinematic_object;
        bool placement_flag;
    };
    union {
        long parent_object_index;
        long location_bsp_index;
    };
    union {
        long parent_marker;
        long location_cluster_index;
    };
    union {
        long child_marker;
        long location_leaf_index;
    };
    s_scenario_multiplayer_object_properties multiplayer_properties;
};
typedef object_placement_data s_object_placement_data;

typedef char obj_place_size_marker[(sizeof(object_marker) == 0x70) ? 1 : -1];
typedef char obj_place_size_reg_perm[(sizeof(s_object_region_permutation) == 8) ? 1 : -1];
typedef char obj_place_size_placement[(sizeof(object_placement_data) == 0x184) ? 1 : -1];
typedef char obj_place_variant_offset[(offsetof(object_placement_data, variant) == 0xc) ? 1 : -1];
typedef char obj_place_placement_flag_offset[(offsetof(object_placement_data, placement_flag) == 0x140) ? 1 : -1];
typedef char obj_place_multiplayer_properties_offset[(offsetof(object_placement_data, multiplayer_properties) == 0x150) ? 1 : -1];

// Datum of the object activation regions array (128 entries of 40 bytes).
struct object_placement_activation_region
{
    short identifier;
    long creation_time;
    real_rectangle3d bounds;
    unsigned long filter_info;
    s_location location;
};
typedef char object_placement_activation_region_size[(sizeof(object_placement_activation_region) == 40) ? 1 : -1];
typedef char object_placement_activation_region_time_offset[(offsetof(object_placement_activation_region, creation_time) == 4) ? 1 : -1];


// Functions defined in tu/objectslibxbox/object_placement.cpp that other units call.

struct s_scenario_object;
struct scenario;
struct scenario_object_palette_entry;

scenario_object_palette_entry *scenario_object_get_palette_entry(scenario *scenario_data, const s_scenario_object *scenario_object);
void object_placement_deactivate_on_zone_set_switch(long object_index, long zone_set_index);
void object_placement_create_global_objects(long game_mode, bool only_unplaced);
void object_placement_create_active_zone_set_objects(long mode);
extern "C" void object_placement_update_multiplayer_item_flags(long object_index);
extern "C" void object_activation_regions_update();
extern "C" void object_activation_regions_update_for_object(int object_index, unsigned char allocate_region);

struct s_tag_block;

const s_scenario_object *scenario_object_try_and_get(scenario *scenario, e_object_type type, long index);
bool object_placement_data_new_from_scenario_object(
    e_object_type type, long scenario_index, const s_scenario_object *object,
    s_tag_block *palette, bool force, bool cinematic, object_placement_data *placement);


typedef char object_placement_data_ai_state_type_offset[(offsetof(object_placement_data, ai_state_type) == 0x134) ? 1 : -1];
typedef char object_placement_data_ai_state_size_offset[(offsetof(object_placement_data, ai_state_size) == 0x136) ? 1 : -1];
typedef char object_placement_data_ai_state_alignment_bits_offset[(offsetof(object_placement_data, ai_state_alignment_bits) == 0x138) ? 1 : -1];

#include "objects/object_type_definition.h"

static __forceinline void object_type_get_placement_tag_block(e_object_type type, long *tag_block_offset, long *tag_block_element_size)
{
    *tag_block_offset = object_type_definition_get(type)->scenario_datums_offset;
    *tag_block_element_size = object_type_definition_get(type)->scenario_datum_size;
}

static __forceinline s_tag_block *scenario_get_object_type_scenario_datums(scenario *scenario_data, e_object_type type, long *datum_size)
{
    long placement_tag_block_offset;
    long placement_size;
    object_type_get_placement_tag_block(type, &placement_tag_block_offset, &placement_size);
    if (datum_size)
        *datum_size = placement_size;
    return (s_tag_block *)((char *)scenario_data + placement_tag_block_offset);
}
#endif // OBJECTS_OBJECT_PLACEMENT_H
