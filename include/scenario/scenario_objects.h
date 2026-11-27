#ifndef SCENARIO_SCENARIO_OBJECTS_H
#define SCENARIO_SCENARIO_OBJECTS_H

// The placed-object entries of the scenario tag's object blocks (scenery, crates, machines, giants, ...). Every
// entry starts with the common object part; object_type_definition::scenario_datums_offset and
// scenario_datum_size locate a type's block and its element size, and units reach the type-specific tail by the
// object's type.

#include <stddef.h>
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "objects/object_types.h"
#include "ai/scenario_object_reference_types.h"
#include "units/scenario_unit_datum_types.h"

// The common object part (object_placement.cpp names +0x00..+0x54; objects.cpp, swarms.cpp and
// sector_environment_objects.cpp read the identifier at +0x38).
struct s_scenario_object_node_orientations
{
    short node_count;
    c_typed_tag_block<unsigned char> bit_vector;
    c_typed_tag_block<short> orientations;
};

struct s_scenario_object
{
    short palette_index;
    short name_index;
    unsigned long placement_flags;
    real_point3d position;
    real_euler_angles3d rotation;
    float scale;
    c_typed_tag_block<s_scenario_object_node_orientations> node_orientations;
    unsigned short transform_flags;
    unsigned short manual_bsp_flags;
    long cinematic_name;
    c_object_identifier object_identifier;
    signed char bsp_placement_policy;
    signed char editing_bound_to_bsp;
    short editor_folder;
    char spare_before_parent_name_index[2];
    short parent_name_index;
    long parent_marker;
    long child_marker;
    unsigned short bsp_mask;
};

struct s_scenario_object_permutation
{
    s_scenario_object object;
    long model_variant;
    unsigned long active_colors;
    unsigned long colors[4];
};

// pathfinding_object_get_object_ref_index reads object_references of each type that pathfinding tracks.
struct s_scenario_scenery : s_scenario_object
{
    long model_variant;
    unsigned long active_colors;
    unsigned long colors[4];
    short pathfinding_policy;
    short lightmapping_policy;
    c_typed_tag_block<s_scenario_object_reference> object_references;
    short havok_mopp_index;
    short squad_index;
};

struct s_scenario_crate : s_scenario_object
{
    long model_variant;
    unsigned long active_colors;
    unsigned long colors[4];
    short pathfinding_policy;
    short lightmapping_policy;
    c_typed_tag_block<s_scenario_object_reference> object_references;
};

// device_machines.cpp reads the machine flags at +0x5c and the pathfinding policy at +0x6c.
struct s_scenario_machine : s_scenario_object
{
    short power_group;
    short position_group;
    unsigned long device_flags;
    unsigned long flags;
    c_typed_tag_block<s_scenario_object_reference> object_references;
    short pathfinding_policy;
};

struct s_scenario_biped : s_scenario_object
{
    long model_variant;
    unsigned long active_colors;
    unsigned long colors[4];
    s_scenario_unit_datum unit;
};

struct s_scenario_giant : s_scenario_object
{
    long model_variant;
    unsigned long active_colors;
    unsigned long colors[4];
    s_scenario_unit_datum unit;
    short pathfinding_policy;
    c_typed_tag_block<s_scenario_object_reference> object_references;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char scenario_permutation_size[(sizeof(s_scenario_object_permutation) == 0x6c) ? 1 : -1];
typedef char scenario_crate_lightmapping_policy_layout[(offsetof(s_scenario_crate, lightmapping_policy) == 0x6e) ? 1 : -1];
typedef char scenario_crate_model_variant_layout[(offsetof(s_scenario_crate, model_variant) == 0x54) ? 1 : -1];
typedef char scenario_giant_model_variant_layout[(offsetof(s_scenario_giant, model_variant) == 0x54) ? 1 : -1];
typedef char scenario_machine_device_flags_layout[(offsetof(s_scenario_machine, device_flags) == 0x58) ? 1 : -1];
typedef char scenario_machine_position_group_layout[(offsetof(s_scenario_machine, position_group) == 0x56) ? 1 : -1];
typedef char scenario_machine_power_group_layout[(offsetof(s_scenario_machine, power_group) == 0x54) ? 1 : -1];
typedef char scenario_object_manual_bsp_flags_layout[(offsetof(s_scenario_object, manual_bsp_flags) == 0x32) ? 1 : -1];
typedef char scenario_object_transform_flags_layout[(offsetof(s_scenario_object, transform_flags) == 0x30) ? 1 : -1];
typedef char scenario_scenery_lightmapping_policy_layout[(offsetof(s_scenario_scenery, lightmapping_policy) == 0x6e) ? 1 : -1];
typedef char scenario_scenery_model_variant_layout[(offsetof(s_scenario_scenery, model_variant) == 0x54) ? 1 : -1];
typedef char scenario_scenery_pathfinding_policy_layout[(offsetof(s_scenario_scenery, pathfinding_policy) == 0x6c) ? 1 : -1];

typedef char scenario_object_identifier_offset[(offsetof(s_scenario_object, object_identifier) == 0x38) ? 1 : -1];
typedef char scenario_object_editor_folder_offset[(offsetof(s_scenario_object, editor_folder) == 0x42) ? 1 : -1];
typedef char scenario_object_node_orientations_offset[(offsetof(s_scenario_object, node_orientations) == 0x24) ? 1 : -1];
typedef char scenario_object_cinematic_name_offset[(offsetof(s_scenario_object, cinematic_name) == 0x34) ? 1 : -1];
typedef char scenario_object_bsp_mask_offset[(offsetof(s_scenario_object, bsp_mask) == 0x50) ? 1 : -1];
typedef char scenario_scenery_object_references_offset[(offsetof(s_scenario_scenery, object_references) == 0x70) ? 1 : -1];
typedef char scenario_crate_object_references_offset[(offsetof(s_scenario_crate, object_references) == 0x70) ? 1 : -1];
typedef char scenario_machine_object_references_offset[(offsetof(s_scenario_machine, object_references) == 0x60) ? 1 : -1];
typedef char scenario_giant_object_references_offset[(offsetof(s_scenario_giant, object_references) == 0x78) ? 1 : -1];
typedef char scenario_crate_policy_offset[(offsetof(s_scenario_crate, pathfinding_policy) == 0x6c) ? 1 : -1];
typedef char scenario_machine_flags_offset[(offsetof(s_scenario_machine, flags) == 0x5c) ? 1 : -1];
typedef char scenario_machine_policy_offset[(offsetof(s_scenario_machine, pathfinding_policy) == 0x6c) ? 1 : -1];
typedef char scenario_giant_unit_offset[(offsetof(s_scenario_giant, unit) == 0x6c) ? 1 : -1];
typedef char scenario_biped_size[(sizeof(s_scenario_biped) == 0x74) ? 1 : -1];
typedef char scenario_giant_policy_offset[(offsetof(s_scenario_giant, pathfinding_policy) == 0x74) ? 1 : -1];
typedef char scenario_scenery_havok_mopp_offset[(offsetof(s_scenario_scenery, havok_mopp_index) == 0x7c) ? 1 : -1];
typedef char scenario_scenery_squad_offset[(offsetof(s_scenario_scenery, squad_index) == 0x7e) ? 1 : -1];
#endif

#endif // SCENARIO_SCENARIO_OBJECTS_H
