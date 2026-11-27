#ifndef STRUCTURES_STRUCTURE_BSP_TAG_H
#define STRUCTURES_STRUCTURE_BSP_TAG_H

#include <stddef.h>
#include "cseries/cseries_types.h"
#include "math/real_math_types.h"
#include "geometry/render_geometry_types.h"
#include "physics/collision_instanced_geometry_definition_types.h"
#include "structures/structure_runtime_resources.h"
#include "structures/structure_bsp_seam_types.h"
#include "tag_files/tag_types.h"

// Element of structure_bsp.widget_references: the marker a widget attaches to and the widget tag.
struct s_structure_bsp_widget_reference {
    short marker;
    s_tag_reference widget;
};
typedef char structure_bsp_widget_reference_size[(sizeof(s_structure_bsp_widget_reference) == 20) ? 1 : -1];

struct s_structure_cluster_cubemap_info
{
    real_point3d position;
    short scenario_cubemap_index;
    short cubemap_bitmap_index;
};

struct structure_cluster
{
    real_rectangle3d bounds;
    signed char scenario_sky_index;
    signed char atmosphere_index;
    signed char camera_fx_index;
    signed char weather_index;
    short acoustics;
    short acoustics_sound_cluster_index;
    short background_sound_index;
    short sound_environment_index;
    short weather_palette_index;
    short background_sound_sound_cluster_index;
    short reverb_sound_cluster_index;
    short runtime_first_decal_index;
    short runtime_decal_count;
    unsigned short flags;
    s_tag_block predicted_resources;
    s_tag_block portal_indices;
    s_collision_instanced_geometry_definition collision_instanced_geometry;
    short mesh_index;
    short unused;
    s_tag_block seam_indices;
    s_tag_block decorator_groups;
    s_tag_block pvs_bound_object_identifiers;
    s_tag_block pvs_bound_object_references;
    s_tag_block cluster_cubemaps;
};

struct structure_atmosphere_palette_entry
{
    string_id name;
    unsigned short atmosphere_setting_index;
    unsigned short pad;
};
typedef char structure_atmosphere_palette_entry_size[(sizeof(structure_atmosphere_palette_entry) == 8) ? 1 : -1];

struct s_structure_physics
{
    s_tag_block mopp_code_block;
    long unused;
    real_point3d mopp_bounds_min;
    real_point3d mopp_bounds_max;
    s_tag_block breakable_surfaces_mopp_code_block;
    s_tag_block breakable_surface_key_table;
};

struct s_structure_bsp_resource
{
    const s_structure_bsp_resources *get_resources() const;
    s_tag_block raw_resources;
    union
    {
        long collision_resource_handle;
        c_structure_bsp_resource_interface collision_resource_interface;
    };
    TAG_POINTER(void) collision_resource_definition;
    long use_resource_items;
};

// BSP tag with the collision resource interface in the trailing resource field.
// Each cluster ends with its per-cluster cubemaps.
// bytes (cubemaps at 0xd0). Both builds put the clusters block at BSP +0xb4.
struct structure_bsp
{
    long import_info_checksum;
    long import_version;
    long unused;
    c_typed_tag_block<s_structure_seam> seam_identifiers;
    c_typed_tag_block<s_structure_seam_edge_mapping> edge_to_seam_edge;
    union
    {
        s_tag_block collision_materials;
        // collision_features.cpp reads 0x18-byte surface seam records through BSP +0x28.
        s_tag_block surface_seam_links;
    };
    s_tag_block leaves;
    real_rectangle3d world_bounds;
    s_tag_block structure_surfaces;
    s_tag_block structure_surface_to_triangle_mapping;
    s_tag_block cluster_portals;
    s_tag_block weather_palette;
    s_tag_block atmosphere_palette;
    s_tag_block camera_fx_palette;
    s_tag_block weather_polyhedra;
    s_tag_block detail_objects;
    s_tag_block clusters;
    s_tag_block materials;
    s_tag_block sky_owner_cluster;
    s_tag_block conveyor_surfaces;
    s_tag_block breakable_surfaces;
    s_tag_block pathfinding_data;
    s_tag_block pathfinding_edges;
    s_tag_block acoustics_palette;
    s_tag_block background_sound_palette;
    s_tag_block sound_environment_palette;
    s_tag_data sound_pas_data;
    s_tag_block markers;
    s_tag_block marker_light_palette;
    s_tag_block marker_light_palette_index;
    s_tag_block runtime_decals;
    s_tag_block environment_object_palette;
    s_tag_block environment_objects;
    long leaf_map_unused;
    s_tag_block leaf_map_leaves;
    s_tag_block leaf_map_connections;
    s_tag_block errors;
    s_tag_block instanced_geometry_instances;
    s_tag_block decorator_sets;
    s_render_geometry decorator_instance_buffer;
    s_tag_block acoustics_sound_clusters;
    s_tag_block ambience_sound_clusters;
    s_tag_block reverb_sound_clusters;
    s_tag_block transparent_planes;
    s_tag_block debug_info;
    s_structure_physics structure_physics;
    s_tag_block audibility;
    s_tag_block object_fake_lightprobes;
    s_render_geometry render_geometry;
    s_tag_block widget_references;
    s_structure_bsp_resource resource;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char structure_cubemap_info_size[(sizeof(s_structure_cluster_cubemap_info) == 0x10) ? 1 : -1];
typedef char structure_cluster_size[(sizeof(structure_cluster) == 0xdc) ? 1 : -1];
typedef char structure_cluster_mesh_offset[(offsetof(structure_cluster, mesh_index) == 0x9c) ? 1 : -1];
typedef char structure_cluster_cubemaps_offset[(offsetof(structure_cluster, cluster_cubemaps) == 0xd0) ? 1 : -1];
typedef char structure_physics_size[(sizeof(s_structure_physics) == 0x40) ? 1 : -1];
typedef char structure_bsp_size[(sizeof(structure_bsp) == 0x388) ? 1 : -1];
typedef char structure_bsp_clusters_offset[(offsetof(structure_bsp, clusters) == 0xb4) ? 1 : -1];
typedef char structure_bsp_resources_offset[(offsetof(structure_bsp, resource) == 0x370) ? 1 : -1];
typedef char structure_bsp_resource_size[(sizeof(s_structure_bsp_resource) == 0x18) ? 1 : -1];
typedef char structure_bsp_resource_interface_offset[(offsetof(s_structure_bsp_resource, collision_resource_handle) == 0xc) ? 1 : -1];
typedef char structure_bsp_resource_pageable_offset[(offsetof(s_structure_bsp_resource, use_resource_items) == 0x14) ? 1 : -1];
#endif

#endif // STRUCTURES_STRUCTURE_BSP_TAG_H
