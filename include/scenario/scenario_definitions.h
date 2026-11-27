#ifndef SCENARIO_SCENARIO_DEFINITIONS_H
#define SCENARIO_SCENARIO_DEFINITIONS_H

// Functions defined in tu/blamlibxbox/scenario_definitions.cpp that other units call.

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "objects/object_types.h"

// Sky scenery references: 20-byte entries, one per sky object;
// March 82be6de0 reads the tag reference and the object name at +0x10.
struct scenario_sky_reference
{
    s_tag_reference object;
    unsigned short name_index;
    unsigned short active_bsps;
};

typedef char scenario_sky_reference_size[(sizeof(scenario_sky_reference) == 20) ? 1 : -1];
typedef char scenario_sky_reference_name_offset[(offsetof(scenario_sky_reference, name_index) == 16) ? 1 : -1];
typedef char scenario_sky_reference_bsps_offset[(offsetof(scenario_sky_reference, active_bsps) == 18) ? 1 : -1];

// Per-cluster visible portal list, read by visibility_portal_traversal.
struct s_scenario_cluster_portals
{
    s_tag_block portal_indices;
};

struct s_scenario_bsp_pvs
{
    s_tag_block cluster_pvs;
    s_tag_block cluster_pvs_doors_closed;
    s_tag_block cluster_attached_sky_indices;   // (one signed char per cluster; read by scenario_sky.cpp)
    s_tag_block cluster_visible_sky_indices;
    // March old_render_debug.cpp: BIT_VECTOR_SIZE_IN_LONGS(structure->clusters.count)
    // == structure_bsp_pvs->cluster_multiple_skies_visible_bit_vector.count.
    s_tag_block cluster_multiple_skies_visible_bit_vector;
    s_tag_block cluster_visibility;
    s_tag_block cluster_portals;
};

// 12-byte machine door object
// references from the first block in each per-BSP mapping.
struct s_scenario_machine_door_reference
{
    c_object_identifier identifier;
    unsigned short portal_index;
    unsigned short flags;
};
typedef char scenario_machine_door_reference_size[(sizeof(s_scenario_machine_door_reference) == 12) ? 1 : -1];

// March game_get_machine_door_portal_reference (82c47c78) reads the second
// block (+0xc) in this 24-byte per-BSP mapping.
struct s_scenario_bsp_machine_door_mapping
{
    s_tag_block machine_door_objects;
    s_tag_block machine_door_portal_references;
};

struct s_scenario_zone_set_visibility
{
    unsigned long structure_bsp_mask;
    short version;
    unsigned short flags;
    s_tag_block bsp_checksums;
    __forceinline bool contains_bsp(long index) const { return (structure_bsp_mask & (1UL << index)) != 0; }
    s_tag_block structure_bsp_pvs;
    s_tag_block machine_door_mappings;
};

typedef char scenario_bsp_pvs_size[(sizeof(s_scenario_bsp_pvs) == 84) ? 1 : -1];
typedef char scenario_bsp_sky_indices_offset[(offsetof(s_scenario_bsp_pvs, cluster_visible_sky_indices) == 36) ? 1 : -1];
typedef char scenario_bsp_skies_bit_vector_offset[(offsetof(s_scenario_bsp_pvs, cluster_multiple_skies_visible_bit_vector) == 48) ? 1 : -1];
typedef char scenario_zone_set_visibility_size[(sizeof(s_scenario_zone_set_visibility) == 44) ? 1 : -1];
typedef char scenario_zone_set_visibility_bsp_offset[(offsetof(s_scenario_zone_set_visibility, structure_bsp_pvs) == 20) ? 1 : -1];
typedef char scenario_zone_set_visibility_doors_offset[(offsetof(s_scenario_zone_set_visibility, machine_door_mappings) == 32) ? 1 : -1];
typedef char scenario_bsp_machine_door_mapping_size[(sizeof(s_scenario_bsp_machine_door_mapping) == 24) ? 1 : -1];

struct s_scenario_zone_set
{
    long name;
    long pvs_index;
    long flags;
    unsigned long bsp_active_mask;
    unsigned long designer_zone_required_mask;
    unsigned long designer_zone_forbidden_mask;
    unsigned long cinematic_zones;
    long hint_previous_zone_set;
    long audibility_index; // actor_perception_initialize_audibility_table reads zone-set +0x20.
};

typedef char scenario_zone_set_size[(sizeof(s_scenario_zone_set) == 0x24) ? 1 : -1];
typedef char scenario_zone_set_bsp_mask_offset[(offsetof(s_scenario_zone_set, bsp_active_mask) == 0xc) ? 1 : -1];
typedef char scenario_zone_set_required_mask_offset[(offsetof(s_scenario_zone_set, designer_zone_required_mask) == 0x10) ? 1 : -1];
typedef char scenario_zone_set_forbidden_mask_offset[(offsetof(s_scenario_zone_set, designer_zone_forbidden_mask) == 0x14) ? 1 : -1];
typedef char scenario_zone_set_cinematic_mask_offset[(offsetof(s_scenario_zone_set, cinematic_zones) == 0x18) ? 1 : -1];

// scenario zone_set_switch_trigger_volumes block element.
struct s_scenario_zone_set_switch_trigger_volume
{
    unsigned short flags;
    short begin_zone_set;
    short trigger_volume;
    short commit_zone_set;
};
typedef char scenario_zone_set_switch_trigger_volume_size[(sizeof(s_scenario_zone_set_switch_trigger_volume) == 0x8) ? 1 : -1];

struct scenario;

long scenario_zone_set_structure_bsp_cluster_visible_sky_index_get( const scenario *scenario_definition, long zone_set_index,
    long structure_bsp_index, long cluster_index);

#endif // SCENARIO_SCENARIO_DEFINITIONS_H
