#ifndef CACHE_CACHE_FILE_RESOURCE_GESTALT_H
#define CACHE_CACHE_FILE_RESOURCE_GESTALT_H

#include <stddef.h>
#include "cseries/cseries.h"
#include "tag_files/tag_groups.h"
#include "tag_files/tag_resource_interop_location_types.h"

struct s_cache_file_zone_resource_usage
{
    unsigned long required_pageable_size;
    unsigned long deferred_required_size;
    unsigned long optional_memory_size;
    unsigned long streamed_size;
    unsigned long dvd_memory_size;
};

struct s_cache_file_zone_manifest
{
    c_typed_tag_block<unsigned long> required_resources;
    c_typed_tag_block<unsigned long> unused_resources;
    c_typed_tag_block<unsigned long> optional_resources;
    c_typed_tag_block<unsigned long> streamed_resources;
    s_cache_file_zone_resource_usage overall_usage;
    string_id name;
    c_typed_tag_block<s_cache_file_zone_resource_usage> resource_usage;
    c_typed_tag_block<unsigned long> active_resource_owners;
    c_typed_tag_block<unsigned long> top_level_resource_owners;
    s_tag_block visitation_hierarchy;
};

struct s_tag_persistent_identifier
{
    unsigned long parts[4];
};
// The resource-type flags are read at +0x10 by 0x1afda8.
struct s_cache_file_tag_resource_type_definition
{
    s_tag_persistent_identifier identifier;
    unsigned long flags;
    short pageable_alignment_bits;
    short optional_alignment_bits;
    string_id name;
};
struct s_cache_file_runtime_location { long value; };
struct s_cache_file_runtime_block { long count; TAG_POINTER(void) elements; };
struct s_cache_file_runtime_interop_block
{
    long count;
    TAG_POINTER(s_tag_resource_interop_location) elements;
};
// Shared serialized record used by resource queries, interop sizing and dependency traversal.
struct s_cache_file_tag_resource_data
{
    s_tag_reference owner_tag;
    short salt;
    signed char page_index;
    char control_alignment_bits;
    long data_offset;
    long data_size;
    long secondary_fixup_information_offset;
    unsigned short flags;
    short fixup_index;
    s_cache_file_runtime_location location;
    s_cache_file_runtime_block fixups;
    TAG_POINTER(void) fixups_definition;
    s_cache_file_runtime_interop_block interop_locations;
    TAG_POINTER(void) interop_locations_definition;
};
struct s_cache_file_resource_shared_file
{
    char name[256];
    short global_page_offset;
    unsigned short flags;
    long extra;
};
struct s_cache_file_tag_resource_cursor_source_slot;
struct s_cache_file_resource_layout_table
{
    c_typed_tag_block<s_tag_persistent_identifier> codecs;
    c_typed_tag_block<s_cache_file_resource_shared_file> shared_files;
    c_typed_tag_block<long> pages;
    c_typed_tag_block<s_cache_file_tag_resource_cursor_source_slot> sizes;
    c_typed_tag_block<long> segments;
};
// the 'play' tag
struct s_cache_file_resource_layout_table_definition
{
    s_cache_file_resource_layout_table layout_table;
};
template<typename t_element>
struct c_cache_file_tag_resource_runtime_typed_tag_block
{
    __forceinline long count() const { return m_count; }
    __forceinline const t_element &operator[](long index) const { return m_elements[index]; }
    __forceinline t_element *begin() const { return m_elements; }
    __forceinline t_element *end() const { return (t_element *)m_elements + m_count; }

    long m_count;
    TAG_POINTER(t_element) m_elements;
    TAG_POINTER(void) m_definition;
};
struct s_cache_file_tag_dependency
{
    s_tag_reference tag;
    char dependency_padding[16];
    c_cache_file_tag_resource_runtime_typed_tag_block<long> child_indices;
};
struct s_cache_file_resource_gestalt
{
    short map_type;
    unsigned short flags;
    s_tag_block resource_types;
    s_tag_block interop_types;
    s_cache_file_resource_layout_table layout_table;
    s_tag_block resources;
    s_tag_block mask2;
    s_tag_block always;
    s_tag_block hs_zone_manifests;
    s_tag_block optional;
    s_tag_block dvd_forbidden_zone_manifests;
    c_typed_tag_block<s_cache_file_zone_manifest> dvd_always_streaming_zone_manifests;
    s_tag_block mask0;
    s_tag_block mask0_alternate;
    s_tag_block mask1;
    s_tag_block mask3;
    s_tag_block zones_only_zone_set_manifests;
    s_tag_block expected_zone_manifests;
    s_tag_block fully_populated_zone_manifests;
    s_tag_block zone_set_zone_usages;
    s_tag_block bsp_references;
    s_tag_block tag_references;
    s_tag_block model_variant_usages;
    s_tag_block character_usages;
    s_tag_data fixup_information;
    unsigned long optional_resource_size;
    unsigned long minimum_required_pageable_data_size;
    unsigned long minimum_required_dvd_data_size;
    unsigned long global_pageable_data_size;
    unsigned long optional_control_data_size;
    s_tag_block global_resource_usage;
    s_tag_block bsp_game_attachments;
    s_tag_block model_variant_zones;
    s_tag_block combat_dialogue_zones;
    s_tag_block tag_zones;
    s_tag_block debug_resource_definitions;
    s_tag_block resource_layouts;
    s_tag_block resource_properties;
    c_cache_file_tag_resource_runtime_typed_tag_block<s_cache_file_tag_dependency> dependencies;
    c_typed_tag_block<long> external_access_blocks[4];
    c_typed_tag_block<long> external_access_list;
    long campaign_id;
    long map_id;
};

