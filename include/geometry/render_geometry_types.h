#ifndef GEOMETRY_RENDER_GEOMETRY_TYPES_H
#define GEOMETRY_RENDER_GEOMETRY_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"

// Serialized geometry shared by render models and structure BSPs. The ten blocks and
// the eight-byte resource reference make up the serialized geometry.
struct s_render_geometry
{
    unsigned long runtime_flags;
    s_tag_block meshes;
    s_tag_block compression_info;
    s_tag_block part_sorting_position;
    s_tag_block user_data;
    s_tag_block per_mesh_temporary;
    s_tag_block per_mesh_mopp;
    s_tag_block per_mesh_node_map;
    s_tag_block per_mesh_subpart_visibility;
    s_tag_block per_mesh_prt_data;
    s_tag_block per_instance_lightmap_texcoords;
    long api_resource_handle;
    TAG_POINTER(void) api_resource_definition;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char render_geometry_size[(sizeof(s_render_geometry) == 0x84) ? 1 : -1];
typedef char render_geometry_resource_offset[(offsetof(s_render_geometry, api_resource_handle) == 0x7c) ? 1 : -1];
#endif

#endif // GEOMETRY_RENDER_GEOMETRY_TYPES_H
