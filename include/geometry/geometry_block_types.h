#ifndef GEOMETRY_GEOMETRY_BLOCK_TYPES_H
#define GEOMETRY_GEOMETRY_BLOCK_TYPES_H
#include <stddef.h>
#include "tag_files/tag_types.h"
struct s_geometry_entry;

// Render geometry API resource: four serialized tag blocks. The sampler reads
// vertex buffers at +0x1c; rasterizer_geometry reads index buffers at +0x28.
// TagTool Tags/Resources/RenderGeometryApiResourceDefinition.cs agrees with both.
struct s_geometry_block {
    s_tag_block textures;
    s_tag_block interleaved_textures;
    long vertex_buffer_count;
    TAG_POINTER(s_geometry_entry) vertex_entries;
    TAG_POINTER(void) vertex_buffer_definition;
    long index_buffer_count;
    TAG_POINTER(s_geometry_entry) entries;
    TAG_POINTER(void) index_buffer_definition;
};
typedef char geometry_resource_size[(sizeof(s_geometry_block) == 0x30) ? 1 : -1];
typedef char geometry_vertex_entries_offset[(offsetof(s_geometry_block, vertex_entries) == 0x1c) ? 1 : -1];
typedef char geometry_index_entries_offset[(offsetof(s_geometry_block, entries) == 0x28) ? 1 : -1];
#endif