STATIC_ASSERT(sizeof(s_cache_file_zone_resource_usage) == 0x14, zone_resource_usage_size);
STATIC_ASSERT(offsetof(s_cache_file_zone_manifest, required_resources) == 0, zone_required_resources_offset);
STATIC_ASSERT(offsetof(s_cache_file_zone_manifest, streamed_resources) == 0x24, zone_streamed_resources_offset);
STATIC_ASSERT(offsetof(s_cache_file_zone_manifest, overall_usage) == 0x30, zone_overall_usage_offset);
STATIC_ASSERT(offsetof(s_cache_file_zone_manifest, visitation_hierarchy) == 0x6c, zone_visitation_hierarchy_offset);
STATIC_ASSERT(sizeof(s_cache_file_zone_manifest) == 0x78, zone_manifest_size);
STATIC_ASSERT(sizeof(s_cache_file_runtime_interop_block) == 8, resource_interop_block_size);
STATIC_ASSERT(sizeof(s_cache_file_tag_resource_data) == 0x40, resource_data_size);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_data, owner_tag) + offsetof(s_tag_reference, index) == 0xc,
    resource_data_owner_tag_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_data, control_alignment_bits) == 0x13, resource_data_alignment_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_data, data_offset) == 0x14, resource_data_offset_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_data, flags) == 0x20, resource_data_flags_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_data, interop_locations) == 0x34, resource_data_interops_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, resources) == 0x58, resource_gestalt_resources_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, mask2) == 0x64, resource_gestalt_mask2_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, dvd_always_streaming_zone_manifests) == 0xa0, resource_access_masks_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, mask0) == 0xac, resource_gestalt_mask0_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, fixup_information) == 0x13c, resource_gestalt_fixup_information_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, external_access_blocks) == 0x1d0, resource_gestalt_external_access_offset);

STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, dependencies) == 0x1c4, resource_gestalt_dependencies_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, bsp_game_attachments) == 0x170, resource_bsp_attachments_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, model_variant_zones) == 0x17c, resource_model_variant_zones_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, combat_dialogue_zones) == 0x188, resource_dialogue_zones_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, tag_zones) == 0x194, resource_tag_zones_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, debug_resource_definitions) == 0x1a0, resource_debug_definitions_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, resource_layouts) == 0x1ac, resource_layouts_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, resource_properties) == 0x1b8, resource_properties_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, campaign_id) == 0x20c, resource_campaign_id_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, map_id) == 0x210, resource_map_id_offset);
STATIC_ASSERT(sizeof(s_cache_file_resource_gestalt) == 0x214, resource_gestalt_size);
STATIC_ASSERT(sizeof(s_cache_file_tag_dependency) == 0x2c, resource_dependency_size);
STATIC_ASSERT(offsetof(s_cache_file_tag_dependency, child_indices) == 0x20, resource_dependency_children_offset);

STATIC_ASSERT(sizeof(s_cache_file_tag_resource_type_definition) == 0x1c, resource_type_definition_size);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_type_definition, flags) == 0x10, resource_type_flags_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_type_definition, pageable_alignment_bits) == 0x14,
    resource_type_pageable_alignment_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_type_definition, optional_alignment_bits) == 0x16,
    resource_type_optional_alignment_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_type_definition, name) == 0x18, resource_type_name_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_data, secondary_fixup_information_offset) == 0x1c, resource_secondary_fixup_offset);
STATIC_ASSERT(offsetof(s_cache_file_resource_gestalt, global_resource_usage) == 0x164, resource_global_usage_offset);

#endif // CACHE_CACHE_FILE_RESOURCE_GESTALT_H
